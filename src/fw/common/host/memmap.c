/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stddef.h>
#include <stdint.h>

#include "fw/common/memmap.h"

// RAM banks
static unsigned char shared_ram[VOLATILE_SHARED_SRAM_SIZE];
static unsigned char persistent_ram[PERSISTENT_SHARED_SRAM_SIZE];
static unsigned char log_ram[LOG_RAM_SIZE];

// 2 MB of flash
static unsigned char flash[TOTAL_FLASH_SIZE];

// RAM

void *GetSharedRamStart(void) { return (void *)shared_ram; }

size_t GetSharedRamSize(void) { return sizeof(shared_ram); }

void *GetPersistentRamStart(void) { return (void *)persistent_ram; }

size_t GetPersistentRamSize(void) { return sizeof(persistent_ram); }

void *GetLogRamStart(void) { return (void *)log_ram; }

size_t GetLogRamSize(void) { return sizeof(log_ram); }

// Flash

void *GetFlashStart(void) { return (void *)flash; }

void *GetFlashEnd(void) {
  return (void *)((uintptr_t)flash + sizeof(flash) - 1);
}

void *GetFwActiveStart(void) { return (void *)&(flash[DICE_TOTAL_SIZE]); }

size_t GetFwSlotSize(void) { return FW_SLOT_SIZE; }

void *GetDICEImageStart(void) { return (void *)flash; }

void *GetKeyManifestStart(void) { return GetFwActiveStart(); }

void *GetL0ImageStart(void) {
  return (void *)((uintptr_t)GetFwActiveStart() + KEY_MANIFEST_SIZE);
}

void *GetL1ImageStart(void) {
  return (void *)((uintptr_t)GetFwActiveStart() + KEY_MANIFEST_SIZE +
                  L0_TOTAL_SIZE);
}
