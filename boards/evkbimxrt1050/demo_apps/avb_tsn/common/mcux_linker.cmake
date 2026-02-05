mcux_add_armgcc_linker_script(
    TARGETS release debug
    BASE_PATH ${SdkRootDirPath}
    LINKER ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/armgcc/MIMXRT1052xxxxx.ld
)
