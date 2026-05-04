/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2017-2020, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include <genavb/genavb.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "clock_domain.h"
#include "avb_tsn/common/log.h"
#include "avb_tsn/common/common.h"

static struct genavb_control_handle *s_clk_handle = NULL;
static int audio_clk_sync[GENAVB_CLOCK_DOMAIN_MAX] = {0};
static unsigned int clk_domain_is_valid[GENAVB_CLOCK_DOMAIN_MAX] = {0};
static SemaphoreHandle_t clk_domain_mutex;
static StaticSemaphore_t clk_domain_mutex_buffer;

static const char *clk_domain_name[] = {
    [GENAVB_CLOCK_DOMAIN_0] = "GENAVB_CLOCK_DOMAIN_0",
    [GENAVB_CLOCK_DOMAIN_1] = "GENAVB_CLOCK_DOMAIN_1",
    [GENAVB_CLOCK_DOMAIN_2] = "GENAVB_CLOCK_DOMAIN_2",
    [GENAVB_CLOCK_DOMAIN_3] = "GENAVB_CLOCK_DOMAIN_3",
    [GENAVB_CLOCK_DOMAIN_MAX] = "GENAVB_CLOCK_DOMAIN_MAX"};

static const char *clk_source_type_name[] = {
    [GENAVB_CLOCK_SOURCE_TYPE_INTERNAL] = "GENAVB_CLOCK_SOURCE_TYPE_INTERNAL",
    [GENAVB_CLOCK_SOURCE_TYPE_INPUT_STREAM] = "GENAVB_CLOCK_SOURCE_TYPE_INPUT_STREAM"};

static const char *clk_domain_status_name[] = {
    [GENAVB_CLOCK_DOMAIN_STATUS_UNLOCKED] = "GENAVB_CLOCK_DOMAIN_STATUS_UNLOCKED",
    [GENAVB_CLOCK_DOMAIN_STATUS_LOCKED] = "GENAVB_CLOCK_DOMAIN_STATUS_LOCKED",
    [GENAVB_CLOCK_DOMAIN_STATUS_FREE_WHEELING] = "GENAVB_CLOCK_DOMAIN_STATUS_FREE_WHEELING",
    [GENAVB_CLOCK_DOMAIN_STATUS_HW_ERROR] = "GENAVB_CLOCK_DOMAIN_STATUS_HW_ERROR"};

static const char *get_genavb_clock_domain_t_name(genavb_clock_domain_t type)
{
    if ((type < GENAVB_CLOCK_DOMAIN_0) || (type > GENAVB_CLOCK_DOMAIN_MAX))
        return "<Unknown clock domain>";
    else
        return clk_domain_name[type];
}

static const char *get_genavb_clock_source_type_t_name(genavb_clock_source_type_t type)
{
    if ((type < GENAVB_CLOCK_SOURCE_TYPE_INTERNAL) || (type > GENAVB_CLOCK_SOURCE_TYPE_INPUT_STREAM))
        return "<Unknown clock source>";
    else
        return clk_source_type_name[type];
}

static const char *get_genavb_clock_domain_status_t_name(genavb_clock_domain_status_t type)
{
    if ((type < GENAVB_CLOCK_DOMAIN_STATUS_UNLOCKED) || (type > GENAVB_CLOCK_DOMAIN_STATUS_HW_ERROR))
        return "<Unknown clock domain status>";
    else
        return clk_domain_status_name[type];
}

int get_clk_domain_validity(genavb_clock_domain_t clk_domain)
{
    unsigned int validity = 0;

    xSemaphoreTake(clk_domain_mutex, portMAX_DELAY);

    if (clk_domain < GENAVB_CLOCK_DOMAIN_MAX)
        validity = clk_domain_is_valid[clk_domain];
    else
        validity = -1;

    xSemaphoreGive(clk_domain_mutex);

    return validity;
}

static void set_clk_domain_validity(genavb_clock_domain_t clk_domain, int validity)
{
    xSemaphoreTake(clk_domain_mutex, portMAX_DELAY);

    if (clk_domain < GENAVB_CLOCK_DOMAIN_MAX)
        clk_domain_is_valid[clk_domain] = validity;
    else
        log_err("cannot set audio clock validity, clk_domain unknown");

    xSemaphoreGive(clk_domain_mutex);
}

int handle_clock_domain_event(void)
{
    union genavb_msg_clock_domain msg;
    genavb_msg_type_t msg_type;
    unsigned int msg_len;
    int rc;

    msg_len = sizeof(union genavb_msg_clock_domain);
    if ((rc = genavb_control_receive(s_clk_handle, &msg_type, &msg, &msg_len)) != GENAVB_SUCCESS) {
        rc = -1;
        goto error_control_receive;
    }

    switch (msg_type) {
    case AVB_MSG_CLOCK_DOMAIN_STATUS:

        if ((msg.status.status == GENAVB_CLOCK_DOMAIN_STATUS_LOCKED) || (msg.status.status == GENAVB_CLOCK_DOMAIN_STATUS_FREE_WHEELING)) {
            log_info("AVB_MSG_CLOCK_DOMAIN_STATUS - domain: %s, source_type: %s, status: %s, Setting clk domain validity to TRUE\n", get_genavb_clock_domain_t_name(msg.status.domain), get_genavb_clock_source_type_t_name(msg.status.source_type), get_genavb_clock_domain_status_t_name(msg.status.status));
            set_clk_domain_validity(msg.status.domain, 1);
        }

        if (msg.status.status == GENAVB_CLOCK_DOMAIN_STATUS_UNLOCKED) {
            log_info("AVB_MSG_CLOCK_DOMAIN_STATUS - domain: %s, source_type: %s, status: %s, Setting clk domain validity to FALSE\n", get_genavb_clock_domain_t_name(msg.status.domain), get_genavb_clock_source_type_t_name(msg.status.source_type), get_genavb_clock_domain_status_t_name(msg.status.status));
            set_clk_domain_validity(msg.status.domain, 0);
        }

        break;

    default:
        log_err("Error, unknown message type: %d\n", msg_type);
        rc = -1;
        break;
    }

error_control_receive:
    return rc;
}

