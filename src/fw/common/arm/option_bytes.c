/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stm32h7xx_hal.h>

#include "fw/common/option_bytes.h"

void ReadOptionBytes(option_bytes *ob) {
  uint32_t *dst = (uint32_t *)&(ob->data[0]);

  // FOPTSR_PRG has some fields that don't matter for security

  // Bits that we don't care about in this register:
  const uint32_t ignored_mask = 0x3FD900EF;

  // Read register and mask out the fields we don't care about
  const uint32_t foptsr_prg_masked = FLASH->OPTSR_CUR & ~ignored_mask;

  *dst++ = foptsr_prg_masked;
  *dst++ = FLASH->PRAR_CUR1;
  *dst++ = FLASH->PRAR_CUR2;
  *dst++ = FLASH->SCAR_CUR1;
  *dst++ = FLASH->SCAR_CUR2;
  *dst++ = FLASH->WPSN_CUR1;
  *dst++ = FLASH->WPSN_CUR2;
  *dst++ = FLASH->BOOT_CUR;
}
