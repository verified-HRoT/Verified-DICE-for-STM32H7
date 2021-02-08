#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

add_library(crypto_host ${CRYPTO_SOURCES})

target_link_libraries(crypto_host
    libcommon_host
)
