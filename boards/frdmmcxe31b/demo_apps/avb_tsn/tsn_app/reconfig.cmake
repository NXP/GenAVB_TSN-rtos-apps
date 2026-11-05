mcux_add_source(
    BASE_PATH ${app_root_path}
    SOURCES
    devices/MCXE3X/common/main.c
)

mcux_add_macro(
    CC "__STARTUP_INITIALIZE_NONCACHEDATA \
        __STARTUP_INITIALIZE_QADATA \
        __STARTUP_INITIALIZE_RAMFUNCTION \
        DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
        __STARTUP_CLEAR_BSS \
        FSL_ETH_ENABLE_CACHE_CONTROL \
        configTOTAL_HEAP_SIZE=122000 \
")

mcux_add_armgcc_linker_script(
    TARGETS motor_controller_release motor_iodevice_release
    BASE_PATH ${app_root_path}
    LINKER devices/MCXE3X/common/armgcc/MCXE3x_flash.ld
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
        mc_drivers/mcdrv_enc_emios.c
        mc_drivers/mcdrv_enc_emios.h
)
endif()
