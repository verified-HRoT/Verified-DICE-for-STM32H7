/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#include "fw/DICE_Image/reset.h"

static reset_reason last_reset_reason = RESET_REASON_UNKNOWN;

// Find the reason for the last reset and save it. Then, clear all reset-related
// flags.
void ResetReason_Update(void) {
  if ((__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST) != RESET) ||
      (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST) != RESET)) {
    // Watchdog exception
    last_reset_reason = RESET_REASON_WATCHDOG;
  } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWR1RST) != RESET) {
    // Low Power reset
    last_reset_reason = RESET_REASON_LOW_POWER;
  } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
    // SW reset
    last_reset_reason = RESET_REASON_SW_RESET;
  } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
    // HW reset
    last_reset_reason = RESET_REASON_HW_RESET;
  } else {
    // Unknown
    last_reset_reason = RESET_REASON_UNKNOWN;
  }

  // It is needed to clear the reset flags or they will persist over most
  // resets, hiding the reason for the most recent one.
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

reset_reason ResetReason_Get(void) { return last_reset_reason; }

void ResetReason_Set(reset_reason reason) {
  // Empty, this is only used for testing in the host
  (void)reason;
}
