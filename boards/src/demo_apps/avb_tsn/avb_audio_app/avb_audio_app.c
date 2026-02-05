/*
 * Copyright 2018-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "avb_tsn/common/common.h"
#include "avb_tsn/common/genavb.h"
#include "avb_tsn/common/log.h"
#include "lwip.h"
#include "lwip_iperf.h"
#include "avb_tsn/common/shell.h"
#include "avb_tsn/common/stats_task.h"
#include "avb_tsn/common/storage.h"
#include "avb_tsn/common/system_config.h"

#include "clock_domain.h"
#include "msrp.h"
#include "crf_stream.h"
#include "audio_stream.h"
#include "task_management.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CONTROL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 350)
#define CONTROL_TASK_PRIORITY   (configMAX_PRIORITIES - 10)

#define CONTROL_EVENT_QUEUE_LENGTH 8

#define MAX_LISTENERS       4
#define MAX_AUDIO_LISTENERS 4
#if MAX_LISTENERS > MAX_AUDIO_LISTENERS
#error
#endif

#define MAX_TALKERS       4
#define MAX_AUDIO_TALKERS 4
#if MAX_TALKERS > MAX_AUDIO_TALKERS
#error
#endif

#define STATS_PERIOD_MS 10000

enum stream_handler_type {
    STREAM_NONE = 0,
    STREAM_AUDIO
};

struct media_generic_stream {
    unsigned long long stream_id;
    enum stream_handler_type handler_type;

    struct {
        int index;
    } audio;
};

/*******************************************************************************
 * Global variables
 ******************************************************************************/

static struct media_generic_stream listener_streams[MAX_LISTENERS] = {0};
static struct media_generic_stream talker_streams[MAX_TALKERS] = {0};
static struct audio_stream audio_listener_streams[MAX_AUDIO_LISTENERS] = {0};
static struct audio_stream audio_talker_streams[MAX_AUDIO_TALKERS] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 *
 */
__init static void avdecc_streams_init(void)
{
    unsigned int i;

    for (i = 0; i < MAX_LISTENERS; i++) {
        listener_streams[i].audio.index = i;
        listener_streams[i].handler_type = STREAM_NONE;
    }

    for (i = 0; i < MAX_TALKERS; i++) {
        talker_streams[i].audio.index = i;
        talker_streams[i].handler_type = STREAM_NONE;
    }
}

/**
 *
 */
__init static void stats_init(void)
{
    unsigned int i;

    for (i = 0; i < MAX_AUDIO_LISTENERS; i++) {
        audio_listener_streams[i].is_listener = true;
    }

    for (i = 0; i < MAX_AUDIO_TALKERS; i++) {
        audio_talker_streams[i].is_listener = false;
    }
}

/**
 *
 */
static struct media_generic_stream *get_generic_stream(unsigned int stream_index, avtp_direction_t direction)
{
    if (direction == AVTP_DIRECTION_TALKER) {
        if (stream_index >= MAX_TALKERS)
            return NULL;

        return &talker_streams[stream_index];
    } else {
        if (stream_index >= MAX_LISTENERS)
            return NULL;

        return &listener_streams[stream_index];
    }
}

/**
 *
 */
static inline unsigned int is_audio_stream(const struct avdecc_format *format)
{
    return (avdecc_format_is_aaf_pcm(format) || avdecc_format_is_61883_6(format));
}

/**
 *
 */
static void talker_connect(struct genavb_msg_media_stack_connect *media_stack_connect)
{
    struct media_generic_stream *avdecc_talker;

    if ((avdecc_talker = get_generic_stream(media_stack_connect->stream_index, AVTP_DIRECTION_TALKER)) == NULL) {
        log_err("get_generic_stream() failed: stream_index = %d\n", media_stack_connect->stream_index);
        return;
    }

    if (avdecc_talker->handler_type != STREAM_NONE) {
        log_err("listener stream already used\n");
        return;
    }

    if (is_audio_stream(&media_stack_connect->stream_params.format)) {
        struct audio_stream *talker = &audio_talker_streams[avdecc_talker->audio.index];

        // audio stream index map to avdecc stream index
        talker->index = avdecc_talker->audio.index;

        if (talker_audio_connect(talker, &media_stack_connect->stream_params))
            return;

        avdecc_talker->handler_type = STREAM_AUDIO;
        log_info("Talker audio stream connected\n");

    } else {
        log_err("Stream format not supported\n");
    }
}

