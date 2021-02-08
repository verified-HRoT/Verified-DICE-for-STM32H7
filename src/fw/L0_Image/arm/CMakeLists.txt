#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_arm()

include(${FW_DIR}/arm/generate_binary_image.cmake)

set(IMAGE_NAME L0_Image)
set(IMAGE_ELF ${IMAGE_NAME}.elf)
set(IMAGE_BIN ${IMAGE_NAME}.bin)
set(IMAGE_VERSION ${VERSION_L0_FULL})

set(IMAGE_KEY ${FW_DIR}/keys/L0_ED25519KEY.txt)

set(LINKERSCRIPT STM32H753ZITx_L0.ld)

search_source_files(L0_ARM_SOURCES)

add_executable(${IMAGE_ELF}
    ${L0_COMMON_SOURCES}
    ${L0_ARM_SOURCES}
)

add_version_definitions(${IMAGE_ELF})

target_link_libraries(${IMAGE_ELF}
    -lc
    l0_engine_arm
    fw_common_arm
)

generate_binary_image(TRUE 261120) # 256 - 1 KB

# Target for the global FW CMakeLists.txt

add_custom_target(L0_IMAGE_TARGET ALL
    DEPENDS ${IMAGE_HEADER_SIGNED} ${IMAGE_BIN}
)
