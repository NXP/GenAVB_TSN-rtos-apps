/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2018-2019, 2024, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 @file avb_stream.h
 @brief      This file implements GENAVB wrapper layer interfaces.
 @details    Copyright 2016 Freescale Semiconductor, Inc.
*/

#include <string.h>

#include "genavb/genavb.h"
#include "avb_tsn/common/log.h"
#include "avb_tsn/common/common.h"

#define MAX_GENAVB_LISTENER_REGISTRATIONS 32
#define MAX_GENAVB_TALKER_REGISTRATIONS   32

static struct genavb_control_handle *s_msrp_handle = NULL;

struct msrp_registration_t {
    avb_u8 stream_id[8];
    unsigned int ref_count;
};

static struct msrp_registration_t msrp_listener_registrations[MAX_GENAVB_LISTENER_REGISTRATIONS];
static struct msrp_registration_t msrp_talker_registrations[MAX_GENAVB_TALKER_REGISTRATIONS];

__init int app_msrp_init(struct genavb_handle *s_genavb_handle)
{
    int genavb_result;
    int rc;

    genavb_result = genavb_control_open(s_genavb_handle, &s_msrp_handle, GENAVB_CTRL_MSRP);
    if (genavb_result != GENAVB_SUCCESS) {
        log_err("genavb_control_open() failed: %s\n", genavb_strerror(genavb_result));
        rc = -1;
        goto err_control_open;
    }

    memset(msrp_listener_registrations, 0, sizeof(msrp_listener_registrations));
    memset(msrp_talker_registrations, 0, sizeof(msrp_talker_registrations));

    log_debug("msrp control handle: %p\n", s_msrp_handle);

    return 0;

err_control_open:
    return rc;
}

__exit int app_msrp_exit(void)
{
    genavb_control_close(s_msrp_handle);

    s_msrp_handle = NULL;

    return 0;
}

static int _app_msrp_talker_deregister(const struct genavb_stream_params *stream_params)
{
    struct genavb_msg_talker_deregister talker_deregister;
    struct genavb_msg_talker_response talker_response;
    unsigned int msg_type, msg_len;
    int rc;

    log_debug("stream_params: %p\n", stream_params);
    talker_deregister.port = stream_params->port;
    memcpy(talker_deregister.stream_id, stream_params->stream_id, 8);

    msg_type = GENAVB_MSG_TALKER_DEREGISTER;
    msg_len = sizeof(talker_response);
    rc = genavb_control_send_sync(s_msrp_handle, (genavb_msg_type_t *)&msg_type, &talker_deregister, sizeof(talker_deregister), &talker_response, &msg_len, 1000);
    if ((rc != GENAVB_SUCCESS) || (msg_type != GENAVB_MSG_TALKER_RESPONSE) || (talker_response.status != GENAVB_SUCCESS)) {
        log_err(STREAM_STR_FMT " failed: %s\n", STREAM_STR(stream_params->stream_id), genavb_strerror(rc));

        return -1;
    }

    return 0;
}

static int _app_msrp_talker_register(const struct genavb_stream_params *stream_params)
{
    struct genavb_msg_talker_register talker_register;
    struct genavb_msg_talker_response talker_response;
    unsigned int max_frame_size, max_interval_frames;
    unsigned int msg_type, msg_len;
    int rc;

    log_debug("stream_params: %p\n", stream_params);
    talker_register.port = stream_params->port;
    memcpy(talker_register.stream_id, stream_params->stream_id, 8);

    talker_register.params.stream_class = stream_params->stream_class;
    memcpy(talker_register.params.destination_address, stream_params->dst_mac, 6);
    talker_register.params.vlan_id = VLAN_VID_DEFAULT;

    if (stream_params->flags & GENAVB_STREAM_FLAGS_CUSTOM_TSPEC) {
        max_frame_size = stream_params->talker.max_frame_size;
        max_interval_frames = stream_params->talker.max_interval_frames;
    } else
        avdecc_fmt_tspec(&stream_params->format, stream_params->stream_class, &max_frame_size, &max_interval_frames);

    talker_register.params.max_frame_size = max_frame_size;
    talker_register.params.max_interval_frames = max_interval_frames;
    talker_register.params.accumulated_latency = 0;
    talker_register.params.rank = NORMAL;

    msg_type = GENAVB_MSG_TALKER_REGISTER;
    msg_len = sizeof(talker_response);
    rc = genavb_control_send_sync(s_msrp_handle, (genavb_msg_type_t *)&msg_type, &talker_register, sizeof(talker_register), &talker_response, &msg_len, 1000);
    if ((rc != GENAVB_SUCCESS) || (msg_type != GENAVB_MSG_TALKER_RESPONSE) || (talker_response.status != GENAVB_SUCCESS)) {
        log_err(STREAM_STR_FMT " failed: %s\n", STREAM_STR(stream_params->stream_id), genavb_strerror(rc));

        return -1;
    }

    return 0;
}

static int _app_msrp_listener_deregister(const struct genavb_stream_params *stream_params)
{
    struct genavb_msg_listener_deregister listener_deregister;
    struct genavb_msg_listener_response listener_response;
    unsigned int msg_type, msg_len;
    int rc;

    log_debug("stream_params: %p\n", stream_params);
    listener_deregister.port = stream_params->port;
    memcpy(listener_deregister.stream_id, stream_params->stream_id, 8);

    msg_type = GENAVB_MSG_LISTENER_DEREGISTER;
    msg_len = sizeof(listener_response);
    rc = genavb_control_send_sync(s_msrp_handle, (genavb_msg_type_t *)&msg_type, &listener_deregister, sizeof(listener_deregister), &listener_response, &msg_len, 1000);
    if ((rc != GENAVB_SUCCESS) || (msg_type != GENAVB_MSG_LISTENER_RESPONSE) || (listener_response.status != GENAVB_SUCCESS)) {
        log_err(STREAM_STR_FMT " failed: %s\n", STREAM_STR(stream_params->stream_id), genavb_strerror(rc));

        return -1;
    }

    return 0;
}

