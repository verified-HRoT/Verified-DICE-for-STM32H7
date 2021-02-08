#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

# Build for NUCLEO-H743ZI2 boards rather than NUCLEO-H743ZI boards
if(USE_NUCLEO2_BOARDS)
    add_definitions(-DUSE_STM32H7XX_NUCLEO_144_MB1364)
endif()

add_subdirectory(common)

include(${FW_DIR}/arm/generate_layer_keys.cmake)
generate_layer_keys()

add_subdirectory(DICE_Image)
add_subdirectory(L0_Image)
add_subdirectory(L1_Image)

add_subdirectory(headertool)
add_subdirectory(keymanifesttool)

add_subdirectory(tests)

if(CROSS_COMPILE_ARM)
    add_subdirectory(keytool)
    add_subdirectory(signtool)
endif()

add_cross_compile_subdirectories()
