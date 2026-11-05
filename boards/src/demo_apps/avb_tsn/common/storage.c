/*
 * Copyright 2020, 2022-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdint.h>

#include "rtos_apps/storage/storage_app.h"
#include "rtos_apps/storage/storage.h"
#include "storage.h"

#ifdef CONFIG_APP_STORAGE

#include "genavb/helpers.h"

#include "rtos_apps/shell/common.h"
#include "common.h"
#include "lfs.h"
#include "shell.h"
#include "shell_config.h"
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
extern int lfs_storage_init(void);

void *storage_get_lfs(void)
{
    return &storage.lfs;
}

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

int storage_read(const char *dirname, const char *filename, char *buf, unsigned int len)
{
    lfs_t *lfs = &storage.lfs;
    lfs_file_t file;
    void *shell = storage.shell;
    int rc;

    if (dirname) {
        if (h_snprintf_strict(absolute_path, MAX_PATH_LENGTH, "%s/%s", dirname, filename) < 0)
            return -1;
    } else {
        if (get_absolute_path(current_dir, filename, absolute_path, MAX_PATH_LENGTH) < 0)
            goto err_path;
    }

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

int storage_write(const char *dirname, const char *filename, const char *buf, unsigned int len)
{
    lfs_t *lfs = &storage.lfs;
    lfs_file_t file;
    void *shell = storage.shell;
    int rc;

    if (dirname) {
        if (h_snprintf_strict(absolute_path, MAX_PATH_LENGTH, "%s/%s", dirname, filename) < 0)
            return -1;
    } else {
        if (get_absolute_path(current_dir, filename, absolute_path, MAX_PATH_LENGTH) < 0)
            goto err_path;
    }

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

     lfs_storage_init();

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

#endif
