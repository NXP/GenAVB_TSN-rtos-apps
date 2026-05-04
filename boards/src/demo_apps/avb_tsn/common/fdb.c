/*
 * Copyright 2022-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <ctype.h>
#include <getopt.h>

#include "genavb/error.h"
#include "genavb/fdb.h"
#include "genavb/helpers.h"

#include "board.h"
#include "common.h"
#include "fdb.h"
#include "genavb.h"
#include "log.h"
#include "storage.h"

static shell_status_t fdb_update(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t fdb_delete(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t fdb_read(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t fdb_dump(shell_handle_t shell, int32_t argc, char **argv);

SHELL_COMMAND_DEFINE(fdb_update,
                     "\nfdb_update <mac> <vid> <port_id> [-c <control>] [-p]\n"
                     "    parameters:\n"
                     "        mac: mac address\n"
                     "        vid: vlan id\n"
                     "        port_id: logical port id\n"
                     "    options:\n"
                     "        -c <control>: filtering control, 0: filtering, 1: forwarding (default)\n"
                     "        -p: update entry in permanent database\n",
                     &fdb_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fdb_delete,
                     "\nfdb_delete <mac> <vid> [-p]\n"
                     "    parameters:\n"
                     "        mac: mac address\n"
                     "        vid: vlan id\n"
                     "    options:\n"
                     "        -p: delete entry from permanent database\n",
                     &fdb_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fdb_read,
                     "\nfdb_read <mac> <vid> [-p]\n"
                     "    parameters:\n"
                     "        mac: mac address\n"
                     "        vid: vlan id\n"
                     "    options:\n"
                     "        -p: read entry from permanent database\n",
                     &fdb_read,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fdb_dump,
                     "\nfdb_dump [-p]\n"
                     "    options:\n"
                     "        -p: print permanent entries\n",
                     &fdb_dump,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_fdb(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(fdb_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fdb_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fdb_read))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fdb_dump))->pcHelpString);
}

static void fdb_print_description(shell_handle_t shell)
{
    shell_printf(shell, "\n");
    shell_printf(shell, "        mac        |  vid | dynamic | status |   forwarding   |    filtering   |\n");
    shell_printf(shell, "-------------------+------+---------+--------+----------------+----------------+\n");
}

static void fdb_print_entry(shell_handle_t shell, uint8_t *address, uint16_t vid, bool dynamic,
        struct genavb_fdb_port_map *port_map, genavb_fdb_status_t status)
{
    char buf[15];
    int i, count;

    shell_printf(shell, " " MAC_STR_FMT " |", MAC_STR(address));
    shell_printf(shell, " % 4u |", vid);
    shell_printf(shell, " % 7s |", dynamic ? " true": "false");
    shell_printf(shell, " % 6d |", status);

    /* forwarding column */
    count = 0;
    buf[0] = '\0';
    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        if (port_map[i].control == GENAVB_FDB_PORT_CONTROL_FORWARDING) {
            if (!count)
                count += h_snprintf(buf + count, 15 - count, "%u", port_map[i].port_id);
            else
                count += h_snprintf(buf + count, 15 - count, ", %u", port_map[i].port_id);
        }
    }

    shell_printf(shell, " % 14s |", buf);

    /* filtering column */
    count = 0;
    buf[0] = '\0';
    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        if (port_map[i].control == GENAVB_FDB_PORT_CONTROL_FILTERING) {
            if (!count)
                count += h_snprintf(buf + count, 15 - count, "%u", port_map[i].port_id);
            else
                count += h_snprintf(buf + count, 15 - count, ", %u", port_map[i].port_id);
        }
    }

    shell_printf(shell, " % 14s |\n", buf);
}

static void print_fdb_update_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fdb_update))->pcHelpString);
}

