/*
 * Copyright 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

#include "fsl_debug_console.h"

#include "audio_app.h"
#include "audio_shell.h"
#include "avb_tsn/common/common.h"

#include "genavb/helpers.h"

#include "rtos_apps/audio/audio_pipeline_ctrl.h"
#include "rtos_apps/audio/audio_ctrl.h"

#define COMMAND_TIMEOUT     500   /* 500ms */
#define MAC_ADDRESS_DEFAULT {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}

/* The specific commands for audio application*/
static shell_status_t audio(shell_handle_t shell, int32_t argc, char **argv);
static shell_status_t audio_routing (shell_handle_t shell, int32_t argc, char **argv);

SHELL_COMMAND_DEFINE(audio,
                     "\nAudio options:\n"
                     "\t-f <frequency> audio clock frequency (in Hz)\n"
                     "\t               imxrt1170: supporting 48000, 96000 Hz\n"
                     "\t               Will use default frequency 48000Hz if not specified\n"
                     "\t-p <frames>    audio processing period (in frames)\n"
                     "\t               Supporting 2, 4 frames\n"
                     "\t               Will use default period 2 frames if not specified\n"
                     "\t-r <id>        run audio mode id:\n"
                     "\t               0 - full pipeline\n"
                     "\t-s             stop running audio mode\n",
                     &audio,
                     SHELL_IGNORE_PARAMETER_COUNT);

SHELL_COMMAND_DEFINE(audio_routing,
                     "\nRouting audio element options:\n"
                     "\t-a <pipeline_id>  audio pipeline id (default 0)\n"
                     "\t-c                connect routing input/output\n"
                     "\t-d                disconnect routing input/output\n"
                     "\t-e <element_id>   routing element id (default 0)\n"
                     "\t-i <input_id>     routing element input\n"
                     "\t-o <output_id>    routing element output\n",
                     &audio_routing,
                     SHELL_IGNORE_PARAMETER_COUNT);

void help_config_app(shell_handle_t shell)
{
    shell_printf(shell, "audio app config\n");
    shell_printf(shell, "path: /audio_app\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    None\n");
}

static int audio_start(shell_handle_t shell, unsigned int id, unsigned int frequency, unsigned int period, uint8_t *hw_addr)
{
    struct audio_app_cmd cmd;
    struct audio_app_resp resp;
    unsigned int len;
    int ret = 0;

#if defined(DEBUG_CONSOLE_RUNTIME_CONTROL)
    DbgConsole_SetState(true);
#endif

    cmd.cmd.u.audio_run.type = AUDIO_CMD_TYPE_RUN;
    cmd.cmd.u.audio_run.id = id;
    cmd.cmd.u.audio_run.frequency = frequency;
    cmd.cmd.u.audio_run.period = period;
    cmd.cmd.u.audio_run.config_idx = 0;
    memcpy(cmd.cmd.u.audio_run.addr, hw_addr, sizeof(*hw_addr));

    len = sizeof(cmd.cmd.u.audio_run);
    cmd.len = len;

    if (rtos_mqueue_send(&audio_ctx.queue_cmd, &cmd, RTOS_NO_WAIT) < 0) {
        shell_printf(shell, "send() failed\n");
        ret = -1;
        goto out;
    }

    if (rtos_mqueue_receive(&audio_ctx.queue_resp, &resp, COMMAND_TIMEOUT) < 0) {
        shell_printf(shell, "receive() failed\n");
        ret = -1;
        goto out;
    }
    else {
        if (resp.resp.u.audio.status == AUDIO_RESP_STATUS_SUCCESS)
            shell_printf(shell, "command success (%d)\n", resp.resp.u.audio.status);
        else
            shell_printf(shell, "command fail (%d)\n", resp.resp.u.audio.status);
    }

out:
#if defined(DEBUG_CONSOLE_RUNTIME_CONTROL)
    DbgConsole_SetState(false);
#endif

    return ret;
}

static int audio_stop(shell_handle_t shell)
{
    struct audio_app_cmd cmd;
    struct audio_app_resp resp;
    unsigned int len;
    int ret = 0;

    cmd.cmd.u.audio_stop.type = AUDIO_CMD_TYPE_STOP;

    len = sizeof(cmd.cmd.u.audio_stop);
    cmd.len = len;

    if (rtos_mqueue_send(&audio_ctx.queue_cmd, &cmd, RTOS_NO_WAIT) < 0) {
        shell_printf(shell, "send() failed\n");
        ret = -1;
        goto out;
    }

    if (rtos_mqueue_receive(&audio_ctx.queue_resp, &resp, COMMAND_TIMEOUT) < 0) {
        shell_printf(shell, "receive() failed\n");
        ret = -1;
    }
    else {
        if (resp.resp.u.audio.status == AUDIO_RESP_STATUS_SUCCESS)
            shell_printf(shell, "command success (%d)\n", resp.resp.u.audio.status);
        else
            shell_printf(shell, "command fail (%d)\n", resp.resp.u.audio.status);
    }
out:
    return ret;
}

