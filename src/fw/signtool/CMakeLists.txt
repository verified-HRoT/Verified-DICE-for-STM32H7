#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(SIGNTOOL_SOURCES)

add_executable(signtool
    ${SIGNTOOL_SOURCES}
)

target_link_libraries(signtool
    crypto_host
)
