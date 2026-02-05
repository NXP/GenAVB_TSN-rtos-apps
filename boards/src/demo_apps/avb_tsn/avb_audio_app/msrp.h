/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 * Copyright 2018-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 @file msrp.h
 @brief      This file defines AVB wraper layer interfaces.
 @details    Copyright 2016 Freescale Semiconductor, Inc.
*/

#ifndef __MSRP_H__
#define __MSRP_H__

/**
 * @addtogroup avb_stream
 * @{
 */

/**
 * @brief      Initialize AVB stack and AVB resources.
 *
 * @return     0 if success or negative error code.
 */
int app_msrp_init(struct genavb_handle *s_avb_handle);

/**
 * @brief      De-initialize AVB stack and AVB resources.
 *
 * @return     0 if success or negative error code.
 */
int app_msrp_exit(void);

int app_msrp_listener_register(struct genavb_stream_params *stream_params);
void app_msrp_listener_deregister(struct genavb_stream_params *stream_params);
int app_msrp_talker_register(struct genavb_stream_params *stream_params);
void app_msrp_talker_deregister(struct genavb_stream_params *stream_params);

/** @} */
#endif /* __AVB_STREAM_H__ */
