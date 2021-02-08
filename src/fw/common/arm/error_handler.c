/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdint.h>

#include <stm32h7xx_hal.h>

#include "common/logger.h"

// This function is used by the HAL when USE_FULL_ASSERT == 1
_Noreturn void assert_failed(uint8_t *file, uint32_t line) {
  HW_LOG_FAIL("%s:%lu\n", (char *)file, line);

  // Infinite loop

  while (1)
    ;
}

void MemManage_Handler(void) {
  HW_LOG_FAIL("Exception");
  HAL_Delay(1000);
  NVIC_SystemReset();
}

void HardFault_Handler(void) {
  HW_LOG_FAIL("Exception");
  HAL_Delay(1000);
  NVIC_SystemReset();
}
