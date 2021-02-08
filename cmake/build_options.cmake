#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

# firmware build options
# --------------------------

option(USE_NUCLEO2_BOARDS
    "Build for NUCLEO-H743ZI2 boards rather than NUCLEO-H743ZI"
    ON
)

option(HW_ENABLE_WATCHDOG
    "Enable hardware and software watchdogs"
    ON
)

option(USE_HW_RNG
    "Use hardware random number generator instead of rand()"
    OFF
)

option(HW_IGNORE_PROTECTION_CHECKS
    "Let DICE ignore all protection checks and boot L0 and L1 anyway."
    ON
)

option(HW_SET_RSS_PROTECTIONS_AT_RUNTIME
    "Set to ON to set up the Secure User Area from DICE during boot."
    OFF
)

# Add a build option called HW_MIN_LOG_LEVEL with only a set of valid values.
# Then, check that the specified value is a valid one.

set(HW_MIN_LOG_LEVEL "LOG_LEVEL_TRACE" CACHE STRING
    "Min level of messages compiled in the ARM firmware binaries.")

set(HW_LOG_LEVELS
    LOG_LEVEL_TRACE LOG_LEVEL_SECRET LOG_LEVEL_DEBUG
    LOG_LEVEL_INFO LOG_LEVEL_FAIL LOG_LEVEL_FATAL
)

set_property(CACHE HW_MIN_LOG_LEVEL PROPERTY STRINGS ${HW_LOG_LEVELS})

list(FIND HW_LOG_LEVELS ${HW_MIN_LOG_LEVEL} LEVEL_INDEX)
if(LEVEL_INDEX EQUAL -1)
    message(FATAL_ERROR "HW_MIN_LOG_LEVEL must be one of ${HW_LOG_LEVELS}")
endif()

add_definitions(-DHW_MIN_LOG_LEVEL=${HW_MIN_LOG_LEVEL})