/**
 *
 */
static void talker_disconnect(int stream_index)
{
    struct media_generic_stream *avdecc_talker;

    if (stream_index >= MAX_LISTENERS)
        return;

    avdecc_talker = &talker_streams[stream_index];

    switch (avdecc_talker->handler_type) {
    case STREAM_AUDIO:

        if (talker_audio_disconnect(&audio_talker_streams[avdecc_talker->audio.index]))
            return;

        log_info("Talker audio stream disconnected\n");

        break;
    case STREAM_NONE:
    default:
        break;
    }

    avdecc_talker->handler_type = STREAM_NONE;
}

/**
 *
 */
static void listener_connect(struct genavb_msg_media_stack_connect *media_stack_connect)
{
    struct media_generic_stream *avdecc_listener;

    if ((avdecc_listener = get_generic_stream(media_stack_connect->stream_index, AVTP_DIRECTION_LISTENER)) == NULL) {
        log_err("get_generic_stream() failed: stream_index = %d\n", media_stack_connect->stream_index);
        return;
    }

    if (avdecc_listener->handler_type != STREAM_NONE) {
        log_err("listener stream already used\n");
        return;
    }

    if (is_audio_stream(&media_stack_connect->stream_params.format)) {
        struct audio_stream *listener = &audio_listener_streams[avdecc_listener->audio.index];

        // audio stream index map to avdecc stream index
        listener->index = avdecc_listener->audio.index;

        if (listener_audio_connect(listener, &media_stack_connect->stream_params))
            return;

        avdecc_listener->handler_type = STREAM_AUDIO;
        log_info("Listener audio stream connected\n");

    } else {
        log_err("Stream format not supported\n");
    }
}

/**
 *
 */
static void listener_disconnect(int stream_index)
{
    struct media_generic_stream *avdecc_listener;

    if (stream_index >= MAX_LISTENERS)
        return;

    avdecc_listener = &listener_streams[stream_index];

    switch (avdecc_listener->handler_type) {
    case STREAM_AUDIO:

        if (listener_audio_disconnect(&audio_listener_streams[avdecc_listener->audio.index]))
            return;

        log_info("Listener audio stream disconnected\n");

        break;
    case STREAM_NONE:
    default:
        break;
    }

    avdecc_listener->handler_type = STREAM_NONE;
}

/**
 *
 */
static int handle_avdecc_event(struct genavb_control_handle *ctrl_h)
{
    struct genavb_msg_media_stack_connect *media_stack_connect;
    struct genavb_msg_media_stack_disconnect *media_stack_disconnect;
    union genavb_media_stack_msg msg;
    genavb_msg_type_t msg_type;
    unsigned int msg_len;
    int rc;

    msg_len = sizeof(union genavb_media_stack_msg);

    if ((rc = genavb_control_receive(ctrl_h, &msg_type, &msg, &msg_len)) != GENAVB_SUCCESS) {
        log_err("genavb_control_receive() failed: %s\n", genavb_strerror(rc));
        goto error_control_receive;
    }

    switch (msg_type) {
    case GENAVB_MSG_MEDIA_STACK_CONNECT:

        media_stack_connect = &msg.media_stack_connect;

        log_info("GENAVB_MSG_MEDIA_STACK_CONNECT stream index: %u\n", media_stack_connect->stream_index);

        if (media_stack_connect->stream_params.direction == AVTP_DIRECTION_TALKER)
            talker_connect(media_stack_connect);
        else
            listener_connect(media_stack_connect);

        break;

    case GENAVB_MSG_MEDIA_STACK_DISCONNECT:

        media_stack_disconnect = &msg.media_stack_disconnect;

        log_info("GENAVB_MSG_MEDIA_STACK_DISCONNECT stream index: %u\n", media_stack_disconnect->stream_index);

        if (media_stack_disconnect->direction == AVTP_DIRECTION_TALKER)
            talker_disconnect(media_stack_disconnect->stream_index);
        else
            listener_disconnect(media_stack_disconnect->stream_index);

        break;

	case GENAVB_MSG_MEDIA_SET_CLOCK_SOURCE:

		log_info("GENAVB_MSG_MEDIA_SET_CLOCK_SOURCE: domain(%u) type(%u), not supported\n", msg.media_stack_set_clock_source.domain, msg.media_stack_set_clock_source.source_type);

		break;

    default:
        log_err("Error, unknown message type: %d\n", msg_type);
        rc = -1;
        break;
    }

error_control_receive:
    return rc;
}

