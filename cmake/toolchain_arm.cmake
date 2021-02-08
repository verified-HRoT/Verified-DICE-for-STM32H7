#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

macro(set_toolchain_arm)

    # This should never happen. This macro should only be used if
    # CROSS_COMPILE_ARM is set.
    if(NOT CROSS_COMPILE_ARM)
        message(FATAL_ERROR "CROSS_COMPILE_ARM isn't set.")
    endif()

    set(CMAKE_ASM_COMPILER "${CROSS_COMPILE_ARM}arm-none-eabi-gcc")
    set(CMAKE_C_COMPILER "${CROSS_COMPILE_ARM}arm-none-eabi-gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_COMPILE_ARM}arm-none-eabi-g++")
    set(CMAKE_OBJCOPY "${CROSS_COMPILE_ARM}arm-none-eabi-objcopy")
    set(CMAKE_LINKER "${CROSS_COMPILE_ARM}arm-none-eabi-ld")
    set(CMAKE_AR "${CROSS_COMPILE_ARM}arm-none-eabi-ar")

    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR ARM)

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    if(NOT DISABLE_WARNINGS)
        set(WARNFLAGS "-Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-return-type -Wno-unused-but-set-variable")
    endif()

    set(ARCHFLAGS
        "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard \
        --specs=nano.specs"
    )

    set(CMAKE_ASM_FLAGS "${ARCHFLAGS} -c -x assembler-with-cpp")

    set(CMAKE_C_FLAGS "${ARCHFLAGS} ${WARNFLAGS} \
        -Os -ffunction-sections -fdata-sections ${EXTRA_ARM_GCC_C_FLAGS} -DKRML_VERIFIED_UINT128"
    )

    set(CMAKE_CXX_FLAGS "${ARCHFLAGS} ${WARNFLAGS} \
        -Os -nodefaultlibs -fno-rtti -fno-exceptions \
        -ffunction-sections -fdata-sections ${EXTRA_ARM_GCC_CPP_FLAGS} -DKRML_VERIFIED_UINT128"
    )

    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,--start-group")

endmacro()
