mcux_add_source(
    SOURCES
    main.c
)

mcux_add_macro(
    TARGETS sdram_release
    CC "CONFIG_APP_SDRAM=1"
)

mcux_add_macro(
    CC "\
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=256000 \
    ENET_ENHANCEDBUFFERDESCRIPTOR_MODE=1 \
    configTOTAL_HEAP_SIZE=175000 \
")

mcux_add_armgcc_linker_script(
    TARGETS motor_release
    BASE_PATH ${app_board_root}
    LINKER demo_apps/avb_tsn/common/${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}.ld
)

mcux_add_armgcc_linker_script(
    TARGETS motor_ram_release
    BASE_PATH ${app_board_root}
    LINKER demo_apps/avb_tsn/common/${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}_ram.ld
)

mcux_add_armgcc_linker_script(
    TARGETS sdram_release
    BASE_PATH ${app_board_root}
    LINKER demo_apps/avb_tsn/common/${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}_sdram.ld
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
        mc_drivers/mcdrv_enc_qd2.c
        mc_drivers/mcdrv_enc_qd2.h
)
endif()
