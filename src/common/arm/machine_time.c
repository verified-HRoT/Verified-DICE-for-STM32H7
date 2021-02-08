/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/timing.h"
#include "fw/common/arm/system_setup.h"
#include "fw/common/watchdog.h"

TimeMsType GetTimeNow(void) { return System_Time(); }

void ResetWatchdogTimer(void) { Watchdog_Reload(); }
