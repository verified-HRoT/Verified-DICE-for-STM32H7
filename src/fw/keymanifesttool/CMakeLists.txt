#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

add_executable(keymanifesttool
    main.cpp
)

target_link_libraries(keymanifesttool
    crypto_host
    cxxopts
)
