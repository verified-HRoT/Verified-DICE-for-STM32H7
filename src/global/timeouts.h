/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <assert.h>

#define TIMEOUT_HW_WATCHDOG_SECONDS (8)

#define TIMEOUT_SW_WATCHDOG_SECONDS (3)

// The SW watchdog timeout should be much shorter than the HW watchdog
static_assert(TIMEOUT_HW_WATCHDOG_SECONDS > (2 * TIMEOUT_SW_WATCHDOG_SECONDS),
              "Invalid timeouts");
