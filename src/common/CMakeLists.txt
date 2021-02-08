#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

search_source_files(LIBCOMMON_SOURCES)

include_directories(${KREMLIN_DIR})
include_directories(${KREMLIN_DIST_DIR})

add_subdirectory(tests)

add_cross_compile_subdirectories()
