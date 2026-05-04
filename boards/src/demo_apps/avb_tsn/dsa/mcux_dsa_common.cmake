if (CONFIG_MCUX_PRJSEG_config.board.dsa)
    mcux_add_include(
        INCLUDES
        .
    )

    mcux_add_source(
        SOURCES
        dsa_control.c
        dsa_control.h
        dsa_control_lpspi.c
        dsa_control_lpspi.h
        dsa_control_utils.c
        dsa_control_utils.h
        dsa_virtual_switch.c
        dsa_virtual_switch.h
    )
endif()

include(${SdkRootDirPath}/middleware/heterogeneous-multicore/net/boards/evkmimxrt1180/cm33/mcux/mcux_virtual_switch.cmake)
