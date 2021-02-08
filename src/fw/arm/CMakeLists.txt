#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

# The STM HAL simply doesn't come with any build system, so this folder
# implements one that is integrated in our build system.

add_subdirectory(libstmhal)

# The following rules don't use files as dependencies because the image binaries
# are generated using add_custom_command() and add_custom_target() in a
# different folder in the source tree. The rules to generate the binary files
# are only seen in the same folder. In order to be sure that the global images
# are always up to date, the following rules are always run and only depend on
# phony targets, that are visible everywhere.
#
# This causes the targets below to be built every time, even if there are no
# changes to the dependencies, but that's better than having outdated targets.

# Rules to build the key manifest
# -------------------------------

set(L0_IMAGE_KEY ${FW_DIR}/keys/L0_ED25519KEY.txt)
set(L1_IMAGE_KEY ${FW_DIR}/keys/L1_ED25519KEY.txt)
set(KEY_MANIFEST_KEY ${FW_DIR}/keys/KEY_MANIFEST_ED25519KEY.txt)

set(KEY_MANIFEST_UNSIGNED key_manifest_unsigned.bin)
set(KEY_MANIFEST_SIGNATURE key_manifest_signature.bin)
set(KEY_MANIFEST key_manifest.bin)

add_custom_target(KEY_MANIFEST_TARGET ALL
    COMMAND ${CMAKE_BINARY_DIR}/src/fw/keymanifesttool/keymanifesttool
                --l0key ${L0_IMAGE_KEY} --l1key ${L1_IMAGE_KEY}
                --out ${KEY_MANIFEST_UNSIGNED}
    COMMAND ${CMAKE_BINARY_DIR}/src/fw/signtool/signtool
                ${KEY_MANIFEST_KEY} ${KEY_MANIFEST_UNSIGNED}
                ${KEY_MANIFEST_SIGNATURE}
    COMMAND cat ${KEY_MANIFEST_UNSIGNED} ${KEY_MANIFEST_SIGNATURE}
                > ${KEY_MANIFEST}
)

add_dependencies(KEY_MANIFEST_TARGET
    keymanifesttool
    signtool
)

# Rules to build the update and global firmware images
# ----------------------------------------------------

set(UPDATE_IMAGE L0_L1.bin)
set(GLOBAL_IMAGE DICE_L0_L1.bin)

# Generate image that contains the key manifest, L0 and L1 to be used as update
# image

add_custom_target(UPDATE_IMAGE_TARGET ALL
    COMMAND cat ${KEY_MANIFEST} > ${UPDATE_IMAGE}
    COMMAND truncate -s 1024 ${UPDATE_IMAGE}
    COMMAND cat ../L0_Image/arm/L0_Image_header_signed.bin
                ../L0_Image/arm/L0_Image.bin >> ${UPDATE_IMAGE}
    COMMAND truncate -s 262144 ${UPDATE_IMAGE}
    COMMAND cat ../L1_Image/arm/L1_Image_header_signed.bin
                ../L1_Image/arm/L1_Image.bin >> ${UPDATE_IMAGE}
)

add_dependencies(UPDATE_IMAGE_TARGET
    KEY_MANIFEST_TARGET
    L0_IMAGE_TARGET
    L1_IMAGE_TARGET
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${UPDATE_IMAGE} DESTINATION firmware)

# Generate an image containing everything (to be flashed directly)

add_custom_target(GLOBAL_IMAGE_TARGET ALL
    COMMAND cat ../DICE_Image/arm/DICE_Image.bin > ${GLOBAL_IMAGE}
    COMMAND truncate -s 131072 ${GLOBAL_IMAGE}
    COMMAND cat ${UPDATE_IMAGE} >> ${GLOBAL_IMAGE}
)

add_dependencies(GLOBAL_IMAGE_TARGET
    DICE_IMAGE_TARGET
    UPDATE_IMAGE_TARGET
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${GLOBAL_IMAGE} DESTINATION firmware)
