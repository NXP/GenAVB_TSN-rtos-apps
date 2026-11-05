mcux_add_macro(
    CC "\
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
    configTOTAL_HEAP_SIZE=217000 \
")

mcux_add_armgcc_configuration(
    LD "\
    -Xlinker --defsym \
    -Xlinker BOARD_FLASH_START_ADDR=0x04000000 \
")

mcux_add_source(
    SOURCES
    genavb_sdk.h
)

mcux_add_source(
    BASE_PATH ${app_root_path}
    SOURCES
    devices/MIMXRT118x/common/${core_id}/main.c
)

mcux_add_armgcc_linker_script(
    TARGETS motor_controller_release motor_iodevice_release
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_flexspi_nor.ld
)

mcux_add_armgcc_linker_script(
    TARGETS motor_controller_ram_release motor_iodevice_ram_release
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_ram.ld
)