static int fdb_port_mask_2_port_map(uint32_t port_mask, struct genavb_fdb_port_map *map)
{
    int i;

    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        map[i].port_id = br_port_list[i];
        if (port_mask & (1 << br_port_list[i]))
            map[i].control = GENAVB_FDB_PORT_CONTROL_FORWARDING;
        else
            map[i].control = GENAVB_FDB_PORT_CONTROL_FILTERING;
    }

    return 0;
}

static int fdb_read_storage(uint8_t *mac, uint16_t vid, uint32_t *port_mask)
{
    char filename[30];

    if (h_snprintf_strict(filename, 30, "/fdb/" MAC_STR_FMT ",%u", MAC_STR(mac), vid) < 0)
        return -1;

    return storage_read_u32(filename, port_mask);
}

static int fdb_write_storage(uint8_t *mac, uint16_t vid, uint32_t port_mask)
{
    char filename[30] = {0};

    /* write '/fdb/xx:xx:xx:xx:xx:xx,vid' file with port bitmask value */
    if (h_snprintf_strict(filename, 30, "/fdb/" MAC_STR_FMT ",%u", MAC_STR(mac), vid) < 0)
        return -1;

    return storage_write_uint_hex(filename, port_mask);
}

static int fdb_get_storage_entry(unsigned int i, uint8_t *mac, uint16_t *vid, uint32_t *port_mask)
{
    char filename[MAX_FILE_SIZE];
    int rc = -1;

    if (!storage_get_file("/fdb", i, filename, MAX_FILE_SIZE)) {
        unsigned int tmp[7];
        uint32_t tmp_port_mask;

        if (sscanf(filename, MAC_STR_FMT ",%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5], &tmp[6]) != 7) {
            rc = -1;
            goto err;
        }

        mac[0] = tmp[0];
        mac[1] = tmp[1];
        mac[2] = tmp[2];
        mac[3] = tmp[3];
        mac[4] = tmp[4];
        mac[5] = tmp[5];
        *vid = tmp[6];

        if (fdb_read_storage(mac, *vid, &tmp_port_mask) < 0) {
            rc = -1;
            goto err;
        }

        *port_mask = tmp_port_mask;

        rc = 0;
    }

err:
    return rc;
}

static int fdb_delete_storage(uint8_t *mac, uint16_t vid)
{
    char filename[30];

    if (h_snprintf_strict(filename, 30, "/fdb/" MAC_STR_FMT ",%u", MAC_STR(mac), vid) < 0)
        return -1;

    return storage_rm(filename, false, true);
}

static int fdb_read_permanent(uint8_t *address, uint16_t vid, bool *dynamic,
        struct genavb_fdb_port_map *map, genavb_fdb_status_t *status)
{
    uint32_t port_mask = 0;

    if (fdb_read_storage(address, vid, &port_mask) < 0)
        return -1;

    if (fdb_port_mask_2_port_map(port_mask, map) < 0)
        return -1;

    *dynamic = false;
    *status = GENAVB_FDB_STATUS_OTHER;

    return 0;
}

static int fdb_dump_permanent(uint32_t *next, uint8_t *address, uint16_t *vid, bool *dynamic,
        struct genavb_fdb_port_map *map, genavb_fdb_status_t *status)
{
    uint32_t port_mask = 0;
    int rc;

    rc = fdb_get_storage_entry(*next, address, vid, &port_mask);
    if (rc != 0)
        goto out;

    if (fdb_port_mask_2_port_map(port_mask, map) < 0)
        return -1;

    *dynamic = false;
    *status = GENAVB_FDB_STATUS_OTHER;
    *next += 1;

out:
    return rc;
}

static int fdb_update_permanent(shell_handle_t shell, uint8_t *address, uint16_t vid, struct genavb_fdb_port_map *map)
{
    uint32_t port_mask = 0;

    if (storage_mkdir("/fdb", true) < 0) {
        return -1;
    }

    fdb_read_storage(address, vid, &port_mask);

    if (map->control == GENAVB_FDB_PORT_CONTROL_FORWARDING) {
        port_mask |= (1 << map->port_id);
    } else if (map->control == GENAVB_FDB_PORT_CONTROL_FILTERING) {
        port_mask &= ~(1 << map->port_id);
    } else {
        shell_printf(shell, "Unknown control type\n");
        return -1;
    }

    return fdb_write_storage(address, vid, port_mask);
}

static int fdb_delete_permanent(uint8_t *address, uint16_t vid)
{
    uint32_t port_mask;

    if (fdb_read_storage(address, vid, &port_mask) < 0)
        return -1;

    return fdb_delete_storage(address, vid);
}

static shell_status_t fdb_update(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_fdb_port_map port_map = {0};
    uint8_t address[6];
    uint16_t vid;
    bool permanent = false;
    unsigned long tmp;
    int opt, rc;

    if (argc < 4)
        goto err_usage;

    if (str2mac(argv[1], address) < 0) {
        shell_printf(shell, "invalid mac address format\n");
        goto err_usage;
    }

    h_strtoul(&tmp, argv[2], NULL, 0);
    vid = tmp;
    if (vid > 4095) {
        shell_printf(shell, "invalid vid value\n");
        goto err_usage;
    }

    h_strtoul(&tmp, argv[3], NULL, 0);
    port_map.port_id = tmp;
    if (port_map.port_id >= CONFIG_APP_LOGICAL_PORTS) {
        shell_printf(shell, "invalid port_id %u\n", port_map.port_id);
        goto err_usage;
    }

    port_map.control = GENAVB_FDB_PORT_CONTROL_FORWARDING;

    optind = 4;
    while ((opt = getopt(argc, argv, "c:p")) != -1) {
        switch (opt) {
        case 'c':
            h_strtoul(&tmp, optarg, NULL, 0);
            port_map.control = (genavb_fdb_port_control_t)tmp;
            break;
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (port_map.control > GENAVB_FDB_PORT_CONTROL_FORWARDING) {
        shell_printf(shell, "invalid control value\n");
        goto err_usage;
    }

    if (permanent) {
        if (fdb_update_permanent(shell, address, vid, &port_map) < 0) {
            shell_printf(shell, "fdb_update_permanent(" MAC_STR_FMT ", %u, %u) failed\n",
                MAC_STR(address), vid, port_map.port_id);
            goto err;
        }
    }

    rc = genavb_fdb_update(address, vid, &port_map);
    if (rc < 0) {
        shell_printf(shell, "genavb_fdb_update(" MAC_STR_FMT ", %u, %u) failed: %s\n",
            MAC_STR(address), vid, port_map.port_id, genavb_strerror(rc));
        goto err;
    }

    shell_printf(shell, "FDB update port(%u) address(" MAC_STR_FMT ") vid(%u) control(%u) permanent(%u)\n",
        port_map.port_id,
        MAC_STR(address),
        vid, port_map.control, permanent);

    return kStatus_SHELL_Success;

err_usage:
        print_fdb_update_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_fdb_read_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fdb_read))->pcHelpString);
}

