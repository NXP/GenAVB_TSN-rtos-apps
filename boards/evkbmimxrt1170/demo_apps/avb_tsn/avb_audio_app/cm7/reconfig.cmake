mcux_add_macro(
    CC "\
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
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
