if (CONFIG_MCUX_PRJSEG_module.board.avbtsn_default_board_files)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/board.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/board.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/clock_config.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/clock_config.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/pin_mux.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/pin_mux.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/FreeRTOSConfig.h
        DEVICE_IDS MIMXRT1052xxxxB MCXE31B MCXE247
    )

    mcux_add_include(
        BASE_PATH ${SdkRootDirPath}
        INCLUDES
        ${board_root}/${board}/demo_apps/avb_tsn/common
        DEVICE_IDS MIMXRT1052xxxxB MCXE31B MCXE247
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/semc_sdram.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/semc_sdram.h
        DEVICE_IDS MIMXRT1052xxxxB
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/hardware_init.c
        DEVICE_IDS MCXE31B MCXE247
    )

endif()

if (CONFIG_MCUX_PRJSEG_module.board.avbtsn_default_coreid_board_files)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/board.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/board.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/clock_config.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/clock_config.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/pin_mux.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/pin_mux.h
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx MIMXRT1176xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/hardware_init.c
        DEVICE_IDS MIMXRT1186xxxxx
        CORE_IDS "cm33"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/flexspi_hyperram.c
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/flexspi_flash_cfg.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/flexspi_ops.h
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_include(
        BASE_PATH ${SdkRootDirPath}
        INCLUDES
        ${board_root}/${board}/demo_apps/avb_tsn/common
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_include(
        BASE_PATH ${SdkRootDirPath}
        INCLUDES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx MIMXRT1176xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/flexspi_flash_cfg.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/flexspi_ops.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/FreeRTOSConfig.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/semc_sdram.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/semc_sdram.c
        DEVICE_IDS MIMXRT1176xxxxx
        CORE_IDS "cm7"
    )
endif()

if (CONFIG_MCUX_PRJSEG_module.board.avbtsn_default_genavb_sdk_files)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/genavb_sdk.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/genavb_sdk.c
        DEVICE_IDS MIMXRT1176xxxxx
        CORE_IDS "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/genavb_sdk_net_port.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/genavb_sdk_net_port_no_enetc0.h
        DEVICE_IDS MIMXRT1189xxxxx
        CORE_IDS "cm33"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/genavb_sdk_net_port_enetc0.h
        DEVICE_IDS MIMXRT1189xxxxx
        CORE_IDS "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/genavb_sdk.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/genavb_sdk.c
        DEVICE_IDS MIMXRT1052xxxxB MCXE31B MCXE247
    )
endif()

if (CONFIG_MCUX_PRJSEG_module.board.avbtsn_rt118x_default_device_files)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/board.c
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/shared_config.c
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/shared_config.h
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/FreeRTOSConfig.h
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/flexspi_hyperram.h
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_include(
        BASE_PATH ${SdkRootDirPath}
        INCLUDES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/configs.c
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/multicore.c
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/multicore.h
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/trdc.c
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/trdc.h
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33"
    )
endif()

if (CONFIG_MCUX_PRJSEG_module.board.avbtsn_rt118x_device_default_genavb_sdk_files)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/genavb_sdk.c
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/genavb_sdk_common.h
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/genavb_sdk_net_port.h
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm7"
    )
endif()

if (CONFIG_MCUX_PRJSEG_module.board.avbtsn_default_startup_files)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/armgcc/startup_${device}_${core_id}.S
        TOOLCHAINS armgcc
        DEVICE_IDS MIMXRT1176xxxxx
        CORE_IDS "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/armgcc/startup_MIMXRT1052.S
        TOOLCHAINS armgcc
        DEVICE_IDS MIMXRT1052xxxxB
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/startup_${device}.c
        TOOLCHAINS armgcc mcux
        DEVICE_IDS MCXE31B
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/armgcc/startup_${device}.S
        TOOLCHAINS armgcc mcux
        DEVICE_IDS MCXE247
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/armgcc/startup_MIMXRT118x_${core_id}.S
        TOOLCHAINS armgcc
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${device_root}/${soc_portfolio}/${soc_series}/MIMXRT118x/common/${core_id}/iar/startup_MIMXRT118x_${core_id}.s
        TOOLCHAINS iar
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx
        CORE_IDS "cm33" "cm7"
    )
endif()
