mcux_add_macro(
    CC "\
    XIP_BOOT_HEADER_DCD_ENABLE=1 \
    SKIP_SYSCLK_INIT \
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    FSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE \
    ENET_ENHANCEDBUFFERDESCRIPTOR_MODE=1 \
")

mcux_add_source(
    SOURCES
    app_board.h
    codec_config.c
    main.c
    sai_clock_config.c
    sai_config.c
)

mcux_add_include(
    INCLUDES
    .
)
