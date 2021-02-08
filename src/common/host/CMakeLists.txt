#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(LIBCOMMON_SOURCES_HOST)

add_library(libcommon_host
    ${LIBCOMMON_SOURCES}
    ${LIBCOMMON_SOURCES_HOST}
)

target_link_libraries(libcommon_host
    fw_common_host
    cxxopts
    kremlib_host
)
