if (CONFIG_MCUX_PRJSEG_config.board.tsn_motor_iodevice)
    mcux_add_source(
        BASE_PATH ${app_root_path}
        SOURCES
        devices/MCXE3X/common/mc_periph_init.c
        devices/MCXE3X/common/mc_periph_init.h
        devices/MCXE3X/common/mcdrv.h

        boards/src/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_emios.c
        boards/src/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_emios.h
    )
endif()
