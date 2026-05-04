mcux_add_armgcc_linker_script(
    TARGETS release debug
    BASE_PATH ${app_board_root}
    LINKER demo_apps/avb_tsn/common/${core_id}/armgcc/MIMXRT1052xxxxx.ld
)
