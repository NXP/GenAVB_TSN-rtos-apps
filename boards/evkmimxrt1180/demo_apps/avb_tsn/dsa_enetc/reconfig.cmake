mcux_add_source(
    BASE_PATH ${app_board_root}
    SOURCES
    demo_apps/avb_tsn/dsa_common/${core_id}/pin_mux_lpspi.h
    demo_apps/avb_tsn/dsa_common/${core_id}/pin_mux_lpspi.c
    demo_apps/avb_tsn/dsa_common/${core_id}/main.c
)

include(${CMAKE_CURRENT_LIST_DIR}/${core_id}/reconfig.cmake OPTIONAL)
