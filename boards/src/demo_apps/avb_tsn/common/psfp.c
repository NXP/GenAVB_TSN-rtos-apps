/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <ctype.h>
#include <getopt.h>

#include "genavb/helpers.h"
#include "genavb/qos.h"
#include "genavb/psfp.h"
#include "genavb/error.h"

#include "rtos_apps/types.h"

#include "storage.h"
#include "genavb.h"
#include "psfp.h"

#define STREAM_FILTER_DEFAULT_STREAM_HANDLE (0)
#define STREAM_FILTER_NULL_FLOW_METER_REF (0xFFFFFFFF)
#define STREAM_FILTER_NULL_STREAM_GATE_REF (0xFFFFFFFF)
#define STREAM_FILTER_DEFAULT_MAX_SDU_SIZE (0)
#define STREAM_FILTER_DEFAULT_FLOW_METER_ENABLE (false)

#define STREAM_GATE_DEFAULT_GATE_ENABLE     (false)
#define STREAM_GATE_DEFAULT_ADMIN_STATE     (1)
#define STREAM_GATE_DEFAULT_ADMIN_IPV       (GENAVB_IPV_SPEC_NULL)
#define STREAM_GATE_DEFAULT_CYCLE_TIME      (100000ULL)
#define STREAM_GATE_DEFAULT_CYCLE_TIME_EXT  (0)
#define STREAM_GATE_DEFAULT_BASE_TIME       (0)
#define STREAM_GATE_DEFAULT_INVALID_RX      (0)
#define STREAM_GATE_DEFAULT_OCTETS_EXCEEDED (0)

#define FLOW_METER_DEFAULT_CIR (10000000ULL)
#define FLOW_METER_DEFAULT_CBS (2000)
#define FLOW_METER_DEFAULT_EIR (0)
#define FLOW_METER_DEFAULT_EBS (0)
#define FLOW_METER_DEFAULT_CF  (0)
#define FLOW_METER_DEFAULT_CM  (0)
#define FLOW_METER_DEFAULT_DROPY (0)
#define FLOW_METER_DEFAULT_MREN (0)

