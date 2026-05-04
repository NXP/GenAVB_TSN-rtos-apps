if (CONFIG_MCUX_PRJSEG_config.board.app_default)
    mcux_add_include(
        INCLUDES
        .
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_enetc0)
    mcux_add_macro(
        CC "\
        CONFIG_APP_EP_NUM_PORTS=2 \
    ")

    mcux_add_include(
        INCLUDES
        ./enetc0
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_motor_iodevice)
    mcux_add_source(
        BASE_PATH ${app_root_path}
        SOURCES
        boards/${board}/demo_apps/avb_tsn/common/${core_id}/mc_periph_init.c
        boards/${board}/demo_apps/avb_tsn/common/${core_id}/mc_periph_init.h
        boards/${board}/demo_apps/avb_tsn/common/${core_id}/mcdrv.h
        boards/src/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_eqd.c
        boards/src/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_eqd.h
    )
endif()
