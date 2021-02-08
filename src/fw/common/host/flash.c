/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <string.h>

#include "common/logger.h"
#include "fw/common/flash.h"
#include "fw/common/memmap.h"

uint32_t Flash_GetWordSize(void) { return FLASH_WORD_SIZE; }

uint32_t Flash_GetSectorSize(void) {
  return 128 * 1024; // Simulate H7
}

int Flash_IsAddressValid(uintptr_t address) {
  if (address < (uintptr_t)GetFlashStart())
    return 0;

  if (address > (uintptr_t)GetFlashEnd())
    return 0;

  return 1;
}

int Flash_Write(void *dest, const void *source, size_t size) {
  if (!source || !dest)
    return -1;

  if (size % Flash_GetWordSize() != 0)
    return -1;

  uintptr_t src = (uintptr_t)source;
  uintptr_t dst = (uintptr_t)dest;

  // Check for overflows
  if (((src + size) < src) || ((dst + size) < dst) ||
      ((size + FLASH_WORD_SIZE) < size))
    return -1;

  const uint8_t *s = source;
  uint8_t *d = dest;

  for (size_t i = 0; i < size; i++) {
    *d = ~((~*d) | (~*s)); // Bits can only go from 1 to 0 (so we OR the zeroes)
    d++;
    s++;
  }

  return 0;
}

int Flash_Erase(void *address, size_t size) {
  if (!address)
    return -1;

  if (size % Flash_GetSectorSize() != 0)
    return -1;

  uintptr_t addr = (uintptr_t)address;

  if (((addr + size) < addr) || ((size + FLASH_WORD_SIZE) < size))
    return -1;

  HW_LOG_INFO("Erasing: 0x%08lX-0x%08lX", addr, addr + size - 1);

  // Reset value is 0xFF
  memset(address, 0xFF, size);

  return 0;
}
