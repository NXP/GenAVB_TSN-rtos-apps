/*
 * Copyright 2020, 2022-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <stdbool.h>

#define MAX_FILE_SIZE          32
#define MAX_FILENAME_LENGTH    32
#define MAX_DIR_NAME_LEN       8

#define MAX_PWD_LENGTH         128
#define MAX_PATH_LENGTH        (MAX_PWD_LENGTH + MAX_FILENAME_LENGTH)

#ifdef CONFIG_APP_STORAGE
int storage_init(void);
int storage_set_shell(void *shell);
void storage_exit(void);
void *storage_get_lfs(void);

int storage_pwd(void);
int storage_cd(const char *filename, bool quiet);
int storage_ls(const char *filename);
int storage_cat(const char *filename);

int storage_rm(const char *filename, bool recursive, bool force);
int storage_mkdir(const char *dirname, bool parent);

#else
static inline int storage_init(void) {return -1;}
static inline int storage_set_shell(void *shell) {return -1;}
static inline void storage_exit(void) {return;}
static inline void *storage_get_lfs(void) {return NULL;}

static inline int storage_pwd(void) {return -1;}
static inline int storage_cd(const char *filename, bool quiet) {return -1;}
static inline int storage_ls(const char *filename) {return -1;}
static inline int storage_cat(const char *filename) {return -1;}

static inline int storage_rm(const char *filename, bool recursive, bool force) {return -1;}
static inline int storage_mkdir(const char *dirname, bool parent) {return -1;}

#endif /* CONFIG_APP_STORAGE */

#endif /* _STORAGE_H_ */
