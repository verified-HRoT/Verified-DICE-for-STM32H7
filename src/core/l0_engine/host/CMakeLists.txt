#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

add_library(l0_engine_host ${L0_ENGINE_SOURCES})

target_link_libraries(l0_engine_host
    crypto_host
    libcommon_host
)
