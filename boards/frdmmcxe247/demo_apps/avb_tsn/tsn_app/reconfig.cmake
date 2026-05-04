mcux_add_source(
    SOURCES
    main.c
)

mcux_add_macro(
    CC "\
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
    ENET_ENHANCEDBUFFERDESCRIPTOR_MODE=1 \
")
