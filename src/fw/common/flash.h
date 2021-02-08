/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "common/machine.h"

EXTERNAL_C_BEGIN

// Size of a flash word in the H7 in bytes (256 bits). In common code shared
// between host and H7 this value can't be used directly, use the function
// Flash_GetWordSize() instead.
#define FLASH_WORD_SIZE (256 / 8)

// Helper that returns the flash word size, that is, the minimum size that can
// be written to flash in one go.
uint32_t Flash_GetWordSize(void);

// Helper that returns the flash sector size, that is, the minimum size that can
// be erased from flash.
uint32_t Flash_GetSectorSize(void);

// Returns 1 if the provided address is a valid flash address, 0 if not.
int Flash_IsAddressValid(uintptr_t address);

// Write data to flash. Size must be a multiple of FLASH_WORD_SIZE and the
// destination must be also aligned to it.
int Flash_Write(void *dest, const void *source, size_t size);

// Erase flash. There are 2 restrictions:
// - This can only erase sectors.
// - All the memory must be in the same bank.
int Flash_Erase(void *address, size_t size);

EXTERNAL_C_END
