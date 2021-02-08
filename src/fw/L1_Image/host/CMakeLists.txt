#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(L1_HOST_SOURCES)

add_library(l1_image_lib_host
    ${L1_COMMON_SOURCES}
    ${L1_HOST_SOURCES}
)

add_version_definitions(l1_image_lib_host)

target_link_libraries(l1_image_lib_host
    fw_common_host
)
