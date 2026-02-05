/*
 * Copyright 2022-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <ctype.h>
#include <getopt.h>

#include "genavb/error.h"
#include "genavb/ether.h"
#include "genavb/helpers.h"
#include "genavb/vlan.h"

#include "storage.h"
#include "genavb.h"
#include "vlan.h"
#include "log.h"

#define VLAN_MAX_FILENAME 30

static shell_status_t vlan_update(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t vlan_delete(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t vlan_read(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t vlan_dump(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t vlan_set_pvid(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t vlan_get_pvid(shell_handle_t shell, int32_t argc, char **argv);

static void vlan_dump_pvid(shell_handle_t shell, bool permanent);

SHELL_COMMAND_DEFINE(vlan_update,
                     "\nvlan_update <vid> <port_id> [-c <control>] [-u] [-p]\n"
                     "    parameters:\n"
                     "        vid: vlan id\n"
                     "        port_id: logical port id\n"
                     "    options:\n"
                     "        -c <control>: registrar-admin-control, 0: forbidden, 1: fixed (default)\n"
                     "        -u: untagged transmit\n"
                     "        -p: update entry in permanent database\n",
                     &vlan_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_delete,
                     "\nvlan_delete <vid> [-p]\n"
                     "    parameters:\n"
                     "        vid: vlan id\n"
                     "    options:\n"
                     "        -p: delete entry in permanent database\n",
                     &vlan_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_read,
                     "\nvlan_read <vid> [-p]\n"
                     "    parameters:\n"
                     "        vid: vlan id\n"
                     "    options:\n"
                     "        -p: read entry from permanent database\n",
                     &vlan_read,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_dump,
                     "\nvlan_dump [-p]\n"
                     "    options:\n"
                     "        -p: print permanent database\n",
                     &vlan_dump,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_set_pvid,
                     "\nvlan_set_pvid <port_id> <vid> [-p]\n"
                     "    parameters:\n"
                     "        port_id: logical port id\n"
                     "        vid: port defaut vlan id (PVID)\n"
                     "    options:\n"
                     "        -p: update entry in permanent database\n",
                     &vlan_set_pvid,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(vlan_get_pvid,
                     "\nvlan_get_pvid <port_id> [-p]\n"
                     "    parameters:\n"
                     "        port_id: logical port id\n"
                     "    options:\n"
                     "        -p: read entry from permanent database\n",
                     &vlan_get_pvid,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_vlan(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(vlan_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_read))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_dump))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_set_pvid))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(vlan_get_pvid))->pcHelpString);
}

enum {
    COLUMN_TAGGED,
    COLUMN_UNTAGGED,
    COLUMN_FIXED,
    COLUMN_FORBIDDEN,
    COLUMN_MAX,
};

static void vlan_print_description(shell_handle_t shell)
{
    shell_printf(shell, "\n");
    shell_printf(shell, "  vid | dynamic |     tagged     |    untagged    |      fixed     |    forbidden   |\n");
    shell_printf(shell, "------+---------+----------------+----------------+----------------+----------------+\n");
    /*                       2 |   false |        2, 3, 4 |              5 |     2, 3, 4, 5 |              6 | */
}

static void vlan_print_port_list(shell_handle_t shell, uint32_t port_mask)
{
    unsigned int port_id;
    char buf[15];
    int i, count;

    count = 0;
    buf[0] = '\0';

    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        port_id = br_port_list[i];
        if (port_mask & (1 << port_id)) {
            if (!count)
                count += h_snprintf(buf + count, 15 - count, "%u", port_id);
            else
                count += h_snprintf(buf + count, 15 - count, ", %u", port_id);
        }
    }

    shell_printf(shell, " % 14s |", buf);
}

static void vlan_print_entry(shell_handle_t shell, uint16_t vid, bool dynamic, struct genavb_vlan_port_map port_map[CONFIG_APP_BR_NUM_PORTS])
{
    uint32_t port_mask[COLUMN_MAX] = {0};
    unsigned int port_id;
    int port, column;

    shell_printf(shell, " % 4u |", vid);
    shell_printf(shell, " % 7s |", dynamic ? " true": "false");

    /* create port mask for each column */
    for (port = 0; port < CONFIG_APP_BR_NUM_PORTS; port++) {
        port_id = port_map[port].port_id;

        if (port_map[port].untagged == false)
            port_mask[COLUMN_TAGGED] |= (1 << port_id);
        else
            port_mask[COLUMN_UNTAGGED] |= (1 << port_id);

        if (port_map[port].control == GENAVB_VLAN_ADMIN_CONTROL_FIXED)
            port_mask[COLUMN_FIXED] |= (1 << port_id);

        if (port_map[port].control == GENAVB_VLAN_ADMIN_CONTROL_FORBIDDEN)
            port_mask[COLUMN_FORBIDDEN] |= (1 << port_id);
    }

    /* print each column */
    for (column = 0; column < COLUMN_MAX; column++)
        vlan_print_port_list(shell, port_mask[column]);

    shell_printf(shell, "\n");
}

