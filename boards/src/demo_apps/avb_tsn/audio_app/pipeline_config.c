/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"

#include "rtos_apps/audio/audio_app.h"
#include "rtos_apps/audio/audio_pipeline.h"

#include "audio_app.h"

const struct audio_pipeline_config pipeline_full_config = {

    .name = "Full audio pipeline",

    .stages = 3,

    .stage[0] = {

        .elements = 3,

        .element[0] = {
            .type = AUDIO_ELEMENT_DTMF_SOURCE,
            .u.dtmf = {
                .us = 120000,
                .pause_us = 100000,
                .sequence_pause_us = 500000,
                .amplitude = 0.5,
                .sequence = "1123ABCD0123456789*#",
            },

            .outputs = 1,
            .output = {0},
        },

        .element[1] = {
            .type = AUDIO_ELEMENT_SINE_SOURCE,
            .u.sine = {
                .freq = 440,
                .amplitude = 0.5,
            },

            .outputs = 1,
            .output = {1},
        },

        .element[2] = {
            .type = AUDIO_ELEMENT_SAI_SOURCE,
            .u.sai_source = {
                .sai_n = 1,
                .sai = {
                    [0] = {
                        .id = 1,
                        .line_n = 1,
                        .line = {
                            [0] = {
                                .channel_n = 2,
                            },
                        },
                    },
                },
            },

            .outputs = 2,
            .output = {2, }, /* 2 - 3 */
        },
    },

    .stage[1] = {

        .elements = 1,

        .element[0] = {
            .type = AUDIO_ELEMENT_ROUTING,
            .u.routing = {
            },

            .inputs = 4,
            .input = {0, }, /* 0 - 3 */

            .outputs = 2,
            .output = {4, }, /* 4 - 5 */
        },
    },

    .stage[2] = {

        .elements = 1,

        .element[0] = {
            .type = AUDIO_ELEMENT_SAI_SINK,
            .u.sai_sink = {
                .sai_n = 1,
                .sai = {
                    [0] = {
                        .id = 1,
                        .line_n = 1,
                        .line = {
                            [0] = {
                                .channel_n = 2,
                            },
                        },
                    },
                },
            },

            .inputs = 2,
            .input = {4, }, /* 4 - 5 */
        },
    },

    .buffers = 6,

    .buffer_storage = 6,
};

const struct audio_pipeline_config pipeline_avb_full_config = {

    .name = "Full audio pipeline (avb)",

    .avb = true,

    .stages = 3,

    .stage[0] = {

        .elements = 4,

        .element[0] = {
            .type = AUDIO_ELEMENT_DTMF_SOURCE,
            .u.dtmf = {
                .us = 120000,
                .pause_us = 100000,
                .sequence_pause_us = 500000,
                .amplitude = 0.5,
                .sequence = "1123ABCD0123456789*#",
            },

            .outputs = 1,
            .output = {0},
        },

        .element[1] = {
            .type = AUDIO_ELEMENT_SINE_SOURCE,
            .u.sine = {
                .freq = 440,
                .amplitude = 0.5,
            },

            .outputs = 1,
            .output = {1},
        },

        .element[2] = {
            .type = AUDIO_ELEMENT_SAI_SOURCE,
            .u.sai_source = {
                .sai_n = 1,
                .sai = {
                    [0] = {
                        .id = 1,
                        .line_n = 1,
                        .line = {
                            [0] = {
                                .channel_n = 2,
                            },
                        },
                    },
                },
            },

            .outputs = 2,
            .output = {2, }, /* 2 - 3 */
        },

        .element[3] = {
            .type = AUDIO_ELEMENT_AVTP_SOURCE,
            .u.avtp_source = {
                .stream_n = 2,
                .stream[0].flags = GENAVB_STREAM_FLAGS_MCR,
                .stream[1].flags = GENAVB_STREAM_FLAGS_MCR,
                .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            },

            .outputs = 4,
            .output = {4, }, /* 4 - 7 */
        },
    },

    .stage[1] = {

        .elements = 1,

        .element[0] = {
            .type = AUDIO_ELEMENT_ROUTING,
            .u.routing = {
            },

            .inputs = 8,
            .input = {0, }, /* 0 - 7 */

            .outputs = 6,
            .output = {8, }, /* 8 - 13 */
        },
    },

    .stage[2] = {

        .elements = 2,

        .element[0] = {
            .type = AUDIO_ELEMENT_SAI_SINK,
            .u.sai_sink = {
                .sai_n = 1,
                .sai = {
                    [0] = {
                        .id = 1,
                        .line_n = 1,
                        .line = {
                            [0] = {
                                .channel_n = 2,
                            },
                        },
                    },
                },
            },

            .inputs = 2,
            .input = {8, }, /* 8 - 9 */
        },

        .element[1] = {
            .type = AUDIO_ELEMENT_AVTP_SINK,
            .u.avtp_sink = {
                .stream_n = 2,
                .clock_domain = GENAVB_CLOCK_DOMAIN_0,
            },

            .inputs = 4,
            .input = {10, }, /* 10 - 13 */
        },
    },

    .buffers = 14,

    .buffer_storage = 14,

    .storage = {
        [4] = {.periods = AUDIO_PIPELINE_AVB_MAX_BUFFER_SIZE},
        [5] = {.periods = AUDIO_PIPELINE_AVB_MAX_BUFFER_SIZE},
        [6] = {.periods = AUDIO_PIPELINE_AVB_MAX_BUFFER_SIZE},
        [7] = {.periods = AUDIO_PIPELINE_AVB_MAX_BUFFER_SIZE},
    },
};

struct play_pipeline_config play_pipeline_full_config = {
    .cfg = {
        &pipeline_full_config,
    }
};

struct play_pipeline_config play_pipeline_avb_full_config = {
    .cfg = {
        &pipeline_avb_full_config,
    }
};

const struct play_pipeline_config *play_config[][AUDIO_APP_MAX_RUN_MODES] = {
    [0] = {
        [0] = &play_pipeline_full_config,
        [1] = &play_pipeline_avb_full_config,
    },
};

uint32_t max_play_configs = ARRAY_SIZE(play_config);
