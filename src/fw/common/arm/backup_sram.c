/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#include "common/logger.h"

#include "memmap_h7.h"

void Backup_SRAM_Enable(void) {
  // Reset backup Domain. This doesn't affect the backup SRAM.
  __HAL_RCC_BACKUPRESET_FORCE();
  __DMB();
  __DSB();

  __HAL_RCC_BACKUPRESET_RELEASE();
  __DMB();
  __DSB();

  // Enable backup SRAM clock
  __HAL_RCC_BKPRAM_CLK_ENABLE();
  __DMB();
  __DSB();

  // Enable backup SRAM access. Enables access to backup regulator registers
  HAL_PWR_EnableBkUpAccess();

  // Enable backup regulator
  if (HAL_PWREx_EnableBkUpReg() != HAL_OK) {
    HW_LOG_FAIL("HAL_PWREx_EnableBkUpReg() failed");
  }
}

void Backup_SRAM_Disable(void) {
  // Make sure that the backup SRAM cache is clean before disabling it
  SCB_CleanDCache_by_Addr((uint32_t *)BACKUP_SRAM_START, BACKUP_SRAM_LENGTH);

  if (HAL_PWREx_DisableBkUpReg() != HAL_OK) {
    HW_LOG_FAIL("HAL_PWREx_DisableBkUpReg() failed");
  }

  HAL_PWR_DisableBkUpAccess();

  __HAL_RCC_BKPRAM_CLK_DISABLE();

  __HAL_RCC_BACKUPRESET_FORCE();
  __DMB();
  __DSB();
}