static void print_vlan_update_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(vlan_update))->pcHelpString);
}

static int vlan_port_mask_2_port_map(uint32_t port_mask, uint32_t untagged_port_mask, struct genavb_vlan_port_map map[CONFIG_APP_BR_NUM_PORTS])
{
    unsigned int port_id;
    int i;

    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        port_id = br_port_list[i];
        map[i].port_id = port_id;

        if (port_mask & (1 << port_id))
            map[i].control = GENAVB_VLAN_ADMIN_CONTROL_FIXED;
        else
            map[i].control = GENAVB_VLAN_ADMIN_CONTROL_FORBIDDEN;

        if (untagged_port_mask & (1 << port_id))
            map[i].untagged = true;
        else
            map[i].untagged = false;
    }

    return 0;
}

static int vlan_read_entry(const char *filename, uint32_t *port_mask, uint32_t *untagged_port_mask)
{
    char buf[MAX_FILE_SIZE + 1];
    unsigned int tmp[2];
    int rc, arg_val;

    rc = storage_read(filename, buf, MAX_FILE_SIZE);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    arg_val = sscanf(buf, "0x%08x,0x%08x", &tmp[0], &tmp[1]);
    if (arg_val < 1)
        return -1;

    /* backward compatibility, if no untagged port bitmask in 
     * permanent entry, all ports are tagged */
    if (arg_val < 2)
        tmp[1] = 0;

    if (port_mask)
        *port_mask = tmp[0];

    if (untagged_port_mask)
        *untagged_port_mask = tmp[1];

    return 0;
}

static int vlan_read_storage(uint16_t vid, uint32_t *port_mask, uint32_t *untagged_port_mask)
{
    char filename[VLAN_MAX_FILENAME];

    if (h_snprintf_strict(filename, VLAN_MAX_FILENAME, "/vlan/%u", vid) < 0)
        return -1;

    return vlan_read_entry(filename, port_mask, untagged_port_mask);
}

static int vlan_write_storage(uint16_t vid, uint32_t port_mask, uint32_t untagged_port_mask)
{
    char filename[VLAN_MAX_FILENAME];
    char str[22];

    /* vlan entries are stored in "/vlan/${vid}" files */
    if (h_snprintf_strict(filename, VLAN_MAX_FILENAME, "/vlan/%u", vid) < 0)
        return -1;

    /* file contains two port bitmasks for forwarded and untagged ports */
    if (h_snprintf_strict(str, sizeof(str), "0x%08x,0x%08x", port_mask, untagged_port_mask) < 0)
        return -1;

    return storage_write(filename, str, strlen(str) + 1);
}

static int vlan_get_storage_entry(unsigned int i, uint16_t *vid, uint32_t *port_mask, uint32_t *untagged_port_mask)
{
    char filename[VLAN_MAX_FILENAME];
    int rc = -1;

    if (!storage_get_file("/vlan", i, filename, VLAN_MAX_FILENAME)) {
        unsigned int tmp_vid;
        uint32_t tmp_port_mask, tmp_untagged_port_mask;

        if (sscanf(filename, "%u", &tmp_vid) != 1) {
            rc = -1;
            goto err;
        }

        *vid = tmp_vid;

        if (vlan_read_storage(*vid, &tmp_port_mask, &tmp_untagged_port_mask) < 0) {
            rc = -1;
            goto err;
        }

        *port_mask = tmp_port_mask;
        *untagged_port_mask = tmp_untagged_port_mask;

        rc = 0;
    }

err:
    return rc;
}

static int vlan_delete_storage(uint16_t vid)
{
    char filename[VLAN_MAX_FILENAME];

    if (h_snprintf_strict(filename, VLAN_MAX_FILENAME, "/vlan/%u", vid) < 0)
        return -1;

    return storage_rm(filename, false, true);
}

