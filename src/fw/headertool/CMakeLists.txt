#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

# Library

add_library(headerlib
    header_utils.c
    header_utils.h
)

target_link_libraries(headerlib
    crypto_host
)

# Tool

if(CROSS_COMPILE_ARM)
    add_executable(headertool
        main.c
    )

    target_link_libraries(headertool
        headerlib
    )
endif()
