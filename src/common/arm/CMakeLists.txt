#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

search_source_files(LIBCOMMON_SOURCES_ARM)

add_library(libcommon_arm
    ${LIBCOMMON_SOURCES}
    ${LIBCOMMON_SOURCES_ARM}
)

# Use hardware random number generator instead of rand()
if(USE_HW_RNG)
    target_compile_definitions(libcommon_arm
        PRIVATE
            -DUSE_HW_RNG
    )
endif()

target_link_libraries(libcommon_arm -lstdc++ kremlib_arm)