static int vlan_read_permanent(uint16_t vid, bool *dynamic, struct genavb_vlan_port_map map[CONFIG_APP_BR_NUM_PORTS])
{
    uint32_t port_mask = 0, untagged_port_mask = 0;

    if (vlan_read_storage(vid, &port_mask, &untagged_port_mask) < 0)
        return -1;

    if (vlan_port_mask_2_port_map(port_mask, untagged_port_mask, map) < 0)
        return -1;

    *dynamic = false;

    return 0;
}

static int vlan_dump_permanent(uint32_t *next, uint16_t *vid, bool *dynamic, struct genavb_vlan_port_map map[CONFIG_APP_BR_NUM_PORTS])
{
    uint32_t port_mask = 0, untagged_port_mask = 0;
    int rc;

    rc = vlan_get_storage_entry(*next, vid, &port_mask, &untagged_port_mask);
    if (rc != 0)
        goto out;

    if (vlan_port_mask_2_port_map(port_mask, untagged_port_mask, map) < 0)
        return -1;

    *dynamic = false;
    *next += 1;

out:
    return rc;
}

static int vlan_update_permanent(shell_handle_t shell, uint16_t vid, struct genavb_vlan_port_map *map)
{
    uint32_t port_mask = 0, untagged_port_mask = 0;

    if (storage_mkdir("/vlan", true) < 0) {
        return -1;
    }

    vlan_read_storage(vid, &port_mask, &untagged_port_mask);

    if (map->control == GENAVB_VLAN_ADMIN_CONTROL_FIXED) {
        port_mask |= (1 << map->port_id);
    } else if (map->control == GENAVB_VLAN_ADMIN_CONTROL_FORBIDDEN) {
        port_mask &= ~(1 << map->port_id);
    } else {
        shell_printf(shell, "Unknown control type\n");
        return -1;
    }

    if (map->untagged)
        untagged_port_mask |= (1 << map->port_id);
    else
        untagged_port_mask &= ~(1 << map->port_id);

    return vlan_write_storage(vid, port_mask, untagged_port_mask);
}

static int vlan_delete_permanent(uint16_t vid)
{
    if (vlan_read_storage(vid, NULL, NULL) < 0)
        return -1;

    return vlan_delete_storage(vid);
}

static shell_status_t vlan_update(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_vlan_port_map port_map = {0};
    bool permanent = false;
    unsigned long tmp;
    uint16_t vid;
    int opt, rc;

    if (argc < 3)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    vid = tmp;
    if (vid > VLAN_VID_MAX) {
        shell_printf(shell, "invalid vid value\n");
        goto err_usage;
    }

    h_strtoul(&tmp, argv[2], NULL, 0);
    port_map.port_id = tmp;
    if (port_map.port_id >= CONFIG_APP_LOGICAL_PORTS) {
        shell_printf(shell, "invalid port_id %u\n", port_map.port_id);
        goto err_usage;
    }

    port_map.control = GENAVB_VLAN_ADMIN_CONTROL_FIXED;

    optind = 3;
    while ((opt = getopt(argc, argv, "c:up")) != -1) {
        switch (opt) {
        case 'c':
            h_strtoul(&tmp, optarg, NULL, 0);
            port_map.control = (genavb_vlan_admin_control_t)tmp;
            break;
        case 'u':
            port_map.untagged = true;
            break;
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (port_map.control > GENAVB_VLAN_ADMIN_CONTROL_NORMAL) {
        shell_printf(shell, "invalid control value\n");
        goto err_usage;
    }

    if (permanent) {
        if (vlan_update_permanent(shell, vid, &port_map) < 0) {
            shell_printf(shell, "vlan_update_permanent(%u, %u) failed\n", vid, port_map.port_id);
            goto err;
        }
    }

    rc = genavb_vlan_update(vid, &port_map);
    if (rc < 0) {
        shell_printf(shell, "genavb_vlan_update(%u, %u) failed: %s\n", vid, port_map.port_id, genavb_strerror(rc));
        goto err;
    }

    shell_printf(shell, "VLAN update port(%u) vid(%u)\n", port_map.port_id, vid);

    return kStatus_SHELL_Success;

err_usage:
    print_vlan_update_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_vlan_read_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(vlan_read))->pcHelpString);
}

