#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

add_library(dice_engine_arm ${DICE_ENGINE_SOURCES})

target_link_libraries(dice_engine_arm
    crypto_arm
    libcommon_arm
)
