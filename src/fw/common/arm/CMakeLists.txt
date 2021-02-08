#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

add_definitions(
    -DSTM32H753xx
    -DUSE_HAL_DRIVER
)

search_source_files(fw_COMMON_ARM_SOURCES)

add_library(fw_common_arm STATIC
    ${fw_COMMON_SOURCES}
    ${fw_COMMON_ARM_SOURCES}
)

target_link_libraries(fw_common_arm
    crypto_arm
    stmhal
)
