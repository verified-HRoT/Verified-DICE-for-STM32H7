#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

include(${FW_DIR}/arm/generate_binary_image.cmake)

set(IMAGE_NAME DICE_Image)
set(IMAGE_ELF ${IMAGE_NAME}.elf)
set(IMAGE_BIN ${IMAGE_NAME}.bin)

set(LINKERSCRIPT STM32H753ZITx_DICE.ld)

search_source_files(DICE_ARM_SOURCES)

add_executable(${IMAGE_ELF}
    ${DICE_COMMON_SOURCES}
    ${DICE_ARM_SOURCES}
)

add_version_definitions(${IMAGE_ELF})

target_compile_definitions(${IMAGE_ELF}
    PRIVATE
        -DSTM32H753xx
        -DUSE_HAL_DRIVER
)

if(HW_IGNORE_PROTECTION_CHECKS)
    target_compile_definitions(${IMAGE_ELF}
        PRIVATE
            -DHW_IGNORE_PROTECTION_CHECKS=1
    )
else()
    target_compile_definitions(${IMAGE_ELF}
        PRIVATE
            -DHW_IGNORE_PROTECTION_CHECKS=0
    )
endif()

if(HW_SET_RSS_PROTECTIONS_AT_RUNTIME)
    target_compile_definitions(${IMAGE_ELF}
        PRIVATE
            -DHW_SET_RSS_PROTECTIONS_AT_RUNTIME=1
    )
else()
    target_compile_definitions(${IMAGE_ELF}
        PRIVATE
            -DHW_SET_RSS_PROTECTIONS_AT_RUNTIME=0
    )
endif()

target_link_libraries(${IMAGE_ELF}
    -lc
    dice_engine_arm
    fw_common_arm
)

generate_binary_image(FALSE 0)

# Target for the global FW CMakeLists.txt

add_custom_target(DICE_IMAGE_TARGET ALL
    DEPENDS ${IMAGE_BIN}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_BIN} DESTINATION firmware)
