mcux_add_armgcc_linker_script(
    TARGETS release debug
    LINKER ${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}.ld
)

mcux_add_armgcc_linker_script(
    TARGETS ram_release ram_debug
    LINKER ${core_id}/armgcc/${CONFIG_MCUX_HW_DEVICE_ID}_${core_id}_ram.ld
)
