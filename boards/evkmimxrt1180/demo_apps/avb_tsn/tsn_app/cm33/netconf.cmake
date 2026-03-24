set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCONFIG_APP_NETCONF=1")
set(NetConfPath "${SdkRootDirPath}/../mcux-netconf")
set(McuxYangPath ${SdkRootDirPath}/components/mcux-yang)
set(SysrepoGenAvbPath ${SdkRootDirPath}/components/sysrepo-genavb)

target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    MBEDTLS_FS_IO=1
    MBEDTLS_THREADING_PTHREAD=1
    MBEDTLS_ERROR_C=1
    MBEDTLS_THREADING_C=1
#    MBEDTLS_CONFIG_FILE="mcux_mbedtls_config.h"
)

target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC _POSIX_TIMERS _POSIX_THREADS _POSIX_SOURCE)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC ${McuxYangPath})
target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC ${SysrepoGenAvbPath}/include)
target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${NetConfPath}/pcre2/mcux/include
    ${NetConfPath}/libyang/mcux/include
    ${NetConfPath}/sysrepo/mcux/include
    ${NetConfPath}/rtos/freertos/freertos-posix/include
    ${NetConfPath}/rtos/freertos/freertos-posix/include/private/
    ${NetConfPath}/rtos/freertos/freertos-posix/FreeRTOS-Plus-POSIX/
    ${NetConfPath}/rtos/freertos/freertos-posix-pthread/include
    ${NetConfPath}/rtos/freertos/freertos-emb-posix/include
    ${NetConfPath}/rtos/freertos/freertos-posix/FreeRTOS-Plus-POSIX/include
    ${NetConfPath}/rtos/freertos/freertos-posix/FreeRTOS-Plus-POSIX/include/portable
    ${NetConfPath}/rtos/freertos/freertos-posix/header_project/
    ${NetConfPath}/middleware/lwip/src/include/compat/posix/
)

get_property(NETCONF_INCLUDE_DIRS TARGET ${MCUX_SDK_PROJECT_NAME} PROPERTY INCLUDE_DIRECTORIES)
get_property(NETCONF_COMPILE_DEFS TARGET ${MCUX_SDK_PROJECT_NAME} PROPERTY INTERFACE_COMPILE_DEFINITIONS)

include(${NetConfPath}/build.cmake OPTIONAL)
include(${SysrepoGenAvbPath}/sysrepo_genavb.cmake)
include(${McuxYangPath}/yang_modules.cmake)

set (NETCONF_LIBRARIES libmcux-netconf.a libnetopeer2.a libnetconf2.a libsysrepo.a liblibyang.a liblibcurl.a libpcre2.a liblibssh.a libcrypt.a libmcux-posix.a)

# Get the current list of linked libraries
get_target_property(TARGET_LIBRARIES ${MCUX_SDK_PROJECT_NAME} LINK_LIBRARIES)

# Remove the specific library from the list
list(REMOVE_ITEM TARGET_LIBRARIES -lc)
list(REMOVE_ITEM TARGET_LIBRARIES -lm)
list(REMOVE_ITEM TARGET_LIBRARIES -lgcc)
list(REMOVE_ITEM TARGET_LIBRARIES -lnosys)

# Overwrite the LINK_LIBRARIES property with the modified list
set_target_properties(${MCUX_SDK_PROJECT_NAME} PROPERTIES LINK_LIBRARIES ${TARGET_LIBRARIES})

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE sysrepo-genavb)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE mcux-yang-modules)

foreach(netconf-lib ${NETCONF_LIBRARIES})
	target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE  ${NetConfPath}/${CMAKE_BUILD_TYPE}/${netconf-lib})
endforeach()
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group  -lm -lc -lgcc -lnosys  -Wl,--end-group)
