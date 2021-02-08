/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <assert.h>
#include <stddef.h>

#include "common/machine.h"

#include "flash.h"
#include "image_header.h"

EXTERNAL_C_BEGIN

// Space reserved after L1 for firmware update flags
#define FW_UPDATE_FLAGS_SIZE (32 * 3) // 3 flash words

static_assert(FW_UPDATE_FLAGS_SIZE % FLASH_WORD_SIZE == 0, "Invalid size");

// Definitions used when the size has to be resolved at compile time
#define DICE_TOTAL_SIZE (128 * 1024)
#define DICE_IMAGE_SIZE DICE_TOTAL_SIZE
#define KEY_MANIFEST_SIZE (1 * 1024)
#define L0_TOTAL_SIZE (255 * 1024)
#define L0_IMAGE_SIZE (L0_TOTAL_SIZE - IMAGE_HEADER_SIZE)
#define L1_TOTAL_SIZE ((512 * 1024) - FW_UPDATE_FLAGS_SIZE)
#define L1_IMAGE_SIZE (L1_TOTAL_SIZE - IMAGE_HEADER_SIZE)

// Size of a firmware update image (key manifest + L0 + L1)
#define FW_UPDATE_TOTAL_SIZE (KEY_MANIFEST_SIZE + L0_TOTAL_SIZE + L1_TOTAL_SIZE)
// Size of the full firmware image (DICE + update image)
#define FW_COMPLETE_TOTAL_SIZE (DICE_TOTAL_SIZE + FW_UPDATE_TOTAL_SIZE)

// Alias for firmware update image size
#define FW_SLOT_SIZE (FW_UPDATE_TOTAL_SIZE)

#define DICE_IMAGE_OFFSET (0)

// Locations of active and update firmwares
#define FW_ACTIVE_OFFSET (DICE_IMAGE_OFFSET + DICE_TOTAL_SIZE)

// Size of different memory banks
#define TOTAL_FLASH_SIZE (2 * 1024 * 1024)
#define VOLATILE_SHARED_SRAM_SIZE (64 * 1024)

#define PERSISTENT_SHARED_SRAM_SIZE (4 * 1024)
#define LOG_RAM_SIZE (160 * 1024)

// RAM
// ---

void *GetSharedRamStart(void);
size_t GetSharedRamSize(void);

void *GetPersistentRamStart(void);
size_t GetPersistentRamSize(void);

void *GetLogRamStart(void);
size_t GetLogRamSize(void);

// Flash
// -----

// Global flash start and end
void *GetFlashStart(void);
void *GetFlashEnd(void);

// Firmware images slot
void *GetFwActiveStart(void);

// Size of all firmware slots (active, update)
size_t GetFwSlotSize(void);

// Actual location in flash of images DICE, L0 and L1
void *GetDICEImageStart(void);
void *GetDICEImageEnd(void);
void *GetKeyManifestStart(void);
void *GetKeyManifestEnd(void);
void *GetL0ImageStart(void);
void *GetL0ImageEnd(void);
void *GetL1ImageStart(void);
void *GetL1ImageEnd(void);

EXTERNAL_C_END