static shell_status_t sf_update(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t sf_delete(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t sf_read(shell_handle_t shell, int32_t argc, char **argv);

static shell_status_t sg_update(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t sg_delete(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t sg_read(shell_handle_t shell, int32_t argc, char **argv);

static shell_status_t fm_update(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t fm_delete(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t fm_read(shell_handle_t shell, int32_t argc, char **argv);

SHELL_COMMAND_DEFINE(sf_update,
                     "\nsf_update <index> [-h <stream_handle>] [-P <priority_spec>] [-m <flow_meter_ref>] [-M <flow_meter_enable>] [-g <stream_gate_ref>] [-S <max_sdu_size>] [-p]\n"
                     "        parameters:\n"
                     "                <index>: 32bits, user defined stream filter identifier\n"

                     "        options:\n"
                     "                -h <stream_handle>: stream handle\n"
                     "                -P <priority>: vlan tag priority code point  value (wildcarded if not specified)\n"
                     "                -m <flow_meter_ref>: flow meter instance identifier\n"
                     "                -M <flow_meter_enable>: flow meter enable, 0: disable, 1: enable, default: 0\n"
                     "                -g <stream_gate_ref>: stream gate instance identifier\n"
                     "                -S <max_sdu_size>: maximum service data unit in bytes\n"
                     "                -p: update entry in permanent database\n",
                     &sf_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sf_delete,
                     "\nsf_delete <index> [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined stream filter identifier\n"
                     "        options:\n"
                     "                -p: delete entry from permanent database\n",
                     &sf_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sf_read,
                     "\nsf_read <index> [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined stream filter identifier\n"
                     "        options:\n"
                     "                -p: read entry from permanent database\n",
                     &sf_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(sg_update,
                     "\nsg_update <index> [-e <enable>] [-s <admin_state>] [-P <admin_ipv>] [-c <cycle_time>] [-C <cycle_time_ext>] [-b <base_time>] [-l <state,ipv,interval,octet>] [-l ...] [-I <enable>] [-X <enable>] [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined stream gate identifier\n"
                     "        options:\n"
                     "                -e <enable>: gate enable, 0: disabled (default), 1: enabled\n"
                     "                -s <admin_state>: administrative gate state, 0: closed, 1: open (default)\n"
                     "                -P <admin_ipv>: administrative IPV, 0 to 7, 0xff:null (default)\n"
                     "                -c <cycle_time>: gate cycle time in ns, default: 100000\n"
                     "                -C <cycle_time_ext>: gate cycle time extension in ns, 0 to 4294967295, default: 0\n"
                     "                -b <base_time>: gate base time in ns, 0 to (2^64 - 1), default: 0\n"
                     "                -l <state,ipv,interval,octet>: gate control list, default: 0,0,100000,0 (one option per list entry)\n"
                     "                -I <enable>: gate closed due to invalid rx enable 0: disabled (default), 1: enabled\n"
                     "                -X <enable>: gate closed due to octets exceeded enable, 0: disabled (default), 1: enabled\n"
                     "                -p: update entry in permanent database\n"
                     "\nsg_update <index> [-i] [-x]\n"
                     "        parameters:\n"
                     "                <index>: user defined stream gate identifier\n"
                     "        options:\n"
                     "                -i: reset gate closed due to invalid rx\n"
                     "                -x: reset gate closed due to octets exceeded\n",
                     &sg_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sg_delete,
                     "\nsg_delete <index> [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined stream gate identifier\n"
                     "        options:\n"
                     "                -p: delete entry from permanent database\n",
                     &sg_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(sg_read,
                     "\nsg_read <index> [-p] [-t <type>]\n"
                     "        parameters:\n"
                     "                <index>: user defined stream gate identifier\n"
                     "        options:\n"
                     "                -p: read entry from permanent database\n"
                     "                -t <type>: configuration type, 0: operational, 1: administrative, default: 0\n",
                     &sg_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(fm_update,
                     "\nfm_update <index> -r <cir> -b <cbs> -R <eir> -B <ebs> -f <cflag> -c <cmode> -y <dropy> -m <markre> [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined flow meter identifier\n"
                     "        options:\n"
                     "                -r <cir>: committed rate in unit of bits per sec, default: 10000000\n"
                     "                -b <cbs>: committed burst size in bytes, default: 2000\n"
                     "                -R <eir>: excess rate in unit of bits per sec, default: 0\n"
                     "                -B <ebs>: excess burst size in bytes, default: 0\n"
                     "                -f <cflag>: coupling flag, 0: CIR and EIR buckets not coupled, 1: CIR and EIR buckets coupled, default:0\n"
                     "                -c <cmode>: color mode, 0: color-blind, 1: color-aware, default: 0\n"
                     "                -y <dropy>: drop on yellow, 0: yellow frames eligible to drop, 1: yellow frames are dropped, default: 0\n"
                     "                -m <markre>: mark all frames red enable, default: 0\n"
                     "                -p: update entry in permanent database\n"
                     "\nfm_update <index> -M \n"
                     "        parameters:\n"
                     "                <index>: user defined flow meter identifier\n"
                     "        options:\n"
                     "                -M: reset mark all frames red\n",
                     &fm_update,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fm_delete,
                     "\nfm_delete <index> [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined flow meter identifier\n"
                     "        options:\n"
                     "                -p: delete entry from permanent database\n",
                     &fm_delete,
                     SHELL_IGNORE_PARAMETER_COUNT);
SHELL_COMMAND_DEFINE(fm_read,
                     "\nfm_read <index> [-p]\n"
                     "        parameters:\n"
                     "                <index>: user defined flow meter identifier\n"
                     "        options:\n"
                     "                -p: read entry from permanent database\n",
                     &fm_read,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_sg_storage(shell_handle_t shell)
{
    shell_printf(shell, "\nstream gate storage configuration\n\n");
    shell_printf(shell, "path: /sg\n");
    shell_printf(shell, "    description: directory used by sg_update/sg_read commands\n");
    shell_printf(shell, "path: /sg/N (N: stream gate instance index, 0 to %u)\n", genavb_stream_gate_get_max_entries() - 1 );
    shell_printf(shell, "parameters (instance index N):\n");
    shell_printf(shell, "    base_time:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: 64bits absolute gPTP time. If in the past, actual base time' = base time + N * cycle_time\n");
    shell_printf(shell, "    admin_state:\n");
    shell_printf(shell, "        value: 0 to 1\n");
    shell_printf(shell, "        description: administrative gate state 0: closed, 1: opened\n");
    shell_printf(shell, "    admin_ipv:\n");
    shell_printf(shell, "        value: 0 to 7 or 0xff\n");
    shell_printf(shell, "        description: administrative internal priority value in decimal\n");
    shell_printf(shell, "    cycle_time:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: 32bits cycle time in nanoseconds\n");
    shell_printf(shell, "    cycle_time_ext:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: 32bits cycle time extension in nanoseconds\n");
    shell_printf(shell, "    gate_closed_due_to_invalid_rx_enable:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: gate closed due to invalid rx enable\n");
    shell_printf(shell, "    gate_closed_due_to_octets_exceeded_enable:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: gate closed due to octets exceeded enable\n");
    shell_printf(shell, "    list_length:\n");
    shell_printf(shell, "        value: 0 to %u\n", genavb_stream_gate_control_get_max_entries());
    shell_printf(shell, "        description: gate control list length\n");
    shell_printf(shell, "    entryM: (M: entry id, 0 to %u)\n", genavb_stream_gate_control_get_max_entries() - 1);
    shell_printf(shell, "        format: state,ipv,interval,octet (e.g 1,7,20000,1000000)\n");
    shell_printf(shell, "        value: 0 to 1, 0 to 7 or 0xff, 0 to 4294967295, 0 to 4294967295\n");
    shell_printf(shell, "        description: gate list entry\n");
    shell_printf(shell, "            state: gate state 0: closed, 1: opened\n");
    shell_printf(shell, "            ipv: internal priority value in decimal\n");
    shell_printf(shell, "            interval: 32bits gate interval in nanoseconds\n");
    shell_printf(shell, "            octet: 32bits max octet interval in bytes\n");
}

void help_config_fm_storage(shell_handle_t shell)
{
    shell_printf(shell, "\nflow meter storage configuration\n\n");
    shell_printf(shell, "path: /fm\n");
    shell_printf(shell, "    description: directory used by fm_update/fm_read commands\n");
    shell_printf(shell, "path: /fm/N (N: flow meter instance index, 0 to %u)\n", genavb_flow_meter_get_max_entries() - 1 );
    shell_printf(shell, "parameters (instance index N):\n");
    shell_printf(shell, "    committed_information_rate:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: committed rate in unit of bits per sec\n");
    shell_printf(shell, "    committed_burst_size:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: committed burst size in bytes\n");
    shell_printf(shell, "    excess_information_rate:\n");
    shell_printf(shell, "        value: 0 to (2^64 - 1)\n");
    shell_printf(shell, "        description: excess rate in unit of bits per sec\n");
    shell_printf(shell, "    excess_burst_size:\n");
    shell_printf(shell, "        value: 0 to 4294967295\n");
    shell_printf(shell, "        description: excess burst size in bytes\n");
    shell_printf(shell, "    coupling_flag:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: coupling flag\n");
    shell_printf(shell, "    color_mode:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: color mode\n");
    shell_printf(shell, "    drop_on_yellow:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: drop on yellow\n");
    shell_printf(shell, "    mark_all_frames_red_enable:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: mark all frames red enable\n");
}

void help_config_psfp(shell_handle_t shell)
{
    shell_printf(shell, (SHELL_COMMAND(sf_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sf_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sf_read))->pcHelpString);

    shell_printf(shell, (SHELL_COMMAND(sg_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sg_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(sg_read))->pcHelpString);

    help_config_sg_storage(shell);

    shell_printf(shell, (SHELL_COMMAND(fm_update))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fm_delete))->pcHelpString);
    shell_printf(shell, (SHELL_COMMAND(fm_read))->pcHelpString);

    help_config_fm_storage(shell);
}

static const struct genavb_stream_filter_instance stream_filter_default = {
    .stream_handle = STREAM_FILTER_DEFAULT_STREAM_HANDLE,
    .priority_spec = GENAVB_PRIORITY_SPEC_WILDCARD,
    .flow_meter_enable = STREAM_FILTER_DEFAULT_FLOW_METER_ENABLE,
    .flow_meter_ref = STREAM_FILTER_NULL_FLOW_METER_REF,
    .stream_gate_ref = STREAM_FILTER_NULL_STREAM_GATE_REF,
    .max_sdu_size = STREAM_FILTER_DEFAULT_MAX_SDU_SIZE,
    .stream_blocked_due_to_oversize_frame_enabled = false,
    .stream_blocked_due_to_oversize_frame = false,
};

static const struct genavb_flow_meter_instance flow_meter_default = {
    .committed_information_rate = FLOW_METER_DEFAULT_CIR,
    .committed_burst_size = FLOW_METER_DEFAULT_CBS,
    .excess_information_rate = FLOW_METER_DEFAULT_EIR,
    .excess_burst_size = FLOW_METER_DEFAULT_EBS,
    .coupling_flag = FLOW_METER_DEFAULT_CF,
    .color_mode = FLOW_METER_DEFAULT_CM,
    .drop_on_yellow = FLOW_METER_DEFAULT_DROPY,
    .mark_all_frames_red_enable = FLOW_METER_DEFAULT_MREN,
};

static void sf_print_entry_counters(shell_handle_t shell, struct genavb_stream_filter_instance *instance)
{
    shell_printf(shell, "matching frames count     %"PRIu64"\n", instance->matching_frames_count);
    shell_printf(shell, "passing frames count      %"PRIu64"\n", instance->passing_frames_count);
    shell_printf(shell, "not passing frames count  %"PRIu64"\n", instance->not_passing_frames_count);
    shell_printf(shell, "red frames count          %"PRIu64"\n", instance->red_frames_count);
    shell_printf(shell, "passing sdu count         %"PRIu64"\n", instance->passing_sdu_count);
    shell_printf(shell, "not passing sdu count     %"PRIu64"\n", instance->not_passing_sdu_count);
}

static void sf_print_entry(shell_handle_t shell, uint32_t index, struct genavb_stream_filter_instance *instance)
{
    shell_printf(shell, "\n");
    shell_printf(shell, "Stream Filter %"PRIu32"\n", index);
    shell_printf(shell, "-----------------------\n");

    if (instance->stream_handle != GENAVB_STREAM_HANDLE_WILDCARD)
        shell_printf(shell, "stream handle             %"PRIu32"\n", instance->stream_handle);
    else
        shell_printf(shell, "stream handle\n");

    if (instance->priority_spec != GENAVB_PRIORITY_SPEC_WILDCARD)
        shell_printf(shell, "priority spec             %u\n", instance->priority_spec);
    else
        shell_printf(shell, "priority spec\n");

    if (instance->flow_meter_ref != STREAM_FILTER_NULL_FLOW_METER_REF)
        shell_printf(shell, "flow meter ref            %"PRIu32"\n", instance->flow_meter_ref);
    else
        shell_printf(shell, "flow meter ref\n");

    shell_printf(shell, "flow meter enable         %u\n", instance->flow_meter_enable);

    if (instance->stream_gate_ref != STREAM_FILTER_NULL_STREAM_GATE_REF)
        shell_printf(shell, "stream gate ref           %"PRIu32"\n", instance->stream_gate_ref);
    else
        shell_printf(shell, "stream gate ref\n");

    shell_printf(shell, "max sdu size              %"PRIu32"\n", instance->max_sdu_size);
}

static int sf_update_permanent(uint32_t index, struct genavb_stream_filter_instance *instance, bool endpoint)
{
    char path[30] = {0};
    int rc = 0;

    if (h_snprintf_strict(path, 30, "/sf/%lu", index) < 0) {
        rc = -1;
        goto err;
    }

    if (storage_mkdir(path, true) < 0) {
        rc = -1;
        goto err;
    }

    storage_cd(path, true);

    storage_write_uint("stream_handle", instance->stream_handle);
    storage_write_uint("priority_spec", instance->priority_spec);
    storage_write_uint("flow_meter_ref", instance->flow_meter_ref);
    storage_write_uint("flow_meter_enable", instance->flow_meter_enable);
    storage_write_uint("stream_gate_ref", instance->stream_gate_ref);
    storage_write_uint("max_sdu_size", instance->max_sdu_size);

    storage_cd("-", true);

err:
    return rc;
}

static int sf_delete_permanent(uint32_t index, bool endpoint)
{
    char filename[30];

    if (h_snprintf_strict(filename, 30, "/sf/%lu", index) < 0)
        return -1;

    return storage_rm(filename, true, true);
}

static int sf_read_permanent(uint32_t index, struct genavb_stream_filter_instance *instance, bool endpoint)
{
    char path[30] = {0};
    int rc = -1;

    if (h_snprintf_strict(path, 30, "/sf/%lu", index) < 0)
        goto err;

    if (storage_cd(path, true) == 0) {
        storage_read_u32("stream_handle", &instance->stream_handle);
        storage_read_u8("priority_spec", &instance->priority_spec);
        storage_read_u32("flow_meter_ref", &instance->flow_meter_ref);
        storage_read_bool("flow_meter_enable", &instance->flow_meter_enable);
        storage_read_u32("stream_gate_ref", &instance->stream_gate_ref);
        storage_read_u32("max_sdu_size", &instance->max_sdu_size);

        storage_cd("-", true);
        rc = 0;
    }

    return rc;
err:
    return rc;
}

static void sf_apply_permanent(shell_handle_t shell)
{
    struct genavb_stream_filter_instance instance;
    unsigned int i, index;
    char subdirname[MAX_DIR_NAME_LEN];

    i = 0;
    while (!storage_get_dir("/sf", i, subdirname, MAX_DIR_NAME_LEN)) {
        i++;

        if (sscanf(subdirname, "%u", &index) != 1)
            continue;

        instance = stream_filter_default;

        if (sf_read_permanent(index, &instance, false) == 0) {
            if (genavb_stream_filter_update(index, &instance) < 0)
                shell_printf(shell, "genavb_stream_filter_update(%u) failed\n", index);
        }
    }
}

static void print_sf_update_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(sf_update))->pcHelpString);
}

static shell_status_t sf_update(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_stream_filter_instance instance = stream_filter_default;
    bool permanent = false;
    unsigned long tmp;
    uint32_t index;
    int opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_stream_filter_get_max_entries()) {
        shell_printf(shell, "invalid stream filter index\n");
        goto err_usage;
    }

    sf_read_permanent(index, &instance, 0);

    optind = 2;
    while ((opt = getopt(argc, argv, "h:P:m:M:g:S:p")) != -1) {
        switch (opt) {
        case 'h':
            h_strtoul(&tmp, optarg, NULL, 0);
            instance.stream_handle = tmp;
            break;
        case 'P':
            h_strtoul(&tmp, optarg, NULL, 0);
            instance.priority_spec = tmp;
            if ((instance.priority_spec >= QOS_PRIORITY_MAX) && (instance.priority_spec != GENAVB_PRIORITY_SPEC_WILDCARD))
                goto err_usage;
            break;
        case 'm':
            h_strtoul(&tmp, optarg, NULL, 0);
            instance.flow_meter_ref = tmp;
            if (instance.flow_meter_ref >= genavb_flow_meter_get_max_entries())
                goto err_usage;
            break;
        case 'M':
            h_strtoul(&tmp, optarg, NULL, 0);
            instance.flow_meter_enable = tmp;
            break;
        case 'g':
            h_strtoul(&tmp, optarg, NULL, 0);
            instance.stream_gate_ref = tmp;
            if (instance.stream_gate_ref >= genavb_stream_gate_get_max_entries())
                goto err_usage;
            break;
        case 'S':
            h_strtoul(&tmp, optarg, NULL, 0);
            instance.max_sdu_size = tmp;
            break;
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (permanent) {
        if (sf_update_permanent(index, &instance, 0) < 0) {
            shell_printf(shell, "sf_update_permanent(%u) failed\n", index);
            goto err;
        }
    }

    if (genavb_stream_filter_update(index, &instance) < 0) {
        shell_printf(shell, "genavb_stream_filter_update(%u) failed\n", index);
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_sf_update_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_sf_delete_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(sf_delete))->pcHelpString);
}

static shell_status_t sf_delete(shell_handle_t shell, int32_t argc, char **argv)
{
    uint32_t index;
    bool permanent = false;
    unsigned long tmp;
    int opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_stream_filter_get_max_entries()) {
        shell_printf(shell, "invalid stream filter index\n");
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
        if (sf_delete_permanent(index, 0) < 0) {
            shell_printf(shell, "sf_delete_permanent(%u) failed: Entry does not exist\n", index);
            goto err;
        }
    }

    if (genavb_stream_filter_delete(index) < 0) {
        shell_printf(shell, "genavb_stream_filter_delete(%u) failed\n", index);
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_sf_delete_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static void print_sf_read_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(sf_read))->pcHelpString);
}

static shell_status_t sf_read(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_stream_filter_instance instance = stream_filter_default;
    bool permanent = false;
    unsigned long tmp;
    uint32_t index;
    int rc, opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_stream_filter_get_max_entries()) {
        shell_printf(shell, "invalid stream filter index\n");
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
        if (sf_read_permanent(index, &instance, 0) < 0) {
            shell_printf(shell, "sf_read_permanent(%u) failed: Entry does not exist\n", index);
            goto no_entry;
        }

        sf_print_entry(shell, index, &instance);
    } else {
        rc = genavb_stream_filter_read(index, &instance);
        if (!rc) {
            shell_printf(shell, "genavb_stream_filter_read(%u) failed: Entry does not exist\n", index);
            goto no_entry;
        } else if (rc < 0) {
            shell_printf(shell, "genavb_stream_filter_read(%u) failed\n", index);
            goto err;
        }

        sf_print_entry(shell, index, &instance);
        sf_print_entry_counters(shell, &instance);
    }

no_entry:
    return kStatus_SHELL_Success;

err_usage:
    print_sf_read_usage(shell);
err:
    return kStatus_SHELL_Error;
}

static int sg_read_entry_from_storage(const char *filename, uint8_t *state, uint8_t *ipv, uint32_t *interval, uint32_t *octet_max)
{
    char buf[32 + 1];
    unsigned int tmp[4];
    int rc;

    rc = storage_read(filename, buf, 32);
    if (rc < 0)
        return -1;

    buf[rc] = '\0';

    if (sscanf(buf, "%u,%u,%u,%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) != 4)
        return -1;

    *state = tmp[0];
    *ipv = tmp[1];
    *interval = tmp[2];
    *octet_max = tmp[3];

    return 0;
}

static int sg_write_entry_to_storage(const char *filename, uint8_t mask, uint8_t ipv, uint32_t interval, uint32_t octet)
{
    char str[28];

    if (h_snprintf_strict(str, 28, "%u,%u,%"PRIu32",%"PRIu32, mask, ipv, interval, octet) < 0)
        return -1;

    return storage_write(filename, str, strlen(str) + 1);
}

static void sg_print_entry(shell_handle_t shell, uint32_t index, struct genavb_stream_gate_instance *instance)
{
    struct genavb_stream_gate_control_entry *entry;
    int i;

    shell_printf(shell, "\n");
    shell_printf(shell, "Stream Gate\n");

    shell_printf(shell, " index  |      enable       | admin state | admin ipv |       base time        |       cycle time       | cycle_time ext  | list length |\n");
    shell_printf(shell, "--------+-------------------+-------------+-----------+------------------------+------------------------+-----------------|-------------+\n");
    shell_printf(shell, " % 6" PRIu32 " |", index);
    shell_printf(shell, " % 17u |", instance->gate_enable);
    shell_printf(shell, " % 11u |", instance->admin_gate_state);
    if (instance->admin_ipv != GENAVB_IPV_SPEC_NULL)
        shell_printf(shell, " % 9u |", instance->admin_ipv);
    else
        shell_printf(shell, "    0xff   |");
    shell_printf(shell, " % 22" PRIu64 " |", instance->base_time);
    shell_printf(shell, " % 22" PRIu32 " |", (NSECS_PER_SEC * (uint64_t)instance->cycle_time_p) / instance->cycle_time_q);
    shell_printf(shell, " % 15" PRIu32 " |", instance->cycle_time_extension);
    shell_printf(shell, " % 11" PRIu32 " |", instance->list_length);
    shell_printf(shell, "\n");

    shell_printf(shell, "        +-------------------+-------------+-----------+------------------------+------------------------+-----------------+-------------+\n");
    shell_printf(shell, "        | invalid rx enable |        invalid rx       | octets exceeded enable |      octets exceeded   |\n");
    shell_printf(shell, "        +-------------------+-------------------------+------------------------+------------------------+\n");
    shell_printf(shell, "        | % 17u |", instance->gate_closed_due_to_invalid_rx_enable);
    shell_printf(shell, " % 23u |", instance->gate_closed_due_to_invalid_rx);
    shell_printf(shell, " % 22u |", instance->gate_closed_due_to_octets_exceeded_enable);
    shell_printf(shell, " % 22u |", instance->gate_closed_due_to_octets_exceeded);
    shell_printf(shell, "\n");

    if (instance->list_length > genavb_stream_gate_control_get_max_entries()) {
        shell_printf(shell, "invalid list length\n");
        return;
    }

    if (instance->list_length && instance->control_list) {
        shell_printf(shell, "\n");
        shell_printf(shell, "Stream Gate Control List\n");

        shell_printf(shell, " entry | oper | state |  ipv  | interval (ns) | interval octet max |\n");
        shell_printf(shell, "-------+------+-------+-------+---------------+--------------------+\n");
        for (i = 0; i < instance->list_length; i++) {
            entry = &instance->control_list[i];
            shell_printf(shell, " % 5u |", i);
            shell_printf(shell, " % 4u |", entry->operation_name);
            shell_printf(shell, " % 5u |", entry->gate_state_value);
            if (entry->ipv_spec != GENAVB_IPV_SPEC_NULL)
                shell_printf(shell, " % 5u |", entry->ipv_spec);
            else
                shell_printf(shell, "  0xff |");
            shell_printf(shell, " % 13" PRIu32 " |", entry->time_interval_value);
            shell_printf(shell, " % 18" PRIu32 " |", entry->interval_octet_max);
            shell_printf(shell, "\n");
        }
    }
    shell_printf(shell, "\n");
}

static int sg_update_permanent(shell_handle_t shell, uint32_t index, struct genavb_stream_gate_instance *instance, bool endpoint)
{
    struct genavb_stream_gate_control_entry *entry;
    char path[30] = {0};
    int rc = 0;
    int i;

    if (h_snprintf_strict(path, 30, "/sg/%"PRIu32, index) < 0) {
        rc = -1;
        goto err;
    }

    if (storage_mkdir(path, true) < 0) {
        rc = -1;
        goto err;
    }

    storage_cd(path, true);

    storage_write_uint("gate_enable", instance->gate_enable);
    storage_write_uint("admin_state", instance->admin_gate_state);
    storage_write_uint("admin_ipv", instance->admin_ipv);
    storage_write_uint("cycle_time", instance->cycle_time_p);
    storage_write_uint("cycle_time_extension", instance->cycle_time_extension);
    storage_write_u64("base_time", instance->base_time);
    storage_write_uint("list_length", instance->list_length);
    storage_write_uint("gate_closed_due_to_invalid_rx_enable", instance->gate_closed_due_to_invalid_rx_enable);
    storage_write_uint("gate_closed_due_to_octets_exceeded_enable", instance->gate_closed_due_to_octets_exceeded_enable);

    if ((instance->list_length) && (instance->control_list)) {
        for (i = 0; i < instance->list_length; i++) {
            entry = &instance->control_list[i];
            h_snprintf(path, 30, "entry%u", i);
            sg_write_entry_to_storage(path, entry->gate_state_value, entry->ipv_spec, entry->time_interval_value, entry->interval_octet_max);
        }
    }

    storage_cd("-", true);
err:
    return rc;
}

static int sg_delete_permanent(uint32_t index, bool endpoint)
{
    char filename[30];

    if (h_snprintf_strict(filename, 30, "/sg/%"PRIu32, index) < 0)
        return -1;

    return storage_rm(filename, true, true);
}

static int sg_read_permanent(shell_handle_t shell, uint32_t index, struct genavb_stream_gate_instance *instance, bool endpoint)
{
    struct genavb_stream_gate_control_entry *gate_list;
    unsigned int num_entries = 0;
    char path[30];
    int rc = -1; 
    int i;

    gate_list = instance->control_list;
    if (!gate_list)
        goto err;

    if (h_snprintf_strict(path, 30, "/sg/%"PRIu32, index) < 0)
        goto err;

    if (storage_cd(path, true) == 0) {
        instance->stream_gate_instance_id = index;
        storage_read_bool("gate_enable", &instance->gate_enable);
        storage_read_u8("admin_state", &instance->admin_gate_state);
        storage_read_u8("admin_ipv", &instance->admin_ipv);
        storage_read_u64("base_time", &instance->base_time);
        storage_read_u32("cycle_time", &instance->cycle_time_p);
        storage_read_u32("cycle_time_extension", &instance->cycle_time_extension);
        storage_read_uint("list_length", &instance->list_length);
        storage_read_bool("gate_closed_due_to_invalid_rx_enable", &instance->gate_closed_due_to_invalid_rx_enable);
        storage_read_bool("gate_closed_due_to_octets_exceeded_enable", &instance->gate_closed_due_to_octets_exceeded_enable);

        for (i = 0; i < instance->list_length; i++) {
            char entry[10] = {0};
            h_snprintf(entry, 10, "entry%u", i);

            if ((rc = sg_read_entry_from_storage(entry, &gate_list[i].gate_state_value, &gate_list[i].ipv_spec, &gate_list[i].time_interval_value, &gate_list[i].interval_octet_max)) == 0) {
                gate_list[i].operation_name = GENAVB_SG_SET_GATE_AND_IPV;
                num_entries++;
            }
        }

        storage_cd("-", true);
        instance->list_length = num_entries;
        rc = 0;
    }

err:
    return rc;
}

static void sg_set_default_parameters(struct genavb_stream_gate_instance *instance, struct genavb_stream_gate_control_entry *gate_list)
{
    instance->gate_enable = STREAM_GATE_DEFAULT_GATE_ENABLE;
    instance->admin_gate_state = STREAM_GATE_DEFAULT_ADMIN_STATE;
    instance->admin_ipv = STREAM_GATE_DEFAULT_ADMIN_IPV;
    instance->base_time = STREAM_GATE_DEFAULT_BASE_TIME;
    instance->cycle_time_p = STREAM_GATE_DEFAULT_CYCLE_TIME;
    instance->cycle_time_q = NSECS_PER_SEC;
    instance->cycle_time_extension = STREAM_GATE_DEFAULT_CYCLE_TIME_EXT;
    instance->gate_closed_due_to_invalid_rx_enable = STREAM_GATE_DEFAULT_INVALID_RX;
    instance->gate_closed_due_to_octets_exceeded_enable = STREAM_GATE_DEFAULT_OCTETS_EXCEEDED;
    instance->list_length = 1;
    instance->control_list = gate_list;
    instance->control_list[0].operation_name = GENAVB_SG_SET_GATE_AND_IPV;
    instance->control_list[0].gate_state_value = 1;
    instance->control_list[0].ipv_spec = GENAVB_IPV_SPEC_NULL;
    instance->control_list[0].time_interval_value = STREAM_GATE_DEFAULT_CYCLE_TIME;
    instance->control_list[0].interval_octet_max = 0xFFFFFFFF;
}

static void sg_apply_permanent(shell_handle_t shell)
{
    struct genavb_stream_gate_instance instance;
    struct genavb_stream_gate_control_entry gate_list[genavb_stream_gate_control_get_max_entries()];
    genavb_clock_id_t clk_id = GENAVB_CLOCK_BR_0_0;
    unsigned int i, index;
    char subdirname[MAX_DIR_NAME_LEN];
    int rc;

    i = 0;
    while (!storage_get_dir("/sg", i, subdirname, MAX_DIR_NAME_LEN)) {
        i++;

        if (sscanf(subdirname, "%u", &index) != 1)
            continue;

        sg_set_default_parameters(&instance, gate_list);

        if (sg_read_permanent(shell, index, &instance, false) == 0) {
            rc = genavb_stream_gate_update(index, clk_id, &instance);
            if (rc < 0)
                shell_printf(shell, "genavb_stream_gate_update(%u) failed: %s\n", index, genavb_strerror(rc));
        }
    }
}

static void print_sg_update_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(sg_update))->pcHelpString);
}

static shell_status_t sg_update(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_stream_gate_instance instance;
    struct genavb_stream_gate_control_entry gate_list[genavb_stream_gate_control_get_max_entries()];
    genavb_clock_id_t clk_id = GENAVB_CLOCK_BR_0_0;
    uint32_t index, num_list_entries = 0;
    bool permanent = false;
    bool reset_gate_closed_due_to_invalid_rx = false;
    bool reset_gate_closed_due_to_octets_exceeded = false;
    unsigned long long tmp0;
    unsigned long tmp1;
    unsigned int tmp[4];
    int opt, rc;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp1, argv[1], NULL, 0);
    index = tmp1;
    if (index >= genavb_stream_gate_get_max_entries()) {
        shell_printf(shell, "invalid stream gate index\n");
        goto err_usage;
    }

    sg_set_default_parameters(&instance, gate_list);

    instance.stream_gate_instance_id = index;

    sg_read_permanent(shell, instance.stream_gate_instance_id, &instance, 0);

    optind = 2;
    while ((opt = getopt(argc, argv, "e:s:P:c:C:b:l:I:iX:xp")) != -1) {
        switch (opt) {
        case 'e':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.gate_enable = tmp1;
            break;
        case 's':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.admin_gate_state = tmp1;
            break;
        case 'P':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.admin_ipv = tmp1;
            break;
        case 'c':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.cycle_time_p = tmp1;
            break;
        case 'C':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.cycle_time_extension = tmp1;
            break;
        case 'b':
            h_strtoull(&tmp0, optarg, NULL, 0);
            instance.base_time = tmp0;
            break;
        case 'l':
            if (sscanf(optarg, "%u,%u,%u,%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) == 4) {
                gate_list[num_list_entries].gate_state_value = tmp[0];
                gate_list[num_list_entries].ipv_spec = tmp[1];
                gate_list[num_list_entries].time_interval_value = tmp[2];
                gate_list[num_list_entries].interval_octet_max = tmp[3];
                if (++num_list_entries > genavb_stream_gate_control_get_max_entries()) {
                    shell_printf(shell, "number of entries must be less than or equal to %d\n", genavb_stream_gate_control_get_max_entries());
                    goto err_usage;
                }
            }
            break;
        case 'I':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.gate_closed_due_to_invalid_rx_enable = tmp1;
            break;
        case 'i':
            reset_gate_closed_due_to_invalid_rx = true;
            break;
        case 'X':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.gate_closed_due_to_octets_exceeded_enable = tmp1;
            break;
        case 'x':
            reset_gate_closed_due_to_octets_exceeded = true;
            break;
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (num_list_entries)
        instance.list_length = num_list_entries;

    if (permanent) {
        if (sg_update_permanent(shell, instance.stream_gate_instance_id, &instance, 0) < 0) {
            shell_printf(shell, "sg_update_permanent(%u) failed: Update permanent entry failed\n",
                instance.stream_gate_instance_id);
            goto err;
        }
    }

    if (reset_gate_closed_due_to_invalid_rx || reset_gate_closed_due_to_octets_exceeded) {
        if (reset_gate_closed_due_to_invalid_rx) {
            rc = genavb_stream_gate_reset_rx_invalid(instance.stream_gate_instance_id);
            if (rc < 0) {
                shell_printf(shell, "genavb_stream_gate_reset_rx_invalid(%u) failed: %s\n",
                    instance.stream_gate_instance_id, genavb_strerror(rc));
                goto err;
            }
        }

        if (reset_gate_closed_due_to_octets_exceeded) {
            rc = genavb_stream_gate_reset_octets_exceeded(instance.stream_gate_instance_id);
            if (rc < 0) {
                shell_printf(shell, "genavb_stream_gate_reset_octets_exceeded(%u) failed: %s\n",
                    instance.stream_gate_instance_id, genavb_strerror(rc));
                goto err;
            }
        }
    } else {
        rc = genavb_stream_gate_update(instance.stream_gate_instance_id, clk_id, &instance);
        if (rc < 0) {
            shell_printf(shell, "genavb_stream_gate_update(%u) failed: %s\n",
                instance.stream_gate_instance_id, genavb_strerror(rc));
            goto err;
        }
    }

    return kStatus_SHELL_Success;

err_usage:
    print_sg_update_usage(shell);

err:
    return kStatus_SHELL_Error;
}

static void print_sg_delete_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(sg_delete))->pcHelpString);
}

static shell_status_t sg_delete(shell_handle_t shell, int32_t argc, char **argv)
{
    bool permanent = false;
    unsigned long tmp;
    uint32_t index;
    int rc, opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_stream_gate_get_max_entries()) {
        shell_printf(shell, "invalid stream gate index\n");
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
        if (sg_delete_permanent(index, 0) < 0) {
            shell_printf(shell, "sg_delete_permanent(%u) failed: Entry does not exist\n", index);
            goto err;
        }
    }

    rc = genavb_stream_gate_delete(index);
    if (rc < 0) {
        shell_printf(shell, "genavb_stream_gate_delete(%u) failed: %s\n", index, genavb_strerror(rc));
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_sg_delete_usage(shell);

err:
    return kStatus_SHELL_Error;
}

static void print_sg_read_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(sg_read))->pcHelpString);
}

