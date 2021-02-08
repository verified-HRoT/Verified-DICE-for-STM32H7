#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

search_source_files_dir(KREMLIN_SOURCES ${KREMLIN_DIR})
search_source_files_dir(KREMLIN_DIST_SOURCES ${KREMLIN_DIST_DIR})

include_directories(${KREMLIN_DIR})
include_directories(${KREMLIN_DIST_DIR})

add_cross_compile_subdirectories()
