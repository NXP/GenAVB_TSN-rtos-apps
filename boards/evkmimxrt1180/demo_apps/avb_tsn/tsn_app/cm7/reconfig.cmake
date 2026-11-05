mcux_add_macro(
    CC "\
    CONFIG_APP_CONTROLLER_NUM_IO_DEVICES=2 \
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
    configTOTAL_HEAP_SIZE=180000 \
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
    TARGETS motor_controller_release motor_iodevice_release enetc0_release
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_flexspi_nor.ld
)

mcux_add_armgcc_linker_script(
    TARGETS motor_controller_ram_release motor_iodevice_ram_release enetc0_ram_release
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_ram.ld
)

if (CONFIG_MCUX_PRJSEG_config.board.app_motor_iodevice)
    # Remove local files conflicting with motor control middleware
    mcux_project_remove_source(
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/pmsm/pmsm_float
        SOURCES 
        mc_state_machine/m1_sm_snsless_enc.c
        mc_state_machine/m1_sm_snsless_enc.h
        mc_algorithms/pmsm_control.c
        mc_algorithms/pmsm_control.h
        mc_state_machine/sm_common.h
        mc_drivers/mcdrv_enc_eqd2.c
        mc_drivers/mcdrv_enc_eqd2.h
)
endif()
