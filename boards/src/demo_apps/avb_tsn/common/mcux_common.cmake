if (CONFIG_MCUX_PRJSEG_config.board.src_common_files)
    mcux_add_include(
        INCLUDES
        ../..
        ./lwip
    )

    mcux_add_source(
        SOURCES
        application_util.c
        common.c
        common.h
        configs.c
        delay.c
        fault_handler.c
        FreeRTOSConfigCommon.h
        frer.h
        fp.c
        fp.h
        genavb.c
        genavb.h
        log.c
        log.h
        lwip/lwip.c
        lwip/lwip.h
        lwip/lwip_ethernetif.c
        lwip/lwip_ethernetif.h
        lwip/lwip_iperf.c
        lwip/lwip_iperf.h
        lwip/lwipopts.h
        memcpy.c
        qbv.c
        qbv.h
        shell.c
        shell.h
        stats_task.c
        stats_task.h
        storage.c
        storage.h
        system_config.c
        system_config.h
        uart.c
        uart.h
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_tsn_bridge)
    mcux_add_source(
        SOURCES
        fdb.c
        fdb.h
        frer.c
        hsr.c
        hsr.h
        psfp.c
        psfp.h
        stream_identification.c
        stream_identification.h
        vlan.c
        vlan.h
    )
endif()

if(CONFIG_MCUX_PRJSEG_config.board.app_storage)
    mcux_add_source(
        SOURCES
        ../common/flexspi_nor_flash_ops.c
        ../common/lfs_qspi_nor.c
        DEVICE_IDS MIMXRT1189xxxxx MIMXRT1186xxxxx MIMXRT1176xxxxx
        CORE_IDS "cm33" "cm7"
    )

    mcux_add_source(
        SOURCES
        BASE_PATH ${app_board_root}
        SOURCES
        demo_apps/avb_tsn/common/flexspi_hyper_flash_ops.c
        demo_apps/avb_tsn/common/flexspi_ops.h
        demo_apps/avb_tsn/common/lfs_hyperflash_nor.c
        DEVICE_IDS MIMXRT1052xxxxB
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/iar/mcux_iar.cmake)
include(${app_board_root}/demo_apps/avb_tsn/common/mcux_linker.cmake OPTIONAL)
include(${SdkRootDirPath}/components/rtos-apps/mcux_rtos_apps.cmake)
include(${SdkRootDirPath}/components/rtos-abstraction-layer/freertos/mcux_rtos_abstraction_layer.cmake)
include(${SdkRootDirPath}/middleware/gen_avb/mcux_genavb.cmake)