static int _app_msrp_listener_register(const struct genavb_stream_params *stream_params)
{
    struct genavb_msg_listener_register listener_register;
    struct genavb_msg_listener_response listener_response;
    unsigned int msg_type, msg_len;
    int rc;

    log_debug("stream_params: %p\n", stream_params);
    listener_register.port = stream_params->port;
    memcpy(listener_register.stream_id, stream_params->stream_id, 8);

    msg_type = GENAVB_MSG_LISTENER_REGISTER;
    msg_len = sizeof(listener_response);
    rc = genavb_control_send_sync(s_msrp_handle, (genavb_msg_type_t *)&msg_type, &listener_register, sizeof(listener_register), &listener_response, &msg_len, 1000);
    if ((rc != GENAVB_SUCCESS) || (msg_type != GENAVB_MSG_LISTENER_RESPONSE) || (listener_response.status != GENAVB_SUCCESS)) {
        log_err(STREAM_STR_FMT " failed: %s\n", STREAM_STR(stream_params->stream_id), genavb_strerror(rc));

        return -1;
    }

    return 0;
}

static struct msrp_registration_t *_app_msrp_find_listener_registration(unsigned char *stream_id)
{
    struct msrp_registration_t *registration;
    int i;

    for (i = 0; i < MAX_GENAVB_LISTENER_REGISTRATIONS; ++i) {
        registration = &msrp_listener_registrations[i];

        if (memcmp(registration->stream_id, stream_id, sizeof(registration->stream_id)) == 0) {
            log_debug("found matching registration %p at index %d for stream " STREAM_STR_FMT "\n", registration, i, STREAM_STR(stream_id));
            return registration;
        }
    }

    // No existing registration, find a free one
    for (i = 0; i < MAX_GENAVB_LISTENER_REGISTRATIONS; ++i) {
        registration = &msrp_listener_registrations[i];

        if (!registration->ref_count) {
            memcpy(registration->stream_id, stream_id, sizeof(registration->stream_id));
            log_debug("found free registration %p at index %d\n", registration, i);
            return registration;
        }
    }

    return NULL;
}

int app_msrp_listener_register(struct genavb_stream_params *stream_params)
{
    struct msrp_registration_t *registration;
    int result = 0;

    log_debug("stream_id: " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));

    registration = _app_msrp_find_listener_registration(stream_params->stream_id);

    if (!registration) {
        log_err("Couldn't find free registration for stream " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
        result = -1;
        goto err;
    }

    if (!registration->ref_count)
        result = _app_msrp_listener_register(stream_params);

    if (result == 0)
        registration->ref_count++;

err:
    return result;
}

void app_msrp_listener_deregister(struct genavb_stream_params *stream_params)
{
    struct msrp_registration_t *registration;

    log_debug("stream_id: " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));

    registration = _app_msrp_find_listener_registration(stream_params->stream_id);

    if (!registration) {
        log_err("Couldn't find registration for stream " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
        return;
    }

    if (!registration->ref_count) {
        log_err("Tried to deregister an unregistered MSRP listener " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
        return;
    }

    registration->ref_count--;

    if (!registration->ref_count)
        _app_msrp_listener_deregister(stream_params);
}

static struct msrp_registration_t *app_msrp_find_talker_registration(unsigned char *stream_id)
{
    struct msrp_registration_t *registration;
    int i;

    for (i = 0; i < MAX_GENAVB_TALKER_REGISTRATIONS; ++i) {
        registration = &msrp_talker_registrations[i];

        if (memcmp(registration->stream_id, stream_id, sizeof(registration->stream_id)) == 0) {
            log_debug("found matching registration %p at index %d for stream " STREAM_STR_FMT "\n", registration, i, STREAM_STR(stream_id));
            return registration;
        }
    }

    // No existing registration, find a free one
    for (i = 0; i < MAX_GENAVB_TALKER_REGISTRATIONS; ++i) {
        registration = &msrp_talker_registrations[i];

        if (!registration->ref_count) {
            memcpy(registration->stream_id, stream_id, sizeof(registration->stream_id));
            log_debug("found free registration %p at index %d\n", registration, i);
            return registration;
        }
    }

    return NULL;
}

int app_msrp_talker_register(struct genavb_stream_params *stream_params)
{
    struct msrp_registration_t *registration;
    int result = 0;

    log_debug("stream_id: " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));

    registration = app_msrp_find_talker_registration(stream_params->stream_id);

    if (!registration) {
        log_err("Couldn't find free registration for stream " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
        result = -1;
        goto err;
    }

    if (!registration->ref_count)
        result = _app_msrp_talker_register(stream_params);

    if (result == 0)
        registration->ref_count++;

err:
    return result;
}

void app_msrp_talker_deregister(struct genavb_stream_params *stream_params)
{
    struct msrp_registration_t *registration;

    log_debug("stream_id: " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
    registration = app_msrp_find_talker_registration(stream_params->stream_id);

    if (!registration) {
        log_err("Couldn't find registration for stream " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
        return;
    }

    if (!registration->ref_count) {
        log_err("Tried to deregister an unregistered MSRP listener " STREAM_STR_FMT "\n", STREAM_STR(stream_params->stream_id));
        return;
    }

    registration->ref_count--;

    if (!registration->ref_count)
        _app_msrp_talker_deregister(stream_params);
}
