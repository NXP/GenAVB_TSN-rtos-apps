mcux_add_macro(
    CC "\
    DSA_CPU_PORT_NETC_SWITCH \
    FSL_RTOS_FREE_RTOS \
    SPI_RETRY_TIMES=3 \
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
")

mcux_add_source(
    SOURCES
    genavb_sdk.h
)

mcux_add_include(
    INCLUDES
    .
)
