/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2017-2020, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CLOCK_DOMAIN_H_
#define _CLOCK_DOMAIN_H_

#include <genavb/genavb.h>

typedef enum {
    MEDIA_CLOCK_MASTER,
    MEDIA_CLOCK_SLAVE
} media_clock_role_t;

int get_clk_domain_validity(genavb_clock_domain_t clk_domain);

int handle_clock_domain_event(void);

void clock_domain_get_status(genavb_clock_domain_t domain);

int get_audio_clk_sync(genavb_clock_domain_t clk_domain);

void set_audio_clk_sync(genavb_clock_domain_t clk_domain, int clk_sync);

int clock_domain_set_source_stream(genavb_clock_domain_t domain, struct genavb_stream_params *stream_params);

int clock_domain_set_source_internal(genavb_clock_domain_t domain, genavb_clock_source_local_id_t local_id);

int clock_domain_set_role(media_clock_role_t role, genavb_clock_domain_t domain, struct genavb_stream_params *stream_params);

int app_clock_domain_init(struct genavb_handle *s_genavb_handle);

int app_clock_domain_exit(void);

#endif /* _CLOCK_DOMAIN_H_ */
