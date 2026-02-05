/*
 * Copyright 2018-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rtos_apps/types.h"

#include "log.h"
#include "genavb.h"
#include "storage.h"
#include "FreeRTOS.h"

#include "genavb/config.h"
#include "genavb/genavb.h"
#include "genavb/helpers.h"
#include "genavb/log.h"
#include "genavb/timer.h"

#ifdef CONFIG_APP_AVB_ENDPOINT
#include "system_config.h"
#include "aem_manager_helpers.h"
#include "aem_manager_rtos.h"

extern struct aem_desc_handler desc_handler[AEM_NUM_DESC_TYPES];

extern void listener_talker_audio_single_milan_init(struct aem_desc_hdr *aem_desc);

static struct aem_desc_hdr *aem_entity;
#endif

const unsigned int br_port_list[CONFIG_APP_BR_NUM_PORTS] = CONFIG_APP_BR_LOGICAL_PORT_LIST;

static struct genavb_handle *s_genavb_handle = NULL;

struct genavb_handle *get_genavb_handle(void)
{
    return s_genavb_handle;
}

static int timer_pps_start(struct gavb_pps *pps)
{
    uint64_t now, start_time;
    int rc;

    rc = genavb_clock_gettime64(pps->clk_id, &now);
    if (rc != GENAVB_SUCCESS) {
        log_err("genavb_clock_gettime64() error %d \n", rc);
        goto err;
    }

    /* Start time = rounded up second + 1 second */
    start_time = ((now + NSECS_PER_SEC / 2) / NSECS_PER_SEC + 1) * NSECS_PER_SEC;

    rc = genavb_timer_start(pps->t, start_time, NSECS_PER_SEC, (genavb_timer_f_t)(GENAVB_TIMERF_PPS | GENAVB_TIMERF_ABS));
    if (rc != GENAVB_SUCCESS) {
        log_err("genavb_timer_start error %d \n", rc);
        goto err;
    }

    return 0;

err:
    return -1;
}

static void timer_callback(void *data, int count)
{
    struct gavb_pps *pps = (struct gavb_pps *)data;

    /* Handle discontinuities */
    if (count < 0) {
        timer_pps_start(pps);
        log_info("discontinuity : callback_counter %d \n", count);
    }
}

int gavb_pps_init(struct gavb_pps *pps, genavb_clock_id_t clk_id)
{
    int rc;

    pps->clk_id = clk_id;

    rc = genavb_timer_create(&pps->t, pps->clk_id, (genavb_timer_f_t)(GENAVB_TIMERF_PPS | GENAVB_TIMERF_PERIODIC));
    if (rc != GENAVB_SUCCESS) {
        log_err("genavb_timer_create error %d \n", rc);
        goto err;
    }

    rc = genavb_timer_set_callback(pps->t, &timer_callback, pps);
    if (rc != GENAVB_SUCCESS) {
        log_err("genavb_timer_set_callback error %d \n", rc);
        goto err_destroy;
    }

    rc = timer_pps_start(pps);
    if (rc != 0) {
        log_err("timer_pps_start error %d \n", rc);
        goto err_destroy;
    }

    log_info("success, clk_id: %u\n", clk_id);
    return 0;

err_destroy:
    genavb_timer_destroy(pps->t);

err:
    return -1;
}

static char *domain_cfg_param_file(uint8_t instance, char *param, char *buf, size_t size)
{
    if (!instance)
        h_snprintf(buf, size, "%s", param);
    else
        h_snprintf(buf, size, "domain%u/%s", instance, param);

    return buf;
}

static char *port_cfg_param_file(uint8_t port, char *param, char *buf, size_t size)
{
    h_snprintf(buf, size, "port%u/%s", port, param);

    return buf;
}

static char *domain_port_cfg_param_file(uint8_t instance, uint8_t port, char *param, char *buf, size_t size)
{
    if (!instance)
        h_snprintf(buf, size, "%s", param);
    else
        h_snprintf(buf, size, "domain%u/port%u/%s", instance, port, param);

    return buf;
}

