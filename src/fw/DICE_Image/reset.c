/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "fw/DICE_Image/reset.h"

const char *ResetReason_GetString(reset_reason reason) {
  switch (reason) {
  default:
  case RESET_REASON_UNKNOWN:
    return "Unknown";
  case RESET_REASON_WATCHDOG:
    return "Watchdog";
  case RESET_REASON_LOW_POWER:
    return "Low Power";
  case RESET_REASON_SW_RESET:
    return "Software Reset";
  case RESET_REASON_HW_RESET:
    return "Hardware Reset";
  }
}
