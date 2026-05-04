mcux_add_macro(
    CC "\
    XIP_EXTERNAL_FLASH=1 \
    XIP_BOOT_HEADER_ENABLE=1 \
    PRINTF_ADVANCED_ENABLE=1 \
    PRINTF_FLOAT_ENABLE=1 \
    DEBUG_CONSOLE_PRINTF_MAX_LOG_LEN=128 \
    DEBUG_CONSOLE_RX_ENABLE=0 \
    DEBUG_CONSOLE_TX_RELIABLE_ENABLE=0 \
    HAL_UART_TRANSFER_MODE=1 \
    SERIAL_MANAGER_WRITE_TIME_DELAY_DEFAULT_VALUE=10 \
    SERIAL_MANAGER_READ_TIME_DELAY_DEFAULT_VALUE=10 \
    SHELL_NON_BLOCKING_MODE=0 \
    SHELL_BUFFER_SIZE=256 \
    SHELL_MAX_ARGS=20 \
    LFS_NO_ASSERT \
    LFS_NO_ERROR \
    LFS_NO_WARN \
    LFS_NO_DEBUG \
    USE_RTOS=1 \
")

mcux_add_configuration(
    TARGETS ram_release ram_debug ram_release_hybrid ram_release_motor
    ram_release_motor_controller ram_release_motor_iodevice ram_release_no_enetc0
    ram_release_enetc0
    TOOLCHAINS armgcc iar mcux
    AS "-D__RAM_BUILD"
)

mcux_add_configuration(
    TARGETS hyperram_release
    TOOLCHAINS armgcc iar mcux
    AS "-D__HYPERRAM_BUILD"
)

mcux_add_configuration(
    TARGETS sdram_release
    TOOLCHAINS armgcc iar mcux
    AS "-D__SDRAM_BUILD"
)

mcux_remove_configuration(
    TOOLCHAINS armgcc mcux
    CC "-std=gnu99"
)

mcux_add_configuration(
    TOOLCHAINS armgcc mcux
    AS "-g"
    CC "\
    -g \
    -std=c99 \
    -Os \
    -Werror \
    -Wall \
    -Wpointer-arith \
")

mcux_add_armgcc_configuration(
    LD "\
    -Xlinker \
    -z \
    -Xlinker \
    muldefs \
    -Werror \
    -u _printf_float \
    -Xlinker -Map=${MCUX_SDK_PROJECT_NAME}.map \
")

mcux_add_iar_configuration(
    CC "\
    --debug \
    --dlib_config full \
    --vla \
    -Ohz \
    --char_is_signed \
")

# Temporary fallback to let IAR project compile with warnings due to changes on the SDK
mcux_remove_iar_configuration(
  CC "--warnings_are_errors"
  CX "--warnings_are_errors"
)

