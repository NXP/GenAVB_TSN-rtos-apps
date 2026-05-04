mcux_add_source(
    SOURCES
    main.c
)

mcux_add_macro(
    CC "__STARTUP_INITIALIZE_NONCACHEDATA\
        __STARTUP_INITIALIZE_QADATA\
        __STARTUP_INITIALIZE_RAMFUNCTION\
        DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384\
        __STARTUP_CLEAR_BSS\
        FSL_ETH_ENABLE_CACHE_CONTROL"
)

mcux_add_armgcc_linker_script(
    TARGETS release_motor_controller release_motor_iodevice
    BASE_PATH ${app_board_root}
    LINKER demo_apps/avb_tsn/common/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_flash.ld
)