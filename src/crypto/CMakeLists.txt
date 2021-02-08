#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

search_source_files(CRYPTO_SOURCES)

include_directories(${KREMLIN_DIR})
include_directories(${KREMLIN_DIST_DIR})

add_cross_compile_subdirectories()
