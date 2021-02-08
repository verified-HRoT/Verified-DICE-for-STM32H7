#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

add_library(l0_engine_arm ${L0_ENGINE_SOURCES})

target_link_libraries(l0_engine_arm
    crypto_arm
    libcommon_arm
    -lstdc++
)
