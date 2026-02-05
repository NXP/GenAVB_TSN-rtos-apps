mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES
    ${board_root}/${board}/demo_apps/avb_tsn/dsa_common/${core_id}/pin_mux_lpspi.h
    ${board_root}/${board}/demo_apps/avb_tsn/dsa_common/${core_id}/pin_mux_lpspi.c
    ${board_root}/${board}/demo_apps/avb_tsn/dsa_common/${core_id}/main.c
)

include(${CMAKE_CURRENT_LIST_DIR}/${core_id}/reconfig.cmake OPTIONAL)
