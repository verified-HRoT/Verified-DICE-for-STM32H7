#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

# This only runs the first time CMake is invoked. To refresh the value it is
# needed to clean the build output and rebuild.
execute_process(
    COMMAND git describe --tags --dirty --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_DESCRIPTION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    RESULT_VARIABLE GIT_COMMIT_DESCRIPTION_RESULT
)

if(NOT ${GIT_COMMIT_DESCRIPTION_RESULT} EQUAL 0)
    set(GIT_COMMIT_DESCRIPTION "00000000")
endif()

# This timestamp string should be used instead of __TIME__ and __DATE__. It can
# be set to any arbitrary value by setting the environment variable
# SOURCE_DATE_EPOCH to a known value. Check the following link for more
# information: https://reproducible-builds.org/docs/source-date-epoch/
string(TIMESTAMP TIMESTAMP_STRING)

# This macro adds the definitions to the given target
macro(add_version_definitions target)
    # Add the definitions strings
    target_compile_definitions(${target}
        PRIVATE
            -DGIT_COMMIT_DESCRIPTION=\"${GIT_COMMIT_DESCRIPTION}\"
            -DTIMESTAMP_STRING=\"${TIMESTAMP_STRING}\"
    )
endmacro()
