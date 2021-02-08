#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

add_library(kremlib_arm STATIC
    ${KREMLIN_SOURCES}
    ${KREMLIN_DIST_SOURCES}
)

target_include_directories(kremlib_arm
    PUBLIC
        ${KREMLIN_DIR}
        ${KREMLIB_DIST_DIR}
)
