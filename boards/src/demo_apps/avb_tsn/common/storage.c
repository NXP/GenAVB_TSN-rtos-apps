/*
 * Copyright 2020, 2022-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include "storage.h"

#ifdef CONFIG_APP_STORAGE

#include "genavb/helpers.h"

#include "common.h"
#include "flexspi_ops.h"
#include "lfs.h"
#include "shell.h"
#include <stdio.h>
#include <errno.h>

#include "lfs.h"

struct storage_ctx {
    lfs_t lfs;
    void *shell;
};

static struct storage_ctx storage;
static char current_dir[MAX_PWD_LENGTH] = "/";
static char old_dir[MAX_PWD_LENGTH] = "/";
static char absolute_path[MAX_PATH_LENGTH];

extern int lfs_get_default_config(const struct lfs_config **lfsc);

static void canonical_path(void *shell, char *path)
{
    char *start, *end;
    unsigned int len;

    start = path;

    /* skip initial / */
    if (start[0] == '/')
        start++;

    while (true) {
        end = strchr(start, '/');

        if (!end) {
            len = strlen(start);
            if (!len)
                break;

            end = start + len - 1;
        } else {
            len = end - start;
        }

        if ((len == 2) && !memcmp(start, "..", 2)) {
           /* go back */
           if (start > path + 1) {
               start--;
               while (start > path) {
                   if ((start - 1)[0] == '/')
                       break;
                   start--;
               }
           }

           len = strlen(end + 1);
           memmove(start, end + 1, len + 1);
        } else if (((len == 1) && !memcmp(start, ".", 1)) ||
            (len == 0)) {
           /* skip */
           len = strlen(end + 1);
           memmove(start, end + 1, len + 1);
        } else {
            /* go forward */
            start = end + 1;
        }
    }
}


static int get_absolute_path(const char *pwd, const char *path, char *absolute_path, unsigned int len)
{
    unsigned int pwd_len;
    unsigned int path_len = strlen(path);

    if (path[0] == '/') {
        if (path_len >= MAX_PATH_LENGTH)
            return -1;

        memcpy(absolute_path, path, path_len + 1);
    } else {
        pwd_len = strlen(pwd);

        if ((pwd_len + 1 + path_len) >= MAX_PATH_LENGTH)
            return -1;

        memcpy(absolute_path, pwd, pwd_len);
        absolute_path[pwd_len] = '/';
        memcpy(absolute_path + pwd_len + 1, path, path_len + 1);
    }

    return 0;
}

static int __storage_read_uint(const char *filename, uint64_t *value)
{
    char buf[MAX_FILE_SIZE + 1];
    uint64_t tmp;
    int rc;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    errno = 0;

    tmp = strtoull(buf, NULL, 0);

    if (errno)
        return -1;

    *value = tmp;

    return 0;
}

static int __storage_read_int(const char *filename, int64_t *value)
{
    char buf[MAX_FILE_SIZE + 1];
    int64_t tmp;
    int rc;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    errno = 0;

    tmp = strtoll(buf, NULL, 0);

    if (errno)
        return -1;

    *value = tmp;

    return 0;
}

