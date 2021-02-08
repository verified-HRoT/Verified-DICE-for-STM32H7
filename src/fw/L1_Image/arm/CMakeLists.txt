#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

include(${FW_DIR}/arm/generate_binary_image.cmake)

set(IMAGE_NAME L1_Image)
set(IMAGE_ELF ${IMAGE_NAME}.elf)
set(IMAGE_BIN ${IMAGE_NAME}.bin)
set(IMAGE_VERSION ${VERSION_L1_FULL})

set(IMAGE_KEY ${FW_DIR}/keys/L1_ED25519KEY.txt)

set(LINKERSCRIPT STM32H753ZITx_L1.ld)

search_source_files(L1_ARM_SOURCES)

add_executable(${IMAGE_ELF}
    ${L1_COMMON_SOURCES}
    ${L1_ARM_SOURCES}
)

add_version_definitions(${IMAGE_ELF})

target_compile_definitions(${IMAGE_ELF}
    PRIVATE
        -DSTM32H753xx
        -DUSE_HAL_DRIVER
)

target_link_libraries(${IMAGE_ELF}
    fw_common_arm
)

generate_binary_image(TRUE 0)

# Target for the global FW CMakeLists.txt

add_custom_target(L1_IMAGE_TARGET ALL
    DEPENDS ${IMAGE_HEADER_SIGNED} ${IMAGE_BIN}
)