static shell_status_t vlan_read(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_vlan_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    bool permanent = false;
    unsigned long tmp;
    bool dynamic = false;
    uint16_t vid;
    int opt, rc;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    vid = tmp;
    if (vid > VLAN_VID_MAX) {
        shell_printf(shell, "invalid vid value\n");
        goto err_usage;
    }

    optind = 2;
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
        if (vlan_read_permanent(vid, &dynamic, port_map) < 0) {
            shell_printf(shell, "vlan_read_permanent(%u) failed\n", vid);
            goto err;
        }
    } else {
        rc = genavb_vlan_read(vid, &dynamic, port_map);
        if (rc < 0) {
            shell_printf(shell, "genavb_vlan_read(%u) failed: %s\n", vid, genavb_strerror(rc));
            goto err;
        }
    }

    vlan_print_description(shell);
    vlan_print_entry(shell, vid, dynamic, &port_map[0]);

    return kStatus_SHELL_Success;

err_usage:
    print_vlan_read_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_vlan_delete_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(vlan_delete))->pcHelpString);
}

static shell_status_t vlan_delete(shell_handle_t shell, int32_t argc, char **argv)
{
    bool permanent = false;
    unsigned long tmp;
    uint16_t vid;
    int opt, rc;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    vid = tmp;
    if (vid > VLAN_VID_MAX) {
        shell_printf(shell, "invalid vid value\n");
        goto err_usage;
    }

    optind = 2;
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
        if (vlan_delete_permanent(vid) < 0) {
            shell_printf(shell, "vlan_delete_permanent(%u) failed\n", vid);
            goto err;
        }
    }

    rc = genavb_vlan_delete(vid);
    if (rc < 0) {
        shell_printf(shell, "genavb_vlan_delete(%u) failed: %s\n", vid, genavb_strerror(rc));
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_vlan_delete_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_vlan_dump_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(vlan_dump))->pcHelpString);
}

static shell_status_t vlan_dump(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_vlan_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    bool permanent = false;
    bool dynamic = false;
    uint32_t next = 0;
    uint16_t vid;
    int opt;

    if (argc > 2)
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

    vlan_print_description(shell);

    if (permanent) {
        while (vlan_dump_permanent(&next, &vid, &dynamic, port_map) == 0)
            vlan_print_entry(shell, vid, dynamic, port_map);
    } else {
        while (genavb_vlan_dump(&next, &vid, &dynamic, port_map) == GENAVB_SUCCESS)
            vlan_print_entry(shell, vid, dynamic, port_map);
    }

    vlan_dump_pvid(shell, permanent);

    return kStatus_SHELL_Success;

err_usage:
    print_vlan_dump_usage(shell);
    return kStatus_SHELL_Error;
}

static int vlan_update_permanent_pvid(unsigned int port_id, uint16_t vid)
{
    char filename[VLAN_MAX_FILENAME];

    if (h_snprintf_strict(filename, VLAN_MAX_FILENAME, "/port%u", port_id) < 0)
        goto err;

    if (storage_mkdir(filename, true) < 0)
        goto err;

    if (h_snprintf_strict(filename, VLAN_MAX_FILENAME, "/port%u/pvid", port_id) < 0)
        goto err;

    return storage_write_uint(filename, vid);

err:
    return -1;
}

static int vlan_read_permanent_pvid(unsigned int port_id, uint16_t *vid)
{
    char filename[VLAN_MAX_FILENAME];

    if (h_snprintf_strict(filename, VLAN_MAX_FILENAME, "/port%u/pvid", port_id) < 0)
        goto err;

    return storage_read_u16(filename, vid);

err:
    return -1;
}

static void vlan_dump_pvid(shell_handle_t shell, bool permanent)
{
    uint16_t vid;
    int port_id;
    int i;

    shell_printf(shell, "\n");

    /**
     *  port |    2 |    3 |    4 |    5 |    6
     * ------+------+------+------+------+------
     *  PVID |   11 |    4 |    1 |    1 |    1
     */

    /* port list row */
    shell_printf(shell, " port ");
    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        port_id = br_port_list[i];
        shell_printf(shell, "| % 4u ", port_id);
    }
    shell_printf(shell, "\n");

    /* separator */
    shell_printf(shell, "------");
    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        shell_printf(shell, "+------");
    }
    shell_printf(shell, "\n");

    /* PVID row */
    shell_printf(shell, " PVID ");
    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        port_id = br_port_list[i];

        if (permanent) {
            if (vlan_read_permanent_pvid(port_id, &vid) < 0) {
                shell_printf(shell, "|      ");
                continue;
            }
        } else {
            if (genavb_vlan_get_port_default(port_id, &vid) < 0) {
                shell_printf(shell, "|      ");
                continue;
            }
        }

        shell_printf(shell, "| % 4u ", vid);
    }
    shell_printf(shell, "\n");

    return;
}

