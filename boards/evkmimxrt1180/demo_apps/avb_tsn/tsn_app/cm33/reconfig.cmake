mcux_add_macro(
    TARGETS release debug release_hybrid release_no_enetc0
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
    LWIP_DEFAULT_LOGICAL_PORT=1 \
    TCP_SND_BUF=4*TCP_MSS \
    TCP_WND=4*TCP_MSS \
    DEBUG_CONSOLE_TRANSMIT_BUFFER_LEN=16384 \
")

mcux_add_source(
    SOURCES
    genavb_sdk.h
    no_enetc0/genavb_sdk.h
    hybrid/genavb_sdk.h
)

mcux_add_iar_linker_script(
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/iar/MIMXRT118xxxxxx_${core_id}_ram.icf
)

mcux_add_armgcc_linker_script(
    TARGETS release_no_enetc0 release_hybrid release_hybrid_motor_controller
    BASE_PATH ${app_root_path}
    LINKER devices/MIMXRT118x/common/${core_id}/armgcc/MIMXRT118xxxxxx_${core_id}_flexspi_nor.ld
)

mcux_add_armgcc_linker_script(
    TARGETS ram_release_no_enetc0 ram_release_hybrid ram_release_hybrid_motor_controller
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

if(${CMAKE_BUILD_TYPE} MATCHES "hyperram")
    include (${app_board_prj_path}/${core_id}/netconf.cmake OPTIONAL)
endif()
