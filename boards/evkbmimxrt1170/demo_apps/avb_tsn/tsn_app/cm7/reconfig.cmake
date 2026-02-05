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
")

mcux_add_armgcc_linker_script(
    TARGETS release_motor
    BASE_PATH ${SdkRootDirPath}
    LINKER ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}.ld
)

mcux_add_armgcc_linker_script(
    TARGETS ram_release_motor
    BASE_PATH ${SdkRootDirPath}
    LINKER ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}_ram.ld
)

mcux_add_armgcc_linker_script(
    TARGETS sdram_release
    BASE_PATH ${SdkRootDirPath}
    LINKER ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}_sdram.ld
)

