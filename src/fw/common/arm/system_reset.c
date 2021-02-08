/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_rcc.h>

#include "common/logger.h"
#include "fw/common/arm/backup_sram.h"
#include "fw/common/system_reset.h"

void System_Reset(system_reset_type resetType) {
  if (resetType == HARDWARE_RESET) {
    HW_LOG_DEBUG("Hardware reset");
  } else {
    HW_LOG_DEBUG("Software reset");
  }

  // Prepare to reset
  Backup_SRAM_Disable();

  // Try to reset
  if (resetType == HARDWARE_RESET) {
    NVIC_SystemReset();
  } else {
    // https://github.com/STMicroelectronics/STM32CubeH7/issues/19
    //
    // __HAL_RCC_AHB3_FORCE_RESET() used to trigger a software reset because
    // it writes to reserved bits in the register. After an update of the
    // STM HAL (to Release v1.7.0) this behaviour has been fixed. The
    // following line writes to the reserved bit that triggers the reset:
    RCC->AHB3RSTR = 0xFFFFFFFFU;
  }

  // This code shouldn't be reached
  Backup_SRAM_Enable();
  HW_LOG_FAIL("%s failed!", __func__);
  Backup_SRAM_Disable();

  // Wait until the watchdog resets the device
  while (1)
    ;
}
