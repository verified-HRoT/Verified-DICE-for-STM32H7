#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

cmake_minimum_required(VERSION 3.13)
project(Verified_Hardware_Boot)

enable_language(CXX C ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Default to Debug builds if build type is not set
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build: Debug/Release" FORCE)
endif()


# Static analysis
# set(CMAKE_CXX_CLANG_TIDY clang-tidy-10 -checks=-*,readability-*,-header-filter=.* .)
# set(CMAKE_CXX_CPPCHECK cppcheck --std=c++11 .)

# Setup testing
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

include(cmake/definitions.cmake)
include(cmake/build_options.cmake)
include(cmake/common.cmake)
include(cmake/git_version.cmake)
include(cmake/install.cmake)
include(cmake/toolchain_arm.cmake)
include(cmake/toolchain_host.cmake)
include(cmake/versions.cmake)

# Global include directories
include_directories(
    ${SRC_DIR}
    ${GSL_DIR}/include
)

add_subdirectory(3rdparty)
add_subdirectory(src)
