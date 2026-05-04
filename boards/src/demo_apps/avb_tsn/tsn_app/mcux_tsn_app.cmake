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
    # remove files from the SDK component that should remain local
    mcux_project_remove_source(
        BASE_PATH ${SdkRootDirPath}/middleware/motor_control/pmsm/pmsm_float
        SOURCES 
        mc_state_machine/m1_sm_snsless_enc.c
        mc_state_machine/m1_sm_snsless_enc.h
        mc_state_machine/pmsm_control.c
        mc_state_machine/pmsm_control.h
        pmsm/pmsm_float/mc_state_machine/sm_common.h
   )

    mcux_add_include(
        INCLUDES
        motor/.
        motor/mc_algorithms/.
        motor/mc_cfg_template/.
        motor/mc_drivers/.
        motor/mc_state_machine/.
    )

    mcux_add_source(
        SOURCES
        motor/mc_algorithms/pmsm_control.c
        motor/mc_algorithms/pmsm_control.h
        motor/mc_cfg_template/m1_pmsm_appconfig.h
        motor/mc_state_machine/m1_sm_snsless_enc.c
        motor/mc_state_machine/m1_sm_snsless_enc.h
        motor/mc_state_machine/sm_common.h
    )
endif()

if (CONFIG_MCUX_PRJSEG_config.board.app_motor_controller)
    mcux_add_include(
        INCLUDES
        motor/.
        motor/mc_algorithms/.
        motor/mc_cfg_template/.
        motor/mc_drivers/.
    )

    mcux_add_source(
        SOURCES
        motor/mc_cfg_template/m1_pmsm_appconfig.h
    )
endif()

include(${app_board_prj_path}/${core_id}/mcux_opt_cfg.cmake OPTIONAL)
include(${app_board_prj_path}/mcux_lib_variable.cmake OPTIONAL)
