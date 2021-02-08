#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

add_library(kremlib_host STATIC
    ${KREMLIN_SOURCES}
    ${KREMLIN_DIST_SOURCES}
)

target_include_directories(kremlib_host
    PUBLIC
        ${KREMLIN_DIST_DIR}
        ${KREMLIN_DIR}
)
