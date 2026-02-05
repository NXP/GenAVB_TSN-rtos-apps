/*
 * Copyright 2018-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "genavb/genavb.h"
#include "genavb/crf.h"
#include "avb_stream_config.h"
#include "crf_stream.h"

#define AAR_AVB_ETH_PORT 0

#define AAF_DEFAULT_FORMAT                          \
    {                                               \
        .v = 0,                                     \
        .subtype = AVTP_SUBTYPE_AAF,                \
        .subtype_u.aaf = {                          \
            .nsr = AAF_NSR_48000,                   \
            .ut = 0,                                \
            .rsvd = 0,                              \
            .format = AAF_FORMAT_INT_32BIT,         \
            .format_u.pcm = {                       \
                .bit_depth = 24,                    \
                AAF_PCM_CHANNELS_PER_FRAME_INIT(2), \
                AAF_PCM_SAMPLES_PER_FRAME_INIT(24), \
                .reserved_msb = 0,                  \
                .reserved_lsb = 0,                  \
            }                                       \
        }                                           \
    }

#define CRF_DEFAULT_FORMAT                    \
    {                                         \
        .v = 0,                               \
        .subtype = AVTP_SUBTYPE_CRF,          \
        .subtype_u.crf = {                    \
            .type = CRF_TYPE_AUDIO_SAMPLE,    \
            CRF_TIMESTAMP_INTERVAL_INIT(320), \
            .timestamps_per_pdu = 6,          \
            .pull = CRF_PULL_1_1,             \
            CRF_BASE_FREQUENCY_INIT(96000),   \
        }                                     \
    }

struct avb_stream avb_listener_streams[MAX_AVB_LISTENER_STREAMS] = {
    [0] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_LISTENER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
    [1] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_LISTENER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
    [2] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_LISTENER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
    [3] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_LISTENER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
};

struct avb_stream avb_talker_streams[MAX_AVB_TALKER_STREAMS] = {
    [0] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_TALKER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
    [1] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_TALKER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
    [2] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_TALKER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
    [3] = {
        .stream_params = {
            .direction = AVTP_DIRECTION_TALKER,
            .subtype = AVTP_SUBTYPE_AAF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = AAF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00},
            .dst_mac = {0x00},
        },

        .stream_handle = NULL,

        .batch_size_ns = DEFAULT_BATCH_SIZE_NS,
    },
};

aar_crf_stream_t g_crf_streams[] = {
    {
        .stream_params = {
            .direction = AVTP_DIRECTION_TALKER,
            .subtype = AVTP_SUBTYPE_CRF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = 0,
            .format.u.s = CRF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
            .dst_mac = {0x91, 0xe0, 0xf0, 0x00, 0xfe, 0xff},
            .talker = {
                .latency = 0,
            },
        },

        .stream_handle = NULL,
        .batch_size_ns = 0,
    },
    {
        .stream_params = {
            .direction = AVTP_DIRECTION_LISTENER,
            .subtype = AVTP_SUBTYPE_CRF,
            .stream_class = SR_CLASS_B,
            .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            .flags = GENAVB_STREAM_FLAGS_MCR,
            .format.u.s = CRF_DEFAULT_FORMAT,
            .port = AAR_AVB_ETH_PORT,
            .stream_id = {0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
            .dst_mac = {0x91, 0xe0, 0xf0, 0x00, 0xfe, 0xff},
            .talker = {
                .latency = 0,
            },
        },

        .stream_handle = NULL,
        .batch_size_ns = 0,
    },
};
