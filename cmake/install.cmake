#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

# Set default install path to the build directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/" CACHE PATH
            "Default install path" FORCE )
endif()

if (NOT WIN32)
    # Set relative RPATH so that the binaries can be run from any location as
    # long as the install tree is intact.
    #
    # This needs to be set before any add_executable() or add_library().
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()
