#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(L0_HOST_SOURCES)

add_library(l0_image_lib_host
    ${L0_COMMON_SOURCES}
    ${L0_HOST_SOURCES}
)

add_version_definitions(l0_image_lib_host)

target_link_libraries(l0_image_lib_host
    l0_engine_host
    fw_common_host
)
