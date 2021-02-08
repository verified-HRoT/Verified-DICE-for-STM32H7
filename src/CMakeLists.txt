#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

add_custom_target(SCRIPTS)
add_custom_target(WHEELS)

add_subdirectory(kremlib)
add_subdirectory(core)
add_subdirectory(fw)
add_subdirectory(common)
add_subdirectory(crypto)
add_subdirectory(global)
