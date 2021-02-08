#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(fw_COMMON_HOST_SOURCES)

add_library(fw_common_host STATIC
    ${fw_COMMON_SOURCES}
    ${fw_COMMON_HOST_SOURCES}
)

target_link_libraries(fw_common_host
    crypto_host
)
