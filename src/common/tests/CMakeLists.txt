#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(COMMON_TESTS_SOURCES)

add_executable(common_tests
    ${COMMON_TESTS_SOURCES}
)

target_link_libraries(common_tests
    libcommon_host
    gtest gtest_main
    pthread
)

add_test(
    NAME common_tests
    COMMAND common_tests
)