static int avdecc_config_init(struct avdecc_config *avdecc_config)
{
#ifdef CONFIG_APP_AVB_ENDPOINT
    struct aem_desc_hdr aem_desc[AEM_NUM_DESC_TYPES] = {0};
    int rc = 0;
#endif
    unsigned int aem_id = 0;

    avdecc_config->milan_mode = false;

    if (storage_cd("/avdecc", true) == 0) {
        int btb_mode = 0;

        uint64_t talker_entity_id = 0;

        storage_read_int("btb_mode", &btb_mode);
        if (btb_mode) {
            log_info("BTB mode\n");
            avdecc_config->entity_cfg[0].flags |= AVDECC_FAST_CONNECT_MODE | AVDECC_FAST_CONNECT_BTB;
            avdecc_config->entity_cfg[0].channel_waitmask |= AVDECC_WAITMASK_MEDIA_STACK;
        }

        if (!storage_read_u64("talker_id", &talker_entity_id)) {
            log_info("talker_entity_id 0x%016"PRIx64"\n", talker_entity_id);
            avdecc_config->entity_cfg[0].talker_entity_id[0] = talker_entity_id;
            avdecc_config->entity_cfg[0].talker_entity_id_n = 1;
            avdecc_config->entity_cfg[0].talker_unique_id[0] = 0;
            avdecc_config->entity_cfg[0].talker_unique_id_n = 1;
            avdecc_config->entity_cfg[0].listener_unique_id[0] = 0;
            avdecc_config->entity_cfg[0].listener_unique_id_n = 1;
        }

        storage_read_bool("milan_mode", &avdecc_config->milan_mode);

        storage_read_uint("aem_id", &aem_id);
    }

    if (avdecc_config->milan_mode) {
        /* Make sure to make entity startup wait for avdecc start command in Milan mode. */
        avdecc_config->entity_cfg[0].channel_waitmask = AVDECC_WAITMASK_MEDIA_STACK | AVDECC_WAITMASK_MEDIA_STACK_START;
    }

    storage_cd("/", true);

#ifdef CONFIG_APP_AVB_ENDPOINT

    if (!aem_id) {
        avdecc_config->entity_cfg[0].aem = NULL;
    } else {
        entity_desc_handler_init(desc_handler);

        if ((aem_entity_create(aem_desc, listener_talker_audio_single_milan_init) < 0)) {
            log_err("entity generation failed\n");
            rc = -1;
            goto exit;
        }

        aem_entity = aem_entity_load_from_reference_entity(aem_desc);
        if (!aem_entity) {
            log_err("Failed to load aem(%d)\n", aem_id);
            rc = -1;
            goto exit;
        }
        avdecc_config->entity_cfg[0].aem = aem_entity;
    }

exit:
    return rc;
#else
    avdecc_config->entity_cfg[0].aem = NULL;
    return 0;
#endif
}