static void print_vlan_set_pvid_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(vlan_set_pvid))->pcHelpString);
}

static shell_status_t vlan_set_pvid(shell_handle_t shell, int32_t argc, char **argv)
{
    bool permanent = false;
    unsigned int port_id;
    unsigned long tmp;
    uint16_t vid;
    int opt, rc;

    if (argc < 3)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    port_id = tmp;
    if (port_id >= CONFIG_APP_LOGICAL_PORTS) {
        shell_printf(shell, "invalid port_id %u\n", port_id);
        goto err_usage;
    }

    h_strtoul(&tmp, argv[2], NULL, 0);
    vid = tmp;
    if (vid > VLAN_VID_MAX) {
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
        if (vlan_update_permanent_pvid(port_id, vid) < 0) {
            shell_printf(shell, "vlan_update_permanent_pvid() failed\n");
            goto err;
        }
    }

    rc = genavb_vlan_set_port_default(port_id, vid);
    if (rc < 0) {
        shell_printf(shell, "genavb_vlan_set_port_default() failed: %s\n", genavb_strerror(rc));
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_vlan_set_pvid_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_vlan_get_pvid_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(vlan_get_pvid))->pcHelpString);
}

static shell_status_t vlan_get_pvid(shell_handle_t shell, int32_t argc, char **argv)
{
    bool permanent = false;
    unsigned int port_id;
    unsigned long tmp;
    uint16_t vid;
    int opt, rc;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    port_id = tmp;
    if (port_id >= CONFIG_APP_LOGICAL_PORTS) {
        shell_printf(shell, "invalid port_id %u\n", port_id);
        goto err_usage;
    }

    optind = 2;
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
        if (vlan_read_permanent_pvid(port_id, &vid) < 0) {
            shell_printf(shell, "vlan_read_permanent_pvid() failed\n");
            goto err;
        }
    } else {
        rc = genavb_vlan_get_port_default(port_id, &vid);
        if (rc < 0) {
            shell_printf(shell, "genavb_vlan_get_port_default() failed: %s\n", genavb_strerror(rc));
            goto err;
        }
    }

    shell_printf(shell, "port %u PVID %u\n", port_id, vid);

    return kStatus_SHELL_Success;

err_usage:
    print_vlan_get_pvid_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static int vlan_apply_permanent_pvid(void)
{
    struct genavb_vlan_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    bool dynamic = false;
    uint16_t vid;
    int port_id;
    int i, rc;

    for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
        port_id = br_port_list[i];

        /**
         * if not configured by the user, set port PVID to VLAN_PVID_DEFAULT
         * and add port to VLAN_PVID_DEFAULT vlan membership
         */
        if (vlan_read_permanent_pvid(port_id, &vid) < 0) {
            vid = VLAN_PVID_DEFAULT;

            if (vlan_read_permanent(vid, &dynamic, port_map) < 0) {
                port_map[i].port_id = port_id;
                port_map[i].control = GENAVB_VLAN_ADMIN_CONTROL_FIXED;

                rc = genavb_vlan_update(vid, &port_map[i]);
                if (rc < 0) {
                    log_err("genavb_vlan_update() failed: %s\n", genavb_strerror(rc));
                    goto err;
                }
            }
        }

        rc = genavb_vlan_set_port_default(port_id, vid);
        if (rc < 0) {
            log_err("genavb_vlan_set_port_default() failed: %s\n", genavb_strerror(rc));
            goto err;
        }
    }

    return 0;

err:
    return -1;
}

static int vlan_apply_permanent_entries(void)
{
    struct genavb_vlan_port_map port_map[CONFIG_APP_BR_NUM_PORTS] = {0};
    bool dynamic = false;
    uint32_t next = 0;
    uint16_t vid;
    int i, rc;

    while (vlan_dump_permanent(&next, &vid, &dynamic, port_map) == 0) {
        for (i = 0; i < CONFIG_APP_BR_NUM_PORTS; i++) {
            rc = genavb_vlan_update(vid, &port_map[i]);
            if (rc < 0) {
                log_err("genavb_vlan_update() failed: %s\n", genavb_strerror(rc));
            }
        }
    }

    return 0;
}

static int vlan_apply_permanent(void)
{
    vlan_apply_permanent_pvid();

    vlan_apply_permanent_entries();

    return 0;
}

void vlan_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_read));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_dump));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_set_pvid));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(vlan_get_pvid));

    vlan_apply_permanent();
}
