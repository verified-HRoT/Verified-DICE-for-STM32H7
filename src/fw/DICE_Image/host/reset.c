/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "fw/DICE_Image/reset.h"

static reset_reason last_reset_reason = RESET_REASON_UNKNOWN;

void ResetReason_Update(void) {
  // Nothing to do
}

reset_reason ResetReason_Get(void) { return last_reset_reason; }

void ResetReason_Set(reset_reason reason) { last_reset_reason = reason; }