static shell_status_t sg_read(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_stream_gate_instance instance = {0};
    struct genavb_stream_gate_control_entry gate_list[genavb_stream_gate_control_get_max_entries()];
    genavb_sg_config_type_t sg_config_type = GENAVB_SG_OPER;
    bool permanent = false;
    unsigned long tmp;
    uint32_t index;
    int rc, opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_stream_gate_get_max_entries()) {
        shell_printf(shell, "invalid stream gate index\n");
        goto err_usage;
    }

    sg_set_default_parameters(&instance, gate_list);

    instance.list_length = genavb_stream_gate_control_get_max_entries();

    optind = 2;
    while ((opt = getopt(argc, argv, "pt:")) != -1) {
        switch (opt) {
        case 'p':
            permanent = true;
            break;
        case 't':
            h_strtoul(&tmp, optarg, NULL, 0);
            sg_config_type = (genavb_sg_config_type_t)tmp;
            break;
        default:
            break;
        }
    }

    if (permanent) {
        if (sg_read_permanent(shell, index, &instance, 0) < 0) {
            shell_printf(shell, "sg_read_permanent(%u) failed: Entry does not exist\n", index);
            goto no_entry;
        }
    } else {
        rc = genavb_stream_gate_read(index, sg_config_type, &instance);
        if (rc < 0) {
            shell_printf(shell, "genavb_stream_gate_read(%u) failed: %s\n", index, genavb_strerror(rc));
            goto no_entry;
        } 
    }

    sg_print_entry(shell, index, &instance);

