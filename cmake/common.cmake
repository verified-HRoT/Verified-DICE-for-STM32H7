#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

# Searches all the source files in the current directory and saves it in 'var'
macro(search_source_files var)
    file(GLOB ${var} CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.s
    )
endmacro()

# Searches all the source files in the current directory, given subdir and saves it in 'var'
macro(search_source_files_subdir var subdir)
    file(GLOB ${var} CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.s
    )
endmacro()

# Searches all the source files in the given dir and saves it in 'var'
macro(search_source_files_dir var dir)
    file(GLOB ${var} CONFIGURE_DEPENDS
        ${dir}/*.c
        ${dir}/*.cpp
        ${dir}/*.h
        ${dir}/*.s
    )
endmacro()

# Add subdirectories 'arm' and 'host' when needed if they exist
macro(add_cross_compile_subdirectories)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/arm)
        if(CROSS_COMPILE_ARM)
            add_subdirectory(arm)
        endif()
    endif()

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/host)
        add_subdirectory(host)
    endif()
endmacro()