int gavb_stack_init(void)
{
    unsigned int endpoint_logical_port_list[CONFIG_APP_EP_NUM_PORTS] = CONFIG_APP_EP_LOGICAL_PORT_LIST;
    unsigned int bridge_logical_port_list[CONFIG_APP_BR_NUM_PORTS] = CONFIG_APP_BR_LOGICAL_PORT_LIST;
#if CONFIG_APP_BR_NUM_PORTS > 1
    unsigned int bridge_port_max = CONFIG_APP_BR_NUM_PORTS - 1; /* assume last port is host port */
#else
    unsigned int bridge_port_max = CONFIG_APP_BR_NUM_PORTS;
#endif
    unsigned int endpoint_port_max = CONFIG_APP_EP_NUM_PORTS;
    struct genavb_config *genavb_config;
    unsigned int logical_port_id;
    char buf[128];
    int i, j;
    int rc = 0;

    genavb_config = pvPortMalloc(sizeof(struct genavb_config));

    if (!genavb_config) {
        rc = -1;
        goto exit;
    }

    if (s_genavb_handle) {
        rc = 0;
        goto exit;
    }

    genavb_get_default_config(genavb_config);

#ifdef CONFIG_APP_TSN_BRIDGE
    genavb_config->management_config.is_bridge = 1;
    genavb_config->fgptp_config.is_bridge = 1;
    genavb_config->srp_config.is_bridge = 1;
#endif /* CONFIG_APP_TSN_BRIDGE */

    if (storage_cd("/management", true) == 0) {
        storage_read_uint("is_bridge", &genavb_config->management_config.is_bridge);
    }

    if (genavb_config->fgptp_config.is_bridge) {
        genavb_config->fgptp_config.port_max = bridge_port_max;
        memcpy(genavb_config->fgptp_config.logical_port_list, bridge_logical_port_list, bridge_port_max * sizeof(unsigned int));
    } else {
        genavb_config->fgptp_config.port_max = endpoint_port_max;
        memcpy(genavb_config->fgptp_config.logical_port_list, endpoint_logical_port_list, endpoint_port_max * sizeof(unsigned int));
    }

    if (storage_cd("/fgptp", true) == 0) {

        storage_read_uint("is_bridge", &genavb_config->fgptp_config.is_bridge);

        /* Read general parameters */
        storage_read_uint("force_2011", &genavb_config->fgptp_config.force_2011);
        storage_read_u64("neighborPropDelayThreshold", &genavb_config->fgptp_config.neighborPropDelayThreshold);

        /* Read per-domain parameters */
        for (i = 0; i < CFG_MAX_GPTP_DOMAINS; i++) {

            if (i != 0)
                storage_read_int(domain_cfg_param_file(i, "domain_number", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].domain_number);

            if (genavb_config->fgptp_config.is_bridge == 1) {
                genavb_config->fgptp_config.domain_cfg[i].gmCapable = 1;
                genavb_config->fgptp_config.domain_cfg[i].priority1 = 246;
            }

            storage_read_u8(domain_cfg_param_file(i, "gmCapable", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].gmCapable);
            storage_read_u8(domain_cfg_param_file(i, "priority1", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].priority1);
            storage_read_u8(domain_cfg_param_file(i, "priority2", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].priority2);
            storage_read_u8(domain_cfg_param_file(i, "clockClass", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].clockClass);
            storage_read_u8(domain_cfg_param_file(i, "clockAccuracy", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].clockAccuracy);
            storage_read_u16(domain_cfg_param_file(i, "offsetScaledLogVariance", buf, sizeof(buf)), &genavb_config->fgptp_config.domain_cfg[i].offsetScaledLogVariance);

            /* Read per-port parameters */
            for (j = 0; j < genavb_config->fgptp_config.port_max; j++) {
                logical_port_id = genavb_config->fgptp_config.logical_port_list[j];
                if (i == 0) {
                    /* Below parameters are only needed for domain 0 */

                    storage_read_int(port_cfg_param_file(logical_port_id, "rxDelayCompensation", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].rxDelayCompensation);
                    storage_read_int(port_cfg_param_file(logical_port_id, "txDelayCompensation", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].txDelayCompensation);
                    storage_read_s8(port_cfg_param_file(logical_port_id, "initialLogPdelayReqInterval", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].initialLogPdelayReqInterval);
                    storage_read_s8(port_cfg_param_file(logical_port_id, "initialLogSyncInterval", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].initialLogSyncInterval);
                    storage_read_s8(port_cfg_param_file(logical_port_id, "initialLogAnnounceInterval", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].initialLogAnnounceInterval);
                    storage_read_s8(port_cfg_param_file(logical_port_id, "operLogPdelayReqInterval", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].operLogPdelayReqInterval);
                    storage_read_s8(port_cfg_param_file(logical_port_id, "operLogSyncInterval", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].operLogSyncInterval);
                    storage_read_u8(port_cfg_param_file(logical_port_id, "allowedLostResponses", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].allowedLostResponses);
                    storage_read_u8(port_cfg_param_file(logical_port_id, "delayMechanism", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].delayMechanism[i]);
                } else {
                    storage_read_u8(domain_port_cfg_param_file(i, logical_port_id, "delayMechanism", buf, sizeof(buf)), &genavb_config->fgptp_config.port_cfg[j].delayMechanism[i]);
                }
            }
        }
    } else {
        if (genavb_config->fgptp_config.is_bridge == 1) {
            for (i = 0; i < CFG_MAX_GPTP_DOMAINS; i++) {
                genavb_config->fgptp_config.domain_cfg[i].gmCapable = 1;
                genavb_config->fgptp_config.domain_cfg[i].priority1 = 246;
            }
        }
    }

    if (storage_cd("/srp", true) == 0) {
        storage_read_uint("is_bridge", &genavb_config->srp_config.is_bridge);
    }

    if (storage_cd("/hsr", true) == 0) {
        storage_read_bool("hsr_enabled", &genavb_config->hsr_config.hsr_enabled);
        for (i = 0; i < CONFIG_APP_LOGICAL_PORTS; i++) {
            uint8_t _hsr_port_type = 0;

            storage_read_u8(port_cfg_param_file(i, "type", buf, sizeof(buf)), &_hsr_port_type);

            genavb_config->hsr_config.hsr_port[i].type = (hsr_port_type)_hsr_port_type;
            genavb_config->hsr_config.hsr_port[i].logical_port = i;
        }
        genavb_config->hsr_config.port_max = i;
    }

    if (avdecc_config_init(&genavb_config->avdecc_config) < 0) {
        log_err("avdecc_config_init() failed");
        rc = -1;
        goto exit;
    }

    if (genavb_config->management_config.is_bridge) {
        genavb_config->management_config.port_max = bridge_port_max;
        memcpy(genavb_config->management_config.logical_port_list, bridge_logical_port_list, bridge_port_max * sizeof(unsigned int));
    } else {
        genavb_config->management_config.port_max = endpoint_port_max;
        memcpy(genavb_config->management_config.logical_port_list, endpoint_logical_port_list, endpoint_port_max * sizeof(unsigned int));
    }

    if (genavb_config->srp_config.is_bridge) {
        genavb_config->srp_config.port_max = bridge_port_max;
        memcpy(genavb_config->srp_config.logical_port_list, bridge_logical_port_list, bridge_port_max * sizeof(unsigned int));
    } else {
        genavb_config->srp_config.port_max = endpoint_port_max;
        memcpy(genavb_config->srp_config.logical_port_list, endpoint_logical_port_list, endpoint_port_max * sizeof(unsigned int));
    }

    genavb_config->srp_config.mvrp_cfg.is_bridge = genavb_config->srp_config.is_bridge;

    if (genavb_config->srp_config.mvrp_cfg.is_bridge) {
        genavb_config->srp_config.mvrp_cfg.port_max = bridge_port_max;
        memcpy(genavb_config->srp_config.mvrp_cfg.logical_port_list, bridge_logical_port_list, bridge_port_max * sizeof(unsigned int));
    } else {
        genavb_config->srp_config.mvrp_cfg.port_max = endpoint_port_max;
        memcpy(genavb_config->srp_config.mvrp_cfg.logical_port_list, endpoint_logical_port_list, endpoint_port_max * sizeof(unsigned int));
    }

    genavb_config->srp_config.msrp_cfg.is_bridge = genavb_config->srp_config.is_bridge;

    if (genavb_config->srp_config.msrp_cfg.is_bridge) {
        genavb_config->srp_config.msrp_cfg.port_max = bridge_port_max;
        memcpy(genavb_config->srp_config.msrp_cfg.logical_port_list, bridge_logical_port_list, bridge_port_max * sizeof(unsigned int));
    } else {
        genavb_config->srp_config.msrp_cfg.port_max = endpoint_port_max;
        memcpy(genavb_config->srp_config.msrp_cfg.logical_port_list, endpoint_logical_port_list, endpoint_port_max * sizeof(unsigned int));
    }

    genavb_set_config(genavb_config);

    if ((rc = genavb_init(&s_genavb_handle, 0)) != GENAVB_SUCCESS) {
        s_genavb_handle = NULL;
        log_err("genavb_init() failed: %s\n", genavb_strerror(rc));
        rc = -1;
        goto exit;
    }

exit:
    if (genavb_config)
        vPortFree(genavb_config);

    return rc;
}

