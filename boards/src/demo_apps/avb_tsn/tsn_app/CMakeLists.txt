cmake_minimum_required(VERSION 3.30.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

get_filename_component(_app_root_path "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
mcux_set_variable(app_root_path "${_app_root_path}")
mcux_set_variable(app_board_root "${app_root_path}/boards/${board}")
mcux_set_variable(app_path "${app_root_path}/boards/src/demo_apps/avb_tsn/tsn_app")
mcux_set_variable(app_board_prj_path "${app_root_path}/boards/${board}/demo_apps/avb_tsn/tsn_app")

project(tsn_app LANGUAGES C ASM PROJECT_BOARD_PORT_PATH ${app_board_prj_path} CUSTOM_PRJ_CONF_PATH ${app_root_path}/boards/src/demo_apps/avb_tsn)

include(${CMAKE_CURRENT_LIST_DIR}/../common/mcux_flags.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../common/mcux_common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../common/prjseg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/mcux_tsn_app.cmake)

include(${SdkRootDirPath}/CMakeLists.txt)

include(${app_board_prj_path}/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
