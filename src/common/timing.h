/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "machine.h"

#include <stdint.h>

// Time interfaces are common to all platforms for now

typedef uint64_t TimeMsType;
typedef uint64_t TimeDeltaMsType;

// the invalid time
#define INVALID_TIME_MS 0

EXTERNAL_C TimeMsType GetTimeNow(void);
EXTERNAL_C void ResetWatchdogTimer(void);
