/*
 * Copyright 2017-2019, 2024-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 @file crf_stream.c
 @brief      This file implements GENAVB wrapper layer interfaces.
 @details    Copyright 2017 NXP
*/

#include "genavb/genavb.h"

#include "avb_tsn/common/common.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/log.h"

#include "avb_stream.h"
#include "crf_stream.h"
#include "clock_domain.h"
#include "msrp.h"

aar_crf_stream_t *crf_stream_get(media_clock_role_t role)
{
    int i;

    for (i = 0; i < MAX_CRF_STREAMS; i++) {
        if (role == MEDIA_CLOCK_MASTER) {
            if (g_crf_streams[i].stream_params.direction == AVTP_DIRECTION_TALKER)
                return &g_crf_streams[i];
        } else {
            if (g_crf_streams[i].stream_params.direction == AVTP_DIRECTION_LISTENER)
                return &g_crf_streams[i];
        }
    }
    return NULL;
}

__init int crf_stream_create(media_clock_role_t role)
{
    aar_crf_stream_t *crf;
    int rc;

    struct genavb_handle *genavb_handle = get_genavb_handle();

    crf = crf_stream_get(role);
    if (!crf) {
        log_err("cannot get CRF stream");
        goto err_crf;
    }

    log_info("stream_id: " STREAM_STR_FMT "\n", STREAM_STR(crf->stream_params.stream_id));
    log_info("dst_mac: " MAC_STR_FMT "\n", MAC_STR(crf->stream_params.dst_mac));

    crf->cur_batch_size = genavbstream_batch_size(crf->batch_size_ns, &crf->stream_params);

    /* The app is not aware of which SR classes are enabled, so different values are tried */
    rc = avb_stream_create(genavb_handle, &crf->stream_handle, &crf->stream_params, &crf->cur_batch_size, 0);
    if (rc != GENAVB_SUCCESS) {
        crf->stream_params.stream_class = SR_CLASS_C;

        rc = avb_stream_create(genavb_handle, &crf->stream_handle, &crf->stream_params, &crf->cur_batch_size, 0);
        if (rc != GENAVB_SUCCESS) {
            crf->stream_params.stream_class = SR_CLASS_E;

            rc = avb_stream_create(genavb_handle, &crf->stream_handle, &crf->stream_params, &crf->cur_batch_size, 0);
            if (rc != GENAVB_SUCCESS) {
                crf->stream_params.stream_class = SR_CLASS_A;

                rc = avb_stream_create(genavb_handle, &crf->stream_handle, &crf->stream_params, &crf->cur_batch_size, 0);
                if (rc != GENAVB_SUCCESS) {
                    crf->stream_params.stream_class = SR_CLASS_D;

                    rc = avb_stream_create(genavb_handle, &crf->stream_handle, &crf->stream_params, &crf->cur_batch_size, 0);
                    if (rc != GENAVB_SUCCESS) {
                        log_err("create CRF stream failed, err %d", rc);
                        goto err_genavb;
                    }
                }
            }
        }
    }

    if (role == MEDIA_CLOCK_MASTER) {
        rc = app_msrp_talker_register(&crf->stream_params);
        if (rc != GENAVB_SUCCESS) {
            log_err("app_msrp_talker_register error, rc = %d", rc);
            goto err_msrp;
        }
    } else {
        rc = app_msrp_listener_register(&crf->stream_params);
        if (rc != GENAVB_SUCCESS) {
            log_err("app_msrp_talker_register error, rc = %d", rc);
            goto err_msrp;
        }
    }

    return 0;

err_msrp:
    genavb_stream_destroy(crf->stream_handle);

err_genavb:
err_crf:
    return -1;
}

__exit int crf_stream_destroy(media_clock_role_t role)
{
    aar_crf_stream_t *crf;
    int rc;

    crf = crf_stream_get(role);
    if (!crf) {
        log_err("cannot get CRF stream");
        return -1;
    }

    rc = genavb_stream_destroy(crf->stream_handle);
    if (rc != GENAVB_SUCCESS)
        log_err("genavb_stream_destroy error, rc = %d", rc);

    if (role == MEDIA_CLOCK_MASTER)
        app_msrp_talker_deregister(&crf->stream_params);
    else
        app_msrp_listener_deregister(&crf->stream_params);

    return rc;
}
