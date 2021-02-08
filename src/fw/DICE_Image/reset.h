/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

EXTERNAL_C_BEGIN

typedef enum {
  RESET_REASON_UNKNOWN = 0,
  RESET_REASON_WATCHDOG = 1,
  RESET_REASON_LOW_POWER = 2,
  RESET_REASON_SW_RESET = 3,
  RESET_REASON_HW_RESET = 4
} reset_reason;

// This function reads the reset reason and saves it to a static variable. This
// is needed because whenever the reset reason is set by hardware, it is never
// reset (except during a hardware reset). If we want to know the actual reset
// reason, we need to clear it in every boot after checking it.
void ResetReason_Update(void);

// This returns the value of the static variable that holds the reset reason.
reset_reason ResetReason_Get(void);

// This function sets the value of the static variable. It shouldn't be used
// under normal circumstances. This is only used for testing, and only works in
// the host port of the firmware. It isn't implemented in the Arm port.
void ResetReason_Set(reset_reason reason);

// This returns a string that describes the reset reason so that it can be used
// in debug prints.
const char *ResetReason_GetString(reset_reason reason);

EXTERNAL_C_END