static shell_status_t fdb_read(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_fdb_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    genavb_fdb_status_t status;
    uint8_t address[6];
    uint16_t vid;
    bool dynamic;
    bool permanent = false;
    unsigned long tmp;
    int opt, rc;

    if (argc < 3)
        goto err_usage;

    if (str2mac(argv[1], address) < 0) {
        shell_printf(shell, "invalid mac address format\n");
        goto err_usage;
    }

    h_strtoul(&tmp, argv[2], NULL, 0);
    vid = tmp;
    if (vid > 4095) {
        shell_printf(shell, "invalid vid value\n");
        goto err_usage;
    }

    optind = 3;
    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (permanent) {
        if (fdb_read_permanent(address, vid, &dynamic, port_map, &status) < 0) {
            shell_printf(shell, "fdb_read_permanent(" MAC_STR_FMT ", %u) failed\n",
                MAC_STR(address), vid);
            goto err;
        }
    } else {
        rc = genavb_fdb_read(&address[0], vid, &dynamic, port_map, &status);
        if (rc < 0) {
            shell_printf(shell, "genavb_fdb_read(" MAC_STR_FMT ", %u) failed: %s\n",
                MAC_STR(address), vid, genavb_strerror(rc));
            goto err;
        }
    }

    fdb_print_description(shell);
    fdb_print_entry(shell, address, vid, dynamic, port_map, status);

    return kStatus_SHELL_Success;

err_usage:
    print_fdb_read_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_fdb_delete_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fdb_delete))->pcHelpString);
}

