mcux_add_macro(
    CC "\
    CONFIG_APP_CONTROLLER_NUM_IO_DEVICES=2 \
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
")

mcux_add_source(
    SOURCES
    genavb_sdk.h
    enetc0/genavb_sdk.h
)

mcux_add_source(
    BASE_PATH ${app_root_path}
    SOURCES
    devices/MIMXRT118x/common/${core_id}/main.c
)

mcux_add_armgcc_linker_script(
    TARGETS release_motor_controller release_motor_iodevice release_enetc0
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_flexspi_nor.ld
)

mcux_add_armgcc_linker_script(
    TARGETS ram_release_motor_controller ram_release_motor_iodevice ram_release_enetc0
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_ram.ld
)
