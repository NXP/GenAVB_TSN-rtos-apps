cmake_minimum_required(VERSION 3.30.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(dsa_enetc LANGUAGES C ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/demo_apps/avb_tsn/dsa_enetc)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/mcux_common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../common/mcux_flags.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../common/prjseg.cmake)

include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/avb_tsn/dsa_enetc/mcux_lib_variable.cmake OPTIONAL)

include(${SdkRootDirPath}/CMakeLists.txt)

include(${CMAKE_CURRENT_LIST_DIR}/../mcux_dsa_common.cmake)

include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/avb_tsn/dsa_enetc/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
