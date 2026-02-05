if (CONFIG_MCUX_PRJSEG_config.board.app_bridge_default)
    mcux_add_macro(
        CC "\
        CONFIG_APP_EP_NUM_PORTS=2 \
    ")

    mcux_add_include(
        INCLUDES
        .
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_bridge_no_enetc0)
    mcux_add_include(
        INCLUDES
        ./no_enetc0
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_bridge_hybrid)
    mcux_add_macro(
        CC "\
        CONFIG_APP_BR_NUM_PORTS=4 \
    ")

    mcux_add_configuration(
        CC "-DCONFIG_APP_BR_LOGICAL_PORT_LIST={0,2,3,4}"
    )

    mcux_add_include(
        INCLUDES
        ./hybrid
    )
endif()
