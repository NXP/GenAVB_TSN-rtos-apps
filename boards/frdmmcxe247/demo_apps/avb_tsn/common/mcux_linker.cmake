mcux_add_armgcc_linker_script(
    TARGETS release debug
    LINKER armgcc/${device}_flash.ld
)