no_entry:
    return kStatus_SHELL_Success;

err_usage:
    print_sg_read_usage(shell);

    return kStatus_SHELL_Error;
}

static void fm_print_entry(shell_handle_t shell, uint32_t index, struct genavb_flow_meter_instance *instance)
{
    shell_printf(shell, "\n");
    shell_printf(shell, "Flow Meter %"PRIu32"\n", index);

    shell_printf(shell, "           cir         |    cbs     |          eir          |    ebs     |  cflag  |  cmode  |  dropy  | mren |  mr  |\n");
    shell_printf(shell, "-----------------------+------------+-----------------------+------------+---------+---------+---------+------+------|\n");
    shell_printf(shell, " % 21" PRIu64 " |", instance->committed_information_rate);
    shell_printf(shell, " % 10" PRIu32 " |", instance->committed_burst_size);
    shell_printf(shell, " % 21" PRIu64 " |", instance->excess_information_rate);
    shell_printf(shell, " % 10" PRIu32 " |", instance->excess_burst_size);
    shell_printf(shell, " % 7" PRIu32 " |", instance->coupling_flag);
    shell_printf(shell, " % 7" PRIu32 " |", instance->color_mode);
    shell_printf(shell, " % 7" PRIu32 " |", instance->drop_on_yellow);
    shell_printf(shell, " % 4" PRIu32 " |", instance->mark_all_frames_red_enable);
    shell_printf(shell, " % 4" PRIu32 " |", instance->mark_all_frames_red);
    shell_printf(shell, "\n");

    shell_printf(shell, "\n");
}

