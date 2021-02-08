#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

add_library(dice_engine_host ${DICE_ENGINE_SOURCES})

target_link_libraries(dice_engine_host
    crypto_host
    libcommon_host
)
