mcux_add_armgcc_linker_script(
    TARGETS release debug
    BASE_PATH ${app_root_path}
    LINKER devices/MCXE3X/common/armgcc/MCXE3x_flash.ld
)