/**
 *
 */
static int control_genavbstack_callback(void *data)
{
    SemaphoreHandle_t control_task_semaphore = (SemaphoreHandle_t)data;

    xSemaphoreGiveFromISR(control_task_semaphore, NULL);

    return 0;
}

__init static int app_media_clock_init(media_clock_role_t role, avb_u64 crf_stream_id)
{
    int rc;
    aar_crf_stream_t *crf;

    /* If CRF stream id is provided, update stream_params */
    if (crf_stream_id) {
        crf = crf_stream_get(role);
        if (!crf) {
            log_err("crf_stream_get failed\n");
            goto err_crf;
        }
        memcpy(&crf->stream_params.stream_id, &crf_stream_id, sizeof(crf_stream_id));
    }

    /*
     * CRF static setup without AVDECC
     */
    if (role == MEDIA_CLOCK_MASTER) {
        rc = clock_domain_set_role(role, GENAVB_CLOCK_DOMAIN_0, NULL);
        if (rc != GENAVB_SUCCESS) {
            log_err("clock_domain_set_role failed: %s\n", genavb_strerror(rc));
            goto err_crf;
        }

        rc = crf_stream_create(role);
        if (rc != GENAVB_SUCCESS) {
            log_err("crf_stream_create failed: %s\n", genavb_strerror(rc));
            goto err_clock_domain;
        }
    } else {
        rc = crf_stream_create(role);
        if (rc != GENAVB_SUCCESS) {
            log_err("crf_stream_create failed: %s\n", genavb_strerror(rc));
            goto err_crf;
        }

        crf = crf_stream_get(role);
        if (!crf) {
            log_err("crf_stream_get failed\n");
            goto err_clock_domain;
        }

        rc = clock_domain_set_role(role, GENAVB_CLOCK_DOMAIN_0, &crf->stream_params);
        if (rc != GENAVB_SUCCESS) {
            log_err("clock_domain_set_role failed: %s\n", genavb_strerror(rc));
            goto err_clock_domain;
        }
    }

    return 0;

err_clock_domain:
    crf_stream_destroy(role);
err_crf:
    return -1;
}

/**
 *
 */
static void app_stats(void *data)
{
    int i;

    app_log_update_time(GENAVB_CLOCK_GPTP_0_0);

    for (i = 0; i < MAX_LISTENERS; i++) {
        struct audio_stream *listener = &audio_listener_streams[i];

        if (listener->created)
            audio_stats_dump(listener);
    }

    for (i = 0; i < MAX_TALKERS; i++) {
        struct audio_stream *talker = &audio_talker_streams[i];

        if (talker->created)
            audio_stats_dump(talker);
    }
}

/**
 *
 */
