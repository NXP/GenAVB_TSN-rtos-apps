if (CONFIG_MCUX_PRJSEG_config.board.tsn_motor_iodevice)
    mcux_add_source(
        BASE_PATH ${SdkRootDirPath}
        SOURCES
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/mc_periph_init.c
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/mc_periph_init.h
        ${board_root}/${board}/demo_apps/avb_tsn/common/${core_id}/mcdrv.h
        examples/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_qd.c
        examples/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_enc_qd.h
        examples/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_adcetc_imxrt11xx_evkb.c
        examples/demo_apps/avb_tsn/tsn_app/motor/mc_drivers/mcdrv_adcetc_imxrt11xx_evkb.h
    )
endif()