static int fm_update_permanent(shell_handle_t shell, uint32_t index, struct genavb_flow_meter_instance *instance, bool endpoint)
{
    char path[MAX_FILENAME_LENGTH];
    int rc = 0;

    if (h_snprintf_strict(path, MAX_FILENAME_LENGTH, "/fm/%"PRIu32, index) < 0) {
        rc = -1;
        goto err;
    }

    if (storage_mkdir(path, true) < 0) {
        rc = -1;
        goto err;
    }

    storage_cd(path, true);

    storage_write_u64("cir", instance->committed_information_rate);
    storage_write_uint("cbs", instance->committed_burst_size);
    storage_write_u64("eir", instance->excess_information_rate);
    storage_write_uint("ebs", instance->excess_burst_size);
    storage_write_uint("cflag", instance->coupling_flag);
    storage_write_uint("cmode", instance->color_mode);
    storage_write_uint("dropy", instance->drop_on_yellow);
    storage_write_uint("mren", instance->mark_all_frames_red_enable);

    storage_cd("-", true);

err:
    return rc;
}

static int fm_delete_permanent(uint32_t index, bool endpoint)
{
    char filename[MAX_FILENAME_LENGTH];

    if (h_snprintf_strict(filename, MAX_FILENAME_LENGTH, "/fm/%"PRIu32, index) < 0)
        return -1;

    return storage_rm(filename, true, true);
}

