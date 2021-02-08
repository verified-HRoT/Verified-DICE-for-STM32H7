#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

set_toolchain_host()

search_source_files(KEYTOOL_SOURCES)

add_executable(keytool
    ${KEYTOOL_SOURCES}
)

target_link_libraries(keytool
    crypto_host
)
