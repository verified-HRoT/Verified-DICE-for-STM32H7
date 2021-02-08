#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

search_source_files(fw_COMMON_SOURCES)

if(HW_ENABLE_WATCHDOG)
    add_definitions(-DHW_ENABLE_WATCHDOG=1)
else()
    add_definitions(-DHW_ENABLE_WATCHDOG=0)
endif()

add_cross_compile_subdirectories()