static int fm_read_permanent(shell_handle_t shell, uint32_t index, struct genavb_flow_meter_instance *instance, bool endpoint)
{
    char path[MAX_FILENAME_LENGTH];
    int rc = -1; 

    if (h_snprintf_strict(path, MAX_FILENAME_LENGTH, "/fm/%"PRIu32, index) < 0)
        goto err;

    if (storage_cd(path, true) == 0) {
        instance->flow_meter_instance_id = index;
        storage_read_u64("cir", &instance->committed_information_rate);
        storage_read_u32("cbs", &instance->committed_burst_size);
        storage_read_u64("eir", &instance->excess_information_rate);
        storage_read_u32("ebs", &instance->excess_burst_size);
        storage_read_u8("cflag", &instance->coupling_flag);
        storage_read_u8("cmode", &instance->color_mode);
        storage_read_bool("dropy", &instance->drop_on_yellow);
        storage_read_bool("mren", &instance->mark_all_frames_red_enable);

        storage_cd("-", true);
        rc = 0;
    }

    return rc;

err:
    return rc;
}

static void fm_apply_permanent(shell_handle_t shell)
{
    struct genavb_flow_meter_instance instance;
    unsigned int i, index;
    char subdirname[MAX_DIR_NAME_LEN];
    int rc;

    i = 0;
    while (!storage_get_dir("/fm", i, subdirname, MAX_DIR_NAME_LEN)) {
        i++;

        if (sscanf(subdirname, "%u", &index) != 1)
            continue;

        instance = flow_meter_default;

        if (fm_read_permanent(shell, index, &instance, false) == 0) {
            rc = genavb_flow_meter_update(index, &instance);
            if (rc < 0)
                shell_printf(shell, "genavb_flow_meter_update(%u) failed: %s\n", index, genavb_strerror(rc));
        }
    }
}