int storage_read_ipv4_address(const char *filename, uint8_t *addr)
{
    char buf[MAX_FILE_SIZE + 1];
    unsigned int tmp[4];
    int rc;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    if (sscanf(buf, "%u.%u.%u.%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) != 4)
        return -1;

    addr[0] = tmp[0];
    addr[1] = tmp[1];
    addr[2] = tmp[2];
    addr[3] = tmp[3];

    return 0;
}

int storage_read_mac_address(const char *filename, uint8_t *mac)
{
    char buf[MAX_FILE_SIZE + 1];
    int rc;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    if (str2mac(buf, mac) < 0)
        return -1;

    return 0;
}

int storage_read_qbv_entry(const char *filename, uint8_t *mask, uint32_t *offset, uint8_t *state)
{
    char buf[MAX_FILE_SIZE + 1];
    unsigned int tmp[3];
    int rc, arg_val;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    arg_val = sscanf(buf, "%2x,%u,%u", &tmp[0], &tmp[1], &tmp[2]);

    if (arg_val < 2)
        return -1;

    *mask = tmp[0];
    *offset = tmp[1];

    if (arg_val == 3)
        *state = tmp[2];

    return 0;
}

int storage_read_uint(const char *filename, unsigned int *value)
{
    uint64_t tmp;

    if (__storage_read_uint(filename, &tmp) < 0)
        return -1;

    *value = (unsigned int)tmp;

    return 0;
}

int storage_read_int(const char *filename, int *value)
{
    int64_t tmp;

    if (__storage_read_int(filename, &tmp) < 0)
        return -1;

    *value = (int)tmp;

    return 0;
}

int storage_read_float(const char *filename, float *value)
{
    char buf[MAX_FILE_SIZE + 1];
    float tmp;
    int rc;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    errno = 0;

    tmp = strtof(buf, NULL);

    if (errno)
        return -1;

    *value = tmp;

    return 0;
}

int storage_read_bool(const char *filename, bool *value)
{
    uint64_t tmp;

    if (__storage_read_uint(filename, &tmp) < 0)
        return -1;

    *value = (bool)tmp;

    return 0;
}

int storage_read_u8(const char *filename, uint8_t *value)
{
    uint64_t tmp;

    if (__storage_read_uint(filename, &tmp) < 0)
        return -1;

    *value = (uint8_t)tmp;

    return 0;
}

int storage_read_u16(const char *filename, uint16_t *value)
{
    uint64_t tmp;

    if (__storage_read_uint(filename, &tmp) < 0)
        return -1;

    *value = (uint16_t)tmp;

    return 0;
}

int storage_read_u32(const char *filename, uint32_t *value)
{
    uint64_t tmp;

    if (__storage_read_uint(filename, &tmp) < 0)
        return -1;

    *value = (uint32_t)tmp;

    return 0;
}

int storage_read_u64(const char *filename, uint64_t *value)
{
    uint64_t tmp;

    if (__storage_read_uint(filename, &tmp) < 0)
        return -1;

    *value = tmp;

    return 0;
}

int storage_read_s8(const char *filename, int8_t *value)
{
    int64_t tmp;

    if (__storage_read_int(filename, &tmp) < 0)
        return -1;

    *value = (int8_t)tmp;

    return 0;
}

int storage_read_s16(const char *filename, int16_t *value)
{
    int64_t tmp;

    if (__storage_read_int(filename, &tmp) < 0)
        return -1;

    *value = (int16_t)tmp;

    return 0;
}

int storage_read_s32(const char *filename, int32_t *value)
{
    int64_t tmp;

    if (__storage_read_int(filename, &tmp) < 0)
        return -1;

    *value = (int32_t)tmp;

    return 0;
}

int storage_write_uint_hex(const char *filename, unsigned int value)
{
    char hex_str[12];

    if (h_snprintf_strict(hex_str, 12, "0x%08x", value) < 0)
        return -1;

    return storage_write(filename, hex_str, strlen(hex_str) + 1);
}

int storage_write_uint(const char *filename, unsigned int value)
{
    char str[11];

    if (h_snprintf_strict(str, sizeof(str), "%u", value) < 0)
        return -1;

    return storage_write(filename, str, strlen(str) + 1);
}

int storage_write_u64(const char *filename, uint64_t value)
{
    char str[22];
    unsigned int tmp[3];

    tmp[0] = value % 1000000000ULL;
    value /= 1000000000ULL;
    tmp[1] = value % 1000000000ULL;
    tmp[2] = value / 1000000000ULL;

    if (tmp[2]) {
        if (h_snprintf_strict(str, sizeof(str), "%u%09u%09u", tmp[2], tmp[1], tmp[0]) < 0)
            return -1;
    } else if (tmp[1]) {
        if (h_snprintf_strict(str, sizeof(str), "%u%09u", tmp[1], tmp[0]) < 0)
            return -1;
    } else {
        if (h_snprintf_strict(str, sizeof(str), "%u", tmp[0]) < 0)
            return -1;
    }

    return storage_write(filename, str, strlen(str) + 1);
}

/* get nth dir/file from dirname */
static int storage_get(const char *dirname, unsigned int n, struct lfs_info *info)
{
    void *shell = storage.shell;
    lfs_t *lfs = &storage.lfs;
    lfs_dir_t dir;
    int rc = -1;

    if (lfs_dir_open(lfs, &dir, dirname) < 0) {
        goto err;
    }

    /* offset 2 for '.' and '..' */
    if (lfs_dir_seek(lfs, &dir, n + 2) < 0) {
        goto err_close;
    }

    if (lfs_dir_read(lfs, &dir, info) > 0) {
        rc = 0;
    }

err_close:
    if (lfs_dir_close(lfs, &dir) < 0) {
        shell_printf(shell, "lfs_dir_close failed: %d\n");
        rc = -1;
        goto err;
    }

err:
    return rc;
}

int storage_get_dir(const char *dirname, unsigned int n, char *subdirname, unsigned int len)
{
    struct lfs_info info;
    int rc = -1;

    rc = storage_get(dirname, n, &info);
    if (rc < 0)
        goto err;

    if (info.type == LFS_TYPE_DIR) {
        if ((strlen(info.name) + 1) > len) {
            rc = -1;
            goto err;
        }

        memcpy(subdirname, &info.name, strlen(info.name) + 1);
    } else {
        rc = -1;
        goto err;
    }

    return rc;

err:
    subdirname = NULL;
    return rc;
}

int storage_get_file(const char *dirname, unsigned int n, char *filename, unsigned int len)
{
    struct lfs_info info;
    int rc = -1;

    rc = storage_get(dirname, n, &info);
    if (rc < 0)
        goto err;

    if (info.type == LFS_TYPE_REG) {
        if ((strlen(info.name) + 1) > len) {
            rc = -1;
            goto err;
        }

        memcpy(filename, &info.name, strlen(info.name) + 1);
    } else {
        rc = -1;
        goto err;
    }

    return rc;

err:
    filename = NULL;
    return rc;
}

int storage_pwd(void)
{
    void *shell = storage.shell;

    shell_printf(shell, "%s\n", current_dir);

    return 0;
}

int storage_cd(const char *dir, bool quiet)
{
    lfs_t *lfs = &storage.lfs;
    struct lfs_info info;
    void *shell = storage.shell;
    int len;

    if (!strcmp(dir, "-"))
        dir = old_dir;

    if (get_absolute_path(current_dir, dir, absolute_path, MAX_PATH_LENGTH) < 0)
        goto err;

    canonical_path(shell, absolute_path);

    if (lfs_stat(lfs, absolute_path, &info) < 0) {
        if (!quiet)
            shell_printf(shell, "directory %s doesn't exist\n", absolute_path);

        goto err;
    }

    if (info.type != LFS_TYPE_DIR) {
        if (!quiet)
            shell_printf(shell, "directory %s not a directory\n", absolute_path);

        goto err;
    }

    len = strlen(absolute_path);
    if (len >= MAX_PWD_LENGTH)
        goto err;

    strcpy(old_dir, current_dir);
    strcpy(current_dir, absolute_path);

    return 0;

err:
    return -1;
}

static void ls_file(struct lfs_info *info)
{
    void *shell = storage.shell;

    shell_printf(shell, "%8d %s\r\n", info->size, info->name);
}

static int ls_dir(lfs_t *lfs, const char *dirname)
{
    void *shell = storage.shell;
    struct lfs_info info;
    lfs_dir_t dir;
    int rc;

    rc = lfs_dir_open(lfs, &dir, dirname);
    if (rc < 0) {
        shell_printf(shell, "lfs_dir_open(%s) failed: %d\n", dirname, rc);
        goto err;
    }

    /* iterate until end of directory */
    while (lfs_dir_read(lfs, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            ls_file(&info);
        } else if (info.type == LFS_TYPE_DIR) {
            shell_printf(shell, "%     DIR %s\r\n", info.name);
        } else {
            shell_printf(shell, "%???\r\n");
        }
    }

    rc = lfs_dir_close(lfs, &dir);
    if (rc < 0) {
        shell_printf(shell, "lfs_dir_close(%s) failed: %d\n", dirname, rc);
        goto err;
    }

err:
    return -1;
}

int storage_ls(const char *name)
{
    void *shell = storage.shell;
    lfs_t *lfs = &storage.lfs;
    struct lfs_info info;
    int rc;

    if (get_absolute_path(current_dir, name, absolute_path, MAX_PATH_LENGTH) < 0)
        goto err;

    rc = lfs_stat(lfs, absolute_path, &info);
    if (rc < 0) {
        shell_printf(shell, "lfs_stat(%s) failed: %d\n", absolute_path, rc);
        goto err;
    }

    if (info.type == LFS_TYPE_REG) {
        ls_file(&info);
    } else if (info.type == LFS_TYPE_DIR) {
        if (ls_dir(lfs, absolute_path) < 0)
            goto err;
    }

    return 0;

err:
    return -1;
}

static int rm_file(lfs_t *lfs, const char *filename)
{
    void *shell = storage.shell;
    int rc;

    rc = lfs_remove(lfs, filename);
    if (rc < 0)
        shell_printf(shell, "lfs_remove(%s) failed: %d\n", filename, rc);

    return rc;
}

static int rm_dir(lfs_t *lfs, char *dirname, unsigned int off, unsigned int end, struct lfs_info *info)
{
    void *shell = storage.shell;
    lfs_dir_t dir;
    unsigned int len;
    int rc;

    rc = lfs_dir_open(lfs, &dir, dirname);
    if (rc < 0) {
        shell_printf(shell, "lfs_dir_open(%s) failed: %d\n", dirname, rc);
        goto err;
    }

    /* iterate until end of directory */
    while (lfs_dir_read(lfs, &dir, info) > 0) {
        len = h_snprintf_strict(dirname + off, end - off, "/%s", info->name);
        if (len == -1) {
            shell_printf(shell, "directory tree (%s/%s) too deep\n", dirname, info->name);
            goto err_close;
        }

        if (info->type == LFS_TYPE_REG) {
            rm_file(lfs, dirname);
        } else if ((info->type == LFS_TYPE_DIR) &&
                   strcmp(info->name, ".") &&
                   strcmp(info->name, "..")) {
            rm_dir(lfs, dirname, off + len, end, info);
        }
    }

    dirname[off] = '\0';
    if (rm_file(lfs, dirname) < 0)
        goto err_close;

    rc = lfs_dir_close(lfs, &dir);
    if (rc < 0) {
        shell_printf(shell, "lfs_dir_close(%s) failed: %d\n", dirname, rc);
        goto err;
    }

err_close:
    lfs_dir_close(lfs, &dir);

err:
    return rc;
}

int storage_rm(const char *filename, bool recursive, bool force)
{
    void *shell = storage.shell;
    lfs_t *lfs = &storage.lfs;
    struct lfs_info info;
    int rc;

    if (get_absolute_path(current_dir, filename, absolute_path, MAX_PATH_LENGTH) < 0)
        goto err;

    rc = lfs_stat(lfs, absolute_path, &info);
    if (rc < 0) {
        if (!force) {
            shell_printf(shell, "lfs_stat(%s) failed: %d\n", absolute_path);
            goto err;
        }

        goto out;
    }

    if ((info.type == LFS_TYPE_REG) || !recursive) {
        if (rm_file(lfs, absolute_path) < 0)
            goto err;
    } else if (info.type == LFS_TYPE_DIR) {
        if (rm_dir(lfs, absolute_path, strlen(absolute_path), MAX_PATH_LENGTH, &info) < 0)
            goto err;
    }

out:
    return 0;

err:
    return -1;
}

int storage_cat(const char *filename)
{
    lfs_t *lfs = &storage.lfs;
    void *shell = storage.shell;
    lfs_file_t file;
    char buf[MAX_FILE_SIZE + 1];
    int rc;

    if (get_absolute_path(current_dir, filename, absolute_path, MAX_PATH_LENGTH) < 0)
        goto err_path;

    rc = lfs_file_open(lfs, &file, absolute_path, 0);
    if (rc < 0) {
        shell_printf(shell, "lfs_file_open(%s) failed: %d\n", absolute_path, rc);
        goto err_open;
    }

    while (1) {
        rc = lfs_file_read(lfs, &file, buf, MAX_FILE_SIZE);
        if (rc <= 0) {
            if (!rc)
                goto done;

            shell_printf(shell, "lfs_file_read() failed: %d\n", rc);
            goto err_read;
        }

        buf[rc] = '\0';

        shell_printf(shell, "%s", buf);
    }

done:
    shell_printf(shell, "\n");

    lfs_file_close(lfs, &file);

    return rc;

err_read:
    lfs_file_close(lfs, &file);

err_open:
err_path:
    return -1;
}

static int storage_mkdir_p(lfs_t *lfs, char *dirname)
{
    char partial_path[MAX_PWD_LENGTH] = {0};
    void *shell = storage.shell;
    struct lfs_info info;
    char *dir;
    int rc, off = 0;

    canonical_path(shell, dirname);

    dir = strtok(dirname, "/");
    while (dir) {
        rc = h_snprintf_strict(partial_path + off, MAX_PWD_LENGTH - off, "/%s", dir);
        if (rc < 0)
            goto err;

        off += rc;

        rc = lfs_mkdir(lfs, partial_path);
        if (rc < 0) {
            if (rc == LFS_ERR_EXIST) {
                if (lfs_stat(lfs, partial_path, &info) == 0) {
                    /* file or dir exists */
                    if (info.type == LFS_TYPE_REG) {
                        /* file -> error */
                        shell_printf(shell, "lfs_mkdir(%s) failed: %d\n", partial_path, rc);
                        goto err;
                    } else if (info.type == LFS_TYPE_DIR) {
                        /* directory -> continue */
                        goto next;
                    }
                }
            } else {
                shell_printf(shell, "lfs_mkdir(%s) failed: %d\n", partial_path, rc);
                goto err;
            }
        }
next:
        dir = strtok(NULL, "/");
    }

    return 0;

err:
    return -1;
}

int storage_mkdir(const char *dirname, bool parent)
{
    lfs_t *lfs = &storage.lfs;
    void *shell = storage.shell;
    int rc = 0;

    if (get_absolute_path(current_dir, dirname, absolute_path, MAX_PATH_LENGTH) < 0) {
        rc = -1;
        goto err_path;
    }

    if (parent) {
        rc = storage_mkdir_p(lfs, absolute_path);
    } else {
        rc = lfs_mkdir(lfs, absolute_path);
        if (rc < 0)
            shell_printf(shell, "lfs_mkdir(%s) failed: %d\n", absolute_path, rc);
    }

err_path:
    return rc;
}

int storage_read(const char *filename, char *buf, unsigned int len)
{
    lfs_t *lfs = &storage.lfs;
    lfs_file_t file;
    void *shell = storage.shell;
    int rc;

    if (get_absolute_path(current_dir, filename, absolute_path, MAX_PATH_LENGTH) < 0)
        goto err_path;

    rc = lfs_file_open(lfs, &file, absolute_path, 0);
    if (rc < 0) {
        goto err_open;
    }

    rc = lfs_file_read(lfs, &file, buf, len);
    if (rc < 0) {
        shell_printf(shell, "lfs_file_read() failed: %d\n", rc);
        goto err_read;
    }

    lfs_file_close(lfs, &file);

    return rc;

err_read:
    lfs_file_close(lfs, &file);

err_open:
err_path:
    return -1;
}

int storage_write(const char *filename, const char *buf, unsigned int len)
{
    lfs_t *lfs = &storage.lfs;
    lfs_file_t file;
    void *shell = storage.shell;
    int rc;

    if (get_absolute_path(current_dir, filename, absolute_path, MAX_PATH_LENGTH) < 0)
        goto err_path;

    rc = lfs_file_open(lfs, &file, absolute_path, LFS_O_CREAT);
    if (rc < 0) {
         shell_printf(shell, "lfs_file_open(%s) failed: %d\n", absolute_path, rc);
         goto err_open;
    }

    rc = lfs_file_write(lfs, &file, buf, len);
    if (rc < 0) {
        shell_printf(shell, "lfs_file_write(%s) failed: %d\n", absolute_path, rc);
        goto err_write;
    }

    lfs_file_close(lfs, &file);

    return 0;

err_write:
    lfs_file_close(lfs, &file);

err_open:
err_path:
    return -1;
}

int storage_init(void)
{
     lfs_t *lfs = &storage.lfs;
     const struct lfs_config *config;
     void *shell = NULL;
     int rc;

     storage.shell = shell;

     flexspi_init();

     lfs_get_default_config(&config);

#if 0
    shell_printf(shell, "Formatting filesystem\n");
    rc = lfs_format(lfs, config);
    if (rc < 0) {
        shell_printf(shell, "lfs_format() failed: %d\n", rc);
    }
#endif

    shell_printf(shell, "Mounting filesystem\n");
    rc = lfs_mount(lfs, config);
    if (rc < 0) {
        shell_printf(shell, "lfs_mount() failed: %d\n", rc);
        shell_printf(shell, "Formatting filesystem\n");

        rc = lfs_format(lfs, config);
        if (rc < 0) {
             shell_printf(shell, "lfs_format() failed: %d\n", rc);
             goto err;
        }

        shell_printf(shell, "Mounting filesystem\n");
        rc = lfs_mount(lfs, config);
        if (rc < 0) {
            shell_printf(shell, "lfs_mount() failed: %d\n", rc);
            goto err;
        }
    }

    return 0;

err:
    return -1;
}

int storage_set_shell(void *shell)
{
    storage.shell = shell;

    return 0;
}

void storage_exit(void)
{
    lfs_t *lfs = &storage.lfs;

    lfs_unmount(lfs);
}

#else

int storage_read_ipv4_address(const char *filename, uint8_t *addr) {return -1;}
int storage_read_mac_address(const char *filename, uint8_t *mac) {return -1;}
int storage_read_qbv_entry(const char *filename, uint8_t *mask, uint32_t *offset, uint8_t *state) {return -1;}
int storage_read_uint(const char *filename, unsigned int *value) {return -1;}
int storage_read_int(const char *filename, int *value) {return -1;}
int storage_read_float(const char *filename, float *value) {return -1;}
int storage_read_bool(const char *filename, bool *value) {return -1;}
int storage_read_u8(const char *filename, uint8_t *value) {return -1;}
int storage_read_u16(const char *filename, uint16_t *value) {return -1;}
int storage_read_u32(const char *filename, uint32_t *value) {return -1;}
int storage_read_u64(const char *filename, uint64_t *value) {return -1;}
int storage_read_s8(const char *filename, int8_t *value) {return -1;}
int storage_read_s16(const char *filename, int16_t *value) {return -1;}
int storage_read_s32(const char *filename, int32_t *value) {return -1;}
int storage_write_uint_hex(const char *filename, unsigned int value) {return -1;}
int storage_write_uint(const char *filename, unsigned int value) {return -1;}
int storage_write_u64(const char *filename, uint64_t value) {return -1;}
int storage_pwd(void) {return -1;}
int storage_cd(const char *filename, bool quiet) {return -1;}
int storage_ls(const char *filename) {return -1;}
int storage_rm(const char *filename, bool recursive, bool force) {return -1;}
int storage_mkdir(const char *dirname, bool parent) {return -1;}
int storage_cat(const char *filename) {return -1;}
int storage_read(const char *filename, char *buf, unsigned int len) {return -1;}
int storage_write(const char *filename, const char *buf, unsigned int len) {return -1;}
int storage_init(void) {return -1;}
int storage_set_shell(void *shell) {return -1;}
void storage_exit(void) {return;}
int storage_get_dir(const char *dirname, unsigned int n, char *subdirname, unsigned int len) {return -1;}
int storage_get_file(const char *dirname, unsigned int n, char *filename, unsigned int len) {return -1;}

#endif
