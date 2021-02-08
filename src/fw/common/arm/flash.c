/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <string.h>

#include <stm32h7xx_hal.h>

#include "common/logger.h"
#include "fw/common/flash.h"
#include "fw/common/memmap.h"
#include "fw/common/watchdog.h"

uint32_t Flash_GetWordSize(void) { return FLASH_WORD_SIZE; }

uint32_t Flash_GetSectorSize(void) {
  // Value defined in CMSIS
  return FLASH_SECTOR_SIZE;
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

  uint32_t i = 0;
  uintptr_t src = (uintptr_t)source;
  uintptr_t dst = (uintptr_t)dest;

  // Check for overflows
  if (((src + size) < src) || ((dst + size) < dst) ||
      ((size + FLASH_WORD_SIZE) < size))
    return -1;

  // Check if alignment of destination is correct and if size is a
  // multiple of the flash word.
  if (size % FLASH_WORD_SIZE != 0)
    return -1;
  if (src % sizeof(uint32_t) != 0)
    return -1;
  if (dst % FLASH_WORD_SIZE != 0)
    return -1;

  if (HAL_FLASH_Unlock() != HAL_OK)
    return -2;

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS_BANK1);
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS_BANK2);

  while (i < size) {
    Watchdog_Reload();

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, dst, src) != HAL_OK) {
      HAL_FLASH_Lock();
      return -3;
    }

    if (memcmp((void *)dst, (void *)src, FLASH_WORD_SIZE) != 0) {
      HAL_FLASH_Lock();
      return -4;
    }

    i += FLASH_WORD_SIZE;
    src += FLASH_WORD_SIZE;
    dst += FLASH_WORD_SIZE;
  }

  if (HAL_FLASH_Lock() != HAL_OK)
    return -5;

  return 0;
}

static uintptr_t Get_Actual_Bank(uintptr_t addr) {
  if (READ_BIT(FLASH->OPTCCR, FLASH_OPTCR_SWAP_BANK) == 0) {
    // Banks not swapped
    if (addr < FLASH_BANK2_BASE)
      return FLASH_BANK_1;
    else
      return FLASH_BANK_2;
  } else {
    // Banks swapped
    if (addr < FLASH_BANK2_BASE)
      return FLASH_BANK_2;
    else
      return FLASH_BANK_1;
  }
}

static uintptr_t Get_Sector(uintptr_t addr) {
  // Values defined in CMSIS
  if (addr < FLASH_BANK2_BASE)
    return (addr - FLASH_BANK1_BASE) / Flash_GetSectorSize();
  else
    return (addr - FLASH_BANK2_BASE) / Flash_GetSectorSize();
}

static int Flash_Erase_Sector(uintptr_t addr) {
  if (addr == 0)
    return -1;

  if (HAL_FLASH_Unlock() != HAL_OK)
    return -2;

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS_BANK1);
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS_BANK2);

  uint32_t sector_error = 0;

  FLASH_EraseInitTypeDef EraseInit;
  EraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInit.Banks = Get_Actual_Bank(addr);
  EraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInit.Sector = Get_Sector(addr);
  EraseInit.NbSectors = 1;

  if (HAL_FLASHEx_Erase(&EraseInit, &sector_error) != HAL_OK) {
    HAL_FLASH_Lock();
    return -3;
  }

  if (HAL_FLASH_Lock() != HAL_OK)
    return -4;

  return 0;
}

int Flash_Erase(void *address, size_t size) {
  size_t flash_sector_size = Flash_GetSectorSize();

  if (!address)
    return -1;

  uintptr_t addr = (uintptr_t)address;

  if (((addr + size) < addr) || ((size + FLASH_WORD_SIZE) < size))
    return -1;

  // Check if alignment of address is correct and if size is a
  // multiple of the flash sector.
  if (size % flash_sector_size != 0)
    return -1;
  if (addr % flash_sector_size != 0)
    return -1;

  // Make sure that the memory range is in flash
  if (!Flash_IsAddressValid(addr))
    return -1;

  if (!Flash_IsAddressValid(addr + size - 1))
    return -1;

  HW_LOG_INFO("Erasing: 0x%08lX-0x%08lX", addr, addr + size - 1);

  size_t erasedbytes = 0;

  while (erasedbytes < size) {
    Watchdog_Reload();

    int ret = Flash_Erase_Sector(addr);
    if (ret != 0) {
      HW_LOG_FAIL("Flash_Erase_Sector(): %d", ret);
      return -1;
    }

    addr += flash_sector_size;
    erasedbytes += flash_sector_size;
  }

  return 0;
}

// Flash interrupts handler
void FLASH_IRQHandler(void) { HAL_FLASH_IRQHandler(); }