static shell_status_t fdb_delete(shell_handle_t shell, int32_t argc, char **argv)
{
    bool permanent = false;
    unsigned long tmp;
    uint8_t address[6];
    uint16_t vid;
    int opt, rc;

    if (argc < 3)
        goto err_usage;

    if (str2mac(argv[1], address)) {
        shell_printf(shell, "invalid mac address format\n");
        goto err_usage;
    }

    h_strtoul(&tmp, argv[2], NULL, 0);
    vid = tmp;
    if (vid > 4095) {
        shell_printf(shell, "invalid vid value\n");
        goto err_usage;
    }

    optind = 3;
    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (permanent) {
        if (fdb_delete_permanent(address, vid) < 0) {
            shell_printf(shell, "fdb_delete_permanent(" MAC_STR_FMT ", %u) failed\n",
                MAC_STR(address), vid);
            goto err;
        }
    }

    rc = genavb_fdb_delete(address, vid);
    if (rc < 0) {
        shell_printf(shell, "genavb_fdb_delete(" MAC_STR_FMT ", %u) failed: %s\n",
            MAC_STR(address), vid, genavb_strerror(rc));
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_fdb_delete_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_fdb_dump_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fdb_dump))->pcHelpString);
}

static shell_status_t fdb_dump(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_fdb_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    genavb_fdb_status_t status = GENAVB_FDB_STATUS_INVALID;
    bool permanent = false;
    uint8_t address[6];
    uint32_t next = 0;
    uint16_t vid;
    bool dynamic;
    int opt;

    if (argc > 3)
        goto err_usage;

    optind = 1;
    while ((opt = getopt(argc, argv, "p")) != -1) {
        switch (opt) {
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    fdb_print_description(shell);

    if (permanent) {
        while (fdb_dump_permanent(&next, address, &vid, &dynamic, port_map, &status) == 0)
            fdb_print_entry(shell, address, vid, dynamic, port_map, status);
    } else {
        while (genavb_fdb_dump(&next, address, &vid, &dynamic, port_map, &status) == GENAVB_SUCCESS)
            fdb_print_entry(shell, address, vid, dynamic, port_map, status);
    }

    return kStatus_SHELL_Success;

err_usage:
    print_fdb_dump_usage(shell);
    return kStatus_SHELL_Error;
}

static int fdb_apply_permanent(shell_handle_t shell)
{
    struct genavb_fdb_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    genavb_fdb_status_t status = GENAVB_FDB_STATUS_INVALID;
    uint8_t address[6];
    uint32_t next = 0;
    uint16_t vid;
    bool dynamic;
    int i, rc;

    while (fdb_dump_permanent(&next, address, &vid, &dynamic, port_map, &status) == 0) {
        for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
            rc = genavb_fdb_update(address, vid, &port_map[i]);
            if (rc < 0) {
                shell_printf(shell, "genavb_fdb_update(" MAC_STR_FMT ", %u, %u) failed: %s\n",
                    MAC_STR(address), vid, port_map[i].port_id, genavb_strerror(rc));
            }
        }
    }

    return 0;
}

void fdb_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_read));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fdb_dump));

    fdb_apply_permanent(shell);
}