static shell_status_t audio(shell_handle_t shell, int32_t argc, char **argv)
{
    uint8_t mac_addr[6] = MAC_ADDRESS_DEFAULT;
    unsigned int frequency = 48000;
    unsigned int period = 2;
    bool is_run_cmd = false;
    unsigned int id = 0;
    unsigned long tmp;
    int option;
    int rc = 0;

    optind = 1;

    while ((option = getopt(argc, argv, "f:p:r:s")) != -1) {
        switch (option) {
        case 'f':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell, "Invalid frequency\n");
                rc = -1;
                goto out;
            }
            frequency = tmp;

            break;

        case 'p':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell, "Invalid period\n");
                rc = -1;
                goto out;
            }
            period = tmp;


            break;

        case 'r':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell, "Invalid pipeline id\n");
                rc = -1;
                goto out;
            }
            id = tmp;
            is_run_cmd = true;

            break;

        case 's':
            audio_stop(shell);

            break;

        default:
            shell_printf(shell, "You did not specify an option\n");
            break;
        }
    }

    /* Run the case after we get all parameters */
    if (is_run_cmd)
        rc = audio_start(shell, id, frequency, period, mac_addr);

out:
    if (rc)
        return kStatus_SHELL_Error;
    else
        return kStatus_SHELL_Success;
}

static int audio_routing_connect(shell_handle_t shell, unsigned int pipeline_id, unsigned int element_id, unsigned int output, unsigned int input)
{
    struct audio_app_cmd cmd;
    struct audio_app_resp resp;
    unsigned int len;
    int ret = 0;

    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect.type = AUDIO_CMD_TYPE_ELEMENT_ROUTING_CONNECT;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect.pipeline.id = pipeline_id;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect.element.type = 1;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect.element.id = element_id;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect.output = output;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect.input = input;

    len = sizeof(cmd.cmd.u.audio_pipeline.u.element.u.routing.u.connect);
    cmd.len = len;

    if (rtos_mqueue_send(&audio_ctx.queue_cmd, &cmd, RTOS_NO_WAIT) < 0) {
        shell_printf(shell, "send() failed\n");
        ret = -1;
        goto out;
    }

    if (rtos_mqueue_receive(&audio_ctx.queue_resp, &resp, COMMAND_TIMEOUT) < 0) {
        shell_printf(shell, "receive() failed\n");
        ret = -1;
    }
    else {
        if (resp.resp.u.routing.status == AUDIO_RESP_STATUS_SUCCESS)
            shell_printf(shell, "command success (%d)\n", resp.resp.u.routing.status);
        else
            shell_printf(shell, "command fail (%d)\n", resp.resp.u.routing.status);
    }
out:
    return ret;
}

static int audio_routing_disconnect(shell_handle_t shell, unsigned int pipeline_id, unsigned int element_id, unsigned int output)
{
    struct audio_app_cmd cmd;
    struct audio_app_resp resp;
    unsigned int len;
    int ret = 0;

    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.disconnect.type = AUDIO_CMD_TYPE_ELEMENT_ROUTING_DISCONNECT;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.disconnect.pipeline.id = pipeline_id;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.disconnect.element.type = 1;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.disconnect.element.id = element_id;
    cmd.cmd.u.audio_pipeline.u.element.u.routing.u.disconnect.output = output;

    len = sizeof(cmd.cmd.u.audio_pipeline.u.element.u.routing.u.disconnect);
    cmd.len = len;

    if (rtos_mqueue_send(&audio_ctx.queue_cmd, &cmd, RTOS_NO_WAIT) < 0) {
        shell_printf(shell, "send() failed\n");
        ret = -1;
        goto out;
    }

    if (rtos_mqueue_receive(&audio_ctx.queue_resp, &resp, COMMAND_TIMEOUT) < 0) {
        shell_printf(shell, "receive() failed\n");
        ret = -1;
        goto out;
    }
    else {
        if (resp.resp.u.routing.status == AUDIO_RESP_STATUS_SUCCESS)
            shell_printf(shell, "command success (%d)\n", resp.resp.u.routing.status);
        else
            shell_printf(shell, "command fail (%d)\n", resp.resp.u.routing.status);
    }

out:
    return ret;
}

static shell_status_t audio_routing(shell_handle_t shell, int32_t argc, char **argv)
{
    bool is_routing_disconnect = false;
    bool is_routing_connect = false;
    unsigned int pipeline_id = 0;
    unsigned int element_id = 0;
    unsigned int output = 0;
    unsigned int input = 0;
    unsigned long tmp;
    int option;
    int rc = 0;
 
    optind = 1;

    while ((option = getopt(argc, argv, "a:cde:i:o:v")) != -1) {
        switch (option) {
        case 'a':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell,"Invalid pipeline id\n");
                rc = -1;
                goto out;
            }
            pipeline_id = tmp;

            break;

        case 'c':
            is_routing_connect = true;

            break;

        case 'd':
            is_routing_disconnect = true;

            break;

        case 'e':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell,"Invalid element id\n");
                rc = -1;
                goto out;
            }
            element_id = tmp;

            break;

        case 'i':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell,"Invalid element input\n");
                rc = -1;
                goto out;
            }
            input = tmp;

            break;

        case 'o':
            if ((h_strtoul(&tmp, optarg, NULL, 0)) < 0) {
                shell_printf(shell,"Invalid element output\n");
                rc = -1;
                goto out;
            }
            output = tmp;

            break;

        default:
            shell_printf(shell, "You did not specify an option\n", option);
            break;
        }
    }

    if (is_routing_disconnect)
        rc = audio_routing_disconnect(shell, pipeline_id, element_id, output);
    else if (is_routing_connect)
        rc = audio_routing_connect(shell, pipeline_id, element_id, output, input);

out:
    if (rc)
        return kStatus_SHELL_Error;
    else
        return kStatus_SHELL_Success;
}

void audio_init_shell(shell_handle_t shell)
{
    SHELL_RegisterCommand(shell, SHELL_COMMAND(audio));
    SHELL_RegisterCommand(shell, SHELL_COMMAND(audio_routing));
}