static void print_fm_update_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fm_update))->pcHelpString);
}

static shell_status_t fm_update(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_flow_meter_instance instance = flow_meter_default;
    bool permanent = false;
    bool mark_red_reset = false;
    unsigned long long tmp0;
    unsigned long tmp1;
    uint32_t index;
    int opt, rc;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp1, argv[1], NULL, 0);
    index = tmp1;
    if (index >= genavb_flow_meter_get_max_entries()) {
        shell_printf(shell, "invalid flow meter index\n");
        goto err_usage;
    }

    instance.flow_meter_instance_id = index;

    fm_read_permanent(shell, instance.flow_meter_instance_id, &instance, false);

    optind = 2;
    while ((opt = getopt(argc, argv, "r:b:R:B:f:c:y:m:Mp")) != -1) {
        switch (opt) {
        case 'r':
            h_strtoull(&tmp0, optarg, NULL, 0);
            instance.committed_information_rate = tmp0;
            break;
        case 'b':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.committed_burst_size = tmp1;
            break;
        case 'R':
            h_strtoull(&tmp0, optarg, NULL, 0);
            instance.excess_information_rate = tmp0;
            break;
        case 'B':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.excess_burst_size = tmp1;
            break;
        case 'f':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.coupling_flag = tmp1;
            break;
        case 'c':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.color_mode = tmp1;
            break;
        case 'y':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.drop_on_yellow = tmp1;
            break;
        case 'm':
            h_strtoul(&tmp1, optarg, NULL, 0);
            instance.mark_all_frames_red_enable = tmp1;
            break;
        case 'M':
            mark_red_reset = true;
            break;
        case 'p':
            permanent = true;
            break;
        default:
            break;
        }
    }

    if (mark_red_reset) {
        rc = genavb_flow_meter_mark_red_reset(instance.flow_meter_instance_id);
        if (rc < 0) {
            shell_printf(shell, "genavb_flow_meter_mark_red_reset(%u) failed: %s\n",
                instance.flow_meter_instance_id, genavb_strerror(rc));
            goto err;
        }
    } else {
        if (permanent) {
            if (fm_update_permanent(shell, instance.flow_meter_instance_id, &instance, false) < 0) {
                shell_printf(shell, "fm_update_permanent(%u) failed: Update permanent entry failed\n",
                    instance.flow_meter_instance_id);
                goto err;
            }
        }

        rc = genavb_flow_meter_update(instance.flow_meter_instance_id, &instance);
        if (rc < 0) {
            shell_printf(shell, "genavb_flow_meter_update(%u) failed: %s\n",
                instance.flow_meter_instance_id, genavb_strerror(rc));
            goto err;
        }
    }

    return kStatus_SHELL_Success;

