mcux_add_macro(
    TARGETS release debug release_hybrid
    TOOLCHAINS armgcc
    AS "\
    __STARTUP_INITIALIZE_TCM_ECC \
")

mcux_add_macro(
    TARGETS hyperram_release hyperram_debug
    CC "CONFIG_APP_HYPERRAM=1"
)

mcux_add_macro(
    CC "\
    TCP_SND_BUF=4*TCP_MSS \
    TCP_WND=4*TCP_MSS \
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
")

mcux_add_macro(
    CC "\
    CONFIG_APP_EP_NUM_PORTS=1 \
    CONFIG_APP_BR_NUM_PORTS=3 \
")

mcux_add_configuration(
    CC "-DCONFIG_APP_BR_LOGICAL_PORT_LIST={2,3,4}"
)

mcux_add_include(
    INCLUDES
    .
)

mcux_add_armgcc_configuration(
    LD "\
    -Xlinker --defsym \
    -Xlinker BOARD_FLASH_START_ADDR=0x04000000 \
")

mcux_add_armgcc_configuration(
    LD "\
    -Xlinker --defsym \
    -Xlinker BOARD_HYPERRAM_START_ADDR=0x28000000 \
")

mcux_add_source(
    SOURCES
    genavb_sdk.h
)

mcux_add_iar_linker_script(
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/iar/MIMXRT118xxxxxx_${core_id}_ram.icf
)

mcux_add_armgcc_linker_script(
    TARGETS release_hybrid
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_flexspi_nor.ld
)

mcux_add_armgcc_linker_script(
    TARGETS ram_release_hybrid
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_ram.ld
)

mcux_add_armgcc_linker_script(
    TARGETS hyperram_release
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_hyperram.ld
)

mcux_add_source(
    BASE_PATH ${app_root_path}
    SOURCES
    devices/MIMXRT118x/common/${core_id}/main.c
)