static void control_task(void *data)
{
    struct genavb_control_handle *ctrl_h = NULL;
    struct genavb_handle *genavb_handle;
    SemaphoreHandle_t control_event_semaphore;
    int genavb_result;
    int rc;
    avb_u64 crf_stream_id = 0;
    const struct avb_app_config *app_conf;
    uint8_t sr_class_cfg[CFG_SR_CLASS_MAX] = {
        [0] = SR_CLASS_A,
        [1] = SR_CLASS_B,
    };
    shell_handle_t shell;

    if (STATS_TaskInit(&app_stats, NULL, STATS_PERIOD_MS, NULL) < 0)
        log_err("STATS_TaskInit() failed\n");

    storage_init();
    shell = shell_init("AVB");
    if (shell == NULL) {
        log_err("shell_init() failed\n");
        goto err;
    }

    shell_start(NULL);

    storage_set_shell(shell);

    app_conf = system_config_get_avb_app();
    if (!app_conf) {
        log_err("system_config_get_avb_app() failed\n");
        goto err;
    }

    //
    if ((control_event_semaphore = xSemaphoreCreateBinary()) == NULL) {
        log_err("xSemaphoreCreateBinary() failed\n");
        goto err;
    }

    if (sr_class_config(sr_class_cfg) < 0)
        goto err;

    // Init GENAVB Stack
    if (gavb_stack_init()) {
        log_err("gavb_stack_init() failed\n");
        goto err;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    genavb_handle = get_genavb_handle();

    //
    avdecc_streams_init();
    stats_init();

    // Init tasks managemant
    if (task_init()) {
        log_err("task_init failed\n");
        goto err_task;
    }

    rc = app_msrp_init(genavb_handle);
    if (rc < 0) {
        log_err("app_msrp_init failed\n");
        goto err_msrp;
    }

    rc = app_clock_domain_init(genavb_handle);
    if (rc < 0) {
        log_err("app_clock_domain_init failed\n");
        goto err_clock_domain_init;
    }

    clock_domain_get_status(GENAVB_CLOCK_DOMAIN_0);

    /*
     * Setup CRF and clock domain
     */
    rc = app_media_clock_init(app_conf->mclock_role, crf_stream_id);
    if (rc != GENAVB_SUCCESS) {
        log_err("media_clock_init failed\n");
        goto err_media_clock;
    }

    // Open control channel
    if ((genavb_result = genavb_control_open(genavb_handle, &ctrl_h, GENAVB_CTRL_AVDECC_MEDIA_STACK)) != GENAVB_SUCCESS) {
        log_err("genavb_control_open() failed: %s\n", genavb_strerror(genavb_result));
        goto err;
    }

    //
    if ((genavb_result = genavb_control_set_callback(ctrl_h, &control_genavbstack_callback, control_event_semaphore)) != GENAVB_SUCCESS) {
        log_err("genavb_control_set_callback() failed: %s\n", genavb_strerror(genavb_result));
        return;
    }

    lwip_stack_init();
    lwip_iperf_start(shell);

    // Wait for clock domain to lock
    // TODO: Use callback for clock_domain control channel
    while (!get_clk_domain_validity(GENAVB_CLOCK_DOMAIN_0)) {
        vTaskDelay(pdMS_TO_TICKS(100));
        handle_clock_domain_event();
    }

    log_info("Main loop started\n");

    // Main loop
    while (1) {
        if (xSemaphoreTake(control_event_semaphore, pdMS_TO_TICKS(10000)) != pdTRUE) {
            continue;
        }

        handle_avdecc_event(ctrl_h);

        if ((genavb_result = genavb_control_enable_callback(ctrl_h)) != GENAVB_SUCCESS)
            log_err("genavb_control_enable_callback() failed: %s\n", genavb_strerror(genavb_result));
    }

err_media_clock:
    app_clock_domain_exit();
err_clock_domain_init:
    app_msrp_exit();
err_msrp:
    task_exit();
err_task:
    vSemaphoreDelete(control_event_semaphore);
err:
    vTaskDelete(NULL);
}

/*!
 * @brief AVB audio application entry point/Main function
 */
int avb_audio_app_main(void)
{
    (void)xTaskCreate(&control_task, "genavb_app ctrl", CONTROL_TASK_STACK_SIZE, NULL, CONTROL_TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    return 0;
}
