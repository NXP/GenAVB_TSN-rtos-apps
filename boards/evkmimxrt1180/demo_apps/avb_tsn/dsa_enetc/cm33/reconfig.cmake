mcux_add_macro(
    CC "\
    DSA_CPU_PORT_ENETC \
    SPI_RETRY_TIMES=3 \
    FSL_RTOS_FREE_RTOS \
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
