#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(DICE_HOST_SOURCES)

add_library(dice_image_lib_host
    ${DICE_COMMON_SOURCES}
    ${DICE_HOST_SOURCES}
)

add_version_definitions(dice_image_lib_host)

target_link_libraries(dice_image_lib_host
    dice_engine_host
    fw_common_host
)
