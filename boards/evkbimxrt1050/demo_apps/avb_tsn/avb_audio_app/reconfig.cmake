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
    main.c
    media_stack/config.c
    media_stack/config.h
)

mcux_add_include(
    INCLUDES
    .
    media_stack/.
)
