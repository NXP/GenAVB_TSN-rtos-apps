if (CONFIG_MCUX_PRJSEG_config.board.tsn_motor_iodevice)
    mcux_add_source(
        BASE_PATH ${app_root_path}
        SOURCES
        boards/${board}/demo_apps/avb_tsn/common/mc_periph_init.c
        boards/${board}/demo_apps/avb_tsn/common/mc_periph_init.h
        boards/${board}/demo_apps/avb_tsn/common/mcdrv.h
        boards/src/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_emios.c
        boards/src/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_emios.h
    )
endif()
