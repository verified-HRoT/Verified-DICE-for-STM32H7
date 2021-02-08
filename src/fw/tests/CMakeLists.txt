#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

add_executable(fw_tests
    fw_tests.cpp
)

target_link_libraries(fw_tests
    dice_image_lib_host
    l0_image_lib_host
    l1_image_lib_host
    headerlib
    gtest gtest_main
    pthread
)

add_test(
    NAME fw_tests
    COMMAND fw_tests
                ${FW_DIR}/keys/KEY_MANIFEST_ED25519KEY.txt
                ${FW_DIR}/keys/L0_ED25519KEY.txt
                ${FW_DIR}/keys/L1_ED25519KEY.txt
)