void clock_domain_get_status(genavb_clock_domain_t domain)
{
    struct genavb_msg_clock_domain_get_status get_status;
    unsigned int msg_len = sizeof(get_status);
    genavb_msg_type_t msg_type = AVB_MSG_CLOCK_DOMAIN_GET_STATUS;
    int rc;

    get_status.domain = domain;

    rc = genavb_control_send(s_clk_handle, msg_type, &get_status, msg_len);
    if (rc != GENAVB_SUCCESS)
        log_err("clock_domain_get_status (AVB_CTRL_CLOCK_DOMAIN) failed: %s\n", genavb_strerror(rc));
}

int get_audio_clk_sync(genavb_clock_domain_t clk_domain)
{
    if (clk_domain < GENAVB_CLOCK_DOMAIN_MAX)
        return audio_clk_sync[clk_domain];

    return -1;
}

void set_audio_clk_sync(genavb_clock_domain_t clk_domain, int clk_sync)
{
    if (clk_domain < GENAVB_CLOCK_DOMAIN_MAX)
        audio_clk_sync[clk_domain] = clk_sync;
    else
        log_err("cannot set audio clock sync, clk_domain unknown\n");
}

static int clock_domain_set_source(struct genavb_msg_clock_domain_set_source *set_source)
{
    struct genavb_msg_clock_domain_response set_source_rsp;
    unsigned int msg_len = sizeof(struct genavb_msg_clock_domain_response);
    unsigned int msg_type = GENAVB_MSG_CLOCK_DOMAIN_SET_SOURCE;
    int rc;

    rc = genavb_control_send_sync(s_clk_handle, (genavb_msg_type_t *)&msg_type, set_source, sizeof(struct genavb_msg_clock_domain_set_source), &set_source_rsp, &msg_len, 1000);
    if ((rc == GENAVB_SUCCESS) && (msg_type == GENAVB_MSG_CLOCK_DOMAIN_RESPONSE))
        rc = set_source_rsp.status;

    return rc;
}

int clock_domain_set_source_internal(genavb_clock_domain_t domain,
                                     genavb_clock_source_local_id_t local_id)
{
    struct genavb_msg_clock_domain_set_source set_source;

    set_source.domain = domain;
    set_source.source_type = GENAVB_CLOCK_SOURCE_TYPE_INTERNAL;
    set_source.local_id = local_id;

    return clock_domain_set_source(&set_source);
}

int clock_domain_set_source_stream(genavb_clock_domain_t domain,
                                   struct genavb_stream_params *stream_params)
{
    struct genavb_msg_clock_domain_set_source set_source;

    set_source.domain = domain;
    set_source.source_type = GENAVB_CLOCK_SOURCE_TYPE_INPUT_STREAM;

    memcpy(set_source.stream_id, stream_params->stream_id, 8);

    return clock_domain_set_source(&set_source);
}

int clock_domain_set_role(media_clock_role_t role, genavb_clock_domain_t domain,
                          struct genavb_stream_params *stream_params)
{
    int rc = 0;

    set_audio_clk_sync(domain, 0);

    /* Master */
    if (role == MEDIA_CLOCK_MASTER) {
        /* If possible try to configure with internal HW source */
        if (clock_domain_set_source_internal(domain, GENAVB_CLOCK_SOURCE_AUDIO_CLK) != GENAVB_SUCCESS) {
            log_info("cannot set clock source to internal audio clock\n");

            /* Fallback */
            rc = clock_domain_set_source_internal(domain, GENAVB_CLOCK_SOURCE_PTP_CLK);
            if (rc != GENAVB_SUCCESS) {
                log_err("cannot set clock source to PTP based clock, rc = %d\n", rc);
                goto exit;
            }
            log_info("successfull fallback to PTP based clock\n");
        } else {
            set_audio_clk_sync(domain, 1);
            log_info("clock source setup to internal audio clock\n");
        }
    }
    /* Slave */
    else {
        if (!stream_params) {
            log_err("slave role requires a stream argument\n");
            goto exit;
        }

        rc = clock_domain_set_source_stream(domain, stream_params);
        if (rc != GENAVB_SUCCESS) {
            log_err("clock_domain_set_source_stream error, rc = %d\n", rc);
            goto exit;
        }

        set_audio_clk_sync(domain, 1);
        log_info("clock source setup to stream ID " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
    }

exit:
    return rc;
}

__init int app_clock_domain_init(struct genavb_handle *s_genavb_handle)
{
    int rc;

    clk_domain_mutex = xSemaphoreCreateMutexStatic(&clk_domain_mutex_buffer);
    if (!clk_domain_mutex)
        return -1;

    rc = genavb_control_open(s_genavb_handle, &s_clk_handle, GENAVB_CTRL_CLOCK_DOMAIN);
    if (rc != GENAVB_SUCCESS) {
        log_err("genavb_control_open(GENAVB_CTRL_CLOCK_DOMAIN)  failed: %s\n", genavb_strerror(rc));
        return -1;
    }

    log_debug("clock domain control handle: %p\n", s_clk_handle);

    return 0;
}

__exit int app_clock_domain_exit(void)
{
    genavb_control_close(s_clk_handle);

    s_clk_handle = NULL;

    return 0;
}
