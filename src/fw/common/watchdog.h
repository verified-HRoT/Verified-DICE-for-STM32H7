/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"
#include "global/timeouts.h"

EXTERNAL_C_BEGIN

// Software watchdog
// -----------------

// Initializes the SW watchdog. Returns 0 on success. Make sure to disable it
// before leaving the current image.
int SW_Watchdog_Init(void);

// This has to be invoked every now and then to reload the IWDG counter
void SW_Watchdog_Reload(void);

// Disables the SW watchdog. Returns 0 on success.
int SW_Watchdog_End(void);

// Hardware watchdog
// -----------------

// Initializes the IWDG watchdog. Returns 0 on success. Note that the watchdog
// stays active even after leaving the current firmware image.
int HW_Watchdog_Init(void);

// This has to be invoked every now and then to reload the IWDG counter
void HW_Watchdog_Reload(void);

// Combined functions
// ------------------

// This has to be invoked every now and then to reload the watchdog counters
static inline void Watchdog_Reload(void) {
  SW_Watchdog_Reload();
  HW_Watchdog_Reload();
}

EXTERNAL_C_END