int gavb_stack_exit(void)
{
    genavb_exit(s_genavb_handle);

    s_genavb_handle = NULL;

    return 0;
}

int gavb_log_level(char *component_str, char *level_str)
{
    genavb_log_component_id_t component;
    genavb_log_level_t level;
    int all = 0;

    if (!strcmp(level_str, "crit"))
        level = GENAVB_LOG_LEVEL_CRIT;
    else if (!strcmp(level_str, "err"))
        level = GENAVB_LOG_LEVEL_ERR;
    else if (!strcmp(level_str, "init"))
        level = GENAVB_LOG_LEVEL_INIT;
    else if (!strcmp(level_str, "info"))
        level = GENAVB_LOG_LEVEL_INFO;
    else if (!strcmp(level_str, "dbg"))
        level = GENAVB_LOG_LEVEL_DEBUG;
    else
        return -1;

    if (!strcmp(component_str, "avtp"))
        component = GENAVB_LOG_COMPONENT_ID_AVTP;
    else if (!strcmp(component_str, "avdecc"))
        component = GENAVB_LOG_COMPONENT_ID_AVDECC;
    else if (!strcmp(component_str, "srp"))
        component = GENAVB_LOG_COMPONENT_ID_SRP;
    else if (!strcmp(component_str, "maap"))
        component = GENAVB_LOG_COMPONENT_ID_MAAP;
    else if (!strcmp(component_str, "common"))
        component = GENAVB_LOG_COMPONENT_ID_COMMON;
    else if (!strcmp(component_str, "os"))
        component = GENAVB_LOG_COMPONENT_ID_OS;
    else if (!strcmp(component_str, "fgptp"))
        component = GENAVB_LOG_COMPONENT_ID_GPTP;
    else if (!strcmp(component_str, "api"))
        component = GENAVB_LOG_COMPONENT_ID_API;
    else if (!strcmp(component_str, "mgmt"))
        component = GENAVB_LOG_COMPONENT_ID_MGMT;
    else if (!strcmp(component_str, "all"))
        all = 1;
    else
        return -1;

    if (all) {
        for (component = GENAVB_LOG_COMPONENT_ID_AVTP; component <= GENAVB_LOG_COMPONENT_ID_MGMT; component++)
            genavb_log_level_set(component, level);
    } else {
        genavb_log_level_set(component, level);
    }

    return 0;
}
