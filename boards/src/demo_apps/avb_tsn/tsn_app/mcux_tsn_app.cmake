if (CONFIG_MCUX_PRJSEG_config.board.tsn_app)
    mcux_add_include(
        INCLUDES
        .
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_tsn_endpoint)
    mcux_add_source(
        SOURCES
        configs.c
        shell.c
        shell.h
        tsn_app.c
        tsn_app.h
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_motor_iodevice)
    mcux_add_include(
        INCLUDES
        motor/.
        motor/mc_algorithms/.
        motor/mc_cfg_template/.
        motor/mc_drivers/.
        motor/mc_state_machine/.
        motor/state_machine/.
    )

    mcux_add_source(
        SOURCES
        motor/mc_algorithms/pmsm_control.c
        motor/mc_algorithms/pmsm_control.h
        motor/mc_cfg_template/m1_pmsm_appconfig.h
        motor/mc_drivers/mcdrv_pwm3ph_pwma.c
        motor/mc_drivers/mcdrv_pwm3ph_pwma.h
        motor/mc_state_machine/m1_sm_snsless_enc.c
        motor/mc_state_machine/m1_sm_snsless_enc.h
        motor/mc_state_machine/sm_common.h
        motor/state_machine/state_machine.c
        motor/state_machine/state_machine.h
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_motor_controller)
    mcux_add_include(
        INCLUDES
        motor/.
        motor/mc_algorithms/.
        motor/mc_cfg_template/.
        motor/mc_drivers/.
        motor/state_machine/.
    )

    mcux_add_source(
        SOURCES
        motor/mc_cfg_template/m1_pmsm_appconfig.h
    )
endif()

include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/avb_tsn/tsn_app/${core_id}/mcux_opt_cfg.cmake OPTIONAL)
include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/avb_tsn/tsn_app/mcux_lib_variable.cmake OPTIONAL)
