if (CONFIG_MCUX_PRJSEG_config.board.audio_app)
    mcux_add_macro(
        CC "\
        CONFIG_HAS_CORTEX_M=1 \
        AUDIO_PIPELINE_MAX_STAGES=3 \
        AUDIO_PIPELINE_MAX_ELEMENTS=6 \
        AUDIO_PIPELINE_MAX_BUFFERS=32 \
        CONFIG_APP_AVB_ENDPOINT=1 \
    ")

    mcux_add_include(
        INCLUDES
        .
    )

    mcux_add_source(
        SOURCES
        audio_app.c
        audio_app.h
        audio_shell.c
        audio_shell.h
        configs.c
        pipeline_config.c
    )
endif()

include(${app_board_prj_path}/mcux_lib_variable.cmake OPTIONAL)
