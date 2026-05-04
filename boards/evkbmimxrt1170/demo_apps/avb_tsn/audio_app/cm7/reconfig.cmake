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

mcux_add_macro(
    CC "\
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    ENET_ENHANCEDBUFFERDESCRIPTOR_MODE=1 \
")