err_usage:
    print_fm_update_usage(shell);

err:
    return kStatus_SHELL_Error;
}

static void print_fm_delete_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fm_delete))->pcHelpString);
}

static shell_status_t fm_delete(shell_handle_t shell, int32_t argc, char **argv)
{
    bool permanent = false;
    unsigned long tmp;
    uint32_t index;
    int opt, rc;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_flow_meter_get_max_entries()) {
        shell_printf(shell, "invalid flow meter index\n");
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
        if (fm_delete_permanent(index, false) < 0) {
            shell_printf(shell, "fm_delete_permanent(%u) failed: Entry does not exist\n", index);
            goto err;
        }
    }

    rc = genavb_flow_meter_delete(index);
    if (rc < 0) {
        shell_printf(shell, "genavb_flow_meter_delete(%u) failed: %s\n", index, genavb_strerror(rc));
        goto err;
    }

    return kStatus_SHELL_Success;

err_usage:
    print_fm_delete_usage(shell);

err:
    return kStatus_SHELL_Error;
}

static void print_fm_read_usage(shell_handle_t shell)
{
    shell_printf(shell, "Usage: ");
    shell_printf(shell, (SHELL_COMMAND(fm_read))->pcHelpString);
}

static shell_status_t fm_read(shell_handle_t shell, int32_t argc, char **argv)
{
    struct genavb_flow_meter_instance instance = flow_meter_default;
    bool permanent = false;
    unsigned long tmp;
    uint32_t index;
    int rc, opt;

    if (argc < 2)
        goto err_usage;

    h_strtoul(&tmp, argv[1], NULL, 0);
    index = tmp;
    if (index >= genavb_flow_meter_get_max_entries()) {
        shell_printf(shell, "invalid flow meter index\n");
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
        if (fm_read_permanent(shell, index, &instance, false) < 0) {
            shell_printf(shell, "fm_read_permanent(%u) failed: Entry does not exist\n", index);
            goto no_entry;
        }
    } else {
        rc = genavb_flow_meter_read(index, &instance);
        if (rc < 0) {
            shell_printf(shell, "genavb_flow_meter_read(%u) failed: %s\n", index, genavb_strerror(rc));
            goto err;
        }
    }

    fm_print_entry(shell, index, &instance);

no_entry:
    return kStatus_SHELL_Success;

err_usage:
    print_fm_read_usage(shell);

err:
    return kStatus_SHELL_Error;
}

void psfp_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sf_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sf_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sf_read));

    sf_apply_permanent(shell);

    SHELL_RegisterCommand(shell, SHELL_COMMAND(sg_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sg_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(sg_read));

    sg_apply_permanent(shell);

    SHELL_RegisterCommand(shell, SHELL_COMMAND(fm_update));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fm_delete));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(fm_read));

    fm_apply_permanent(shell);
}
