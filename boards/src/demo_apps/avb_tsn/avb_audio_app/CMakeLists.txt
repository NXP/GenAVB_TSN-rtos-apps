cmake_minimum_required(VERSION 3.30.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(avb_audio_app LANGUAGES C ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/demo_apps/avb_tsn/avb_audio_app)

include(${CMAKE_CURRENT_LIST_DIR}/../common/mcux_flags.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../common/mcux_common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../common/prjseg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/mcux_avb_app.cmake)

include(${SdkRootDirPath}/CMakeLists.txt)

include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/avb_tsn/avb_audio_app/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
