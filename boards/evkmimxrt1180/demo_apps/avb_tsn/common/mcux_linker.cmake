mcux_add_iar_linker_script(
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/iar/MIMXRT118xxxxxx_${core_id}_ram.icf
)

mcux_add_armgcc_linker_script(
    TARGETS ram_release ram_debug
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_ram.ld
)

mcux_add_armgcc_linker_script(
    TARGETS release debug
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_flexspi_nor.ld
)
