#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

add_library(crypto_arm ${CRYPTO_SOURCES})

target_include_directories(crypto_arm
    PUBLIC
        ${FW_DIR}/config
)

target_link_libraries(crypto_arm
    libcommon_arm
)
