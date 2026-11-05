if(CONFIG_MCUX_PRJSEG_module.board.iar_stubs)
    mcux_add_source(
        SOURCES
        stubs/assert_func.c
        stubs/errno.c
        stubs/errno.h
        stubs/getopt.c
        stubs/getopt.h
        TOOLCHAINS iar
    )

    mcux_add_include(
        INCLUDES
        ./stubs/.
        TOOLCHAINS iar
    )
endif()
