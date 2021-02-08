#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

macro(set_toolchain_host)

    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
            message(FATAL_ERROR "GCC version 8.0 or greater is needed.")
        endif()

        if(NOT DISABLE_WARNINGS)
            set(WARNFLAGS "-Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-return-type -Wno-unused-but-set-variable")
        endif()

        set(CMAKE_C_FLAGS "${WARNFLAGS} ${EXTRA_GCC_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${WARNFLAGS} -Wno-class-memaccess ${EXTRA_GCC_CXX_FLAGS}")

        set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,--start-group")
        set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections -Wl,--start-group")

        link_libraries(stdc++fs)
    endif()

    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

endmacro()
