/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stddef.h>
#include <stdint.h>

#include <stm32h7xx_hal.h>

#include "fw/common/arm/memmap_h7.h"
#include "fw/common/memmap.h"

// RAM

void *GetSharedRamStart(void) { return (void *)SRAM4_START; }

size_t GetSharedRamSize(void) { return SRAM4_LENGTH; }

void *GetPersistentRamStart(void) { return (void *)BACKUP_SRAM_START; }

size_t GetPersistentRamSize(void) { return BACKUP_SRAM_LENGTH; }

void *GetLogRamStart(void) { return (void *)SRAM23_START; }

size_t GetLogRamSize(void) { return SRAM23_LENGTH; }

// Flash

void *GetFlashStart(void) { return (void *)FLASH_BANK1_BASE; }

void *GetFlashEnd(void) { return (void *)FLASH_END; }

void *GetFwActiveStart(void) { return (void *)REGION_FW_ACTIVE_START; }

size_t GetFwSlotSize(void) { return FW_SLOT_SIZE; }

void *GetDICEImageStart(void) { return (void *)DICE_IMAGE_START; }

void *GetDICEImageEnd(void) { return (void *)DICE_IMAGE_END; }

void *GetKeyManifestStart(void) { return (void *)KEY_MANIFEST_START; }

void *GetKeyManifestEnd(void) { return (void *)KEY_MANIFEST_END; }

void *GetL0ImageStart(void) { return (void *)L0_IMAGE_START; }

void *GetL0ImageEnd(void) { return (void *)L0_IMAGE_END; }

void *GetL1ImageStart(void) { return (void *)L1_IMAGE_START; }

void *GetL1ImageEnd(void) { return (void *)L1_IMAGE_END; }
