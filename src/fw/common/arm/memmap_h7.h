/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>

// RAM definitions
// ===============

// RAM used to share information between images

// SRAM4 (0x38000000 - 0x3800FFFF) (64K)
#define SRAM4_START (0x38000000U)
#define SRAM4_LENGTH (64 * 1024)

// Backup SRAM (0x38800000 - 0x38800FFF) (4K)
#define BACKUP_SRAM_START (0x38800000U)
#define BACKUP_SRAM_LENGTH (4 * 1024)

// SRAM2 + SRAM3 (0x30020000 - 0x30047FFF) (160K)
#define SRAM23_START (0x30020000)
#define SRAM23_LENGTH (160 * 1024)

// Flash definitions
// =================

// Firmware images slots
extern uint32_t __REGION_FW_ACTIVE_START__;
#define REGION_FW_ACTIVE_START ((uint32_t)&__REGION_FW_ACTIVE_START__)
extern uint32_t __REGION_FW_ACTIVE_END__;
#define REGION_FW_ACTIVE_END ((uint32_t)&__REGION_FW_ACTIVE_END__)

// Actual location in flash of images DICE, L0 and L1
extern uint32_t __DICE_IMAGE_START__;
#define DICE_IMAGE_START ((uint32_t)&__DICE_IMAGE_START__)
extern uint32_t __DICE_IMAGE_END__;
#define DICE_IMAGE_END ((uint32_t)&__DICE_IMAGE_END__)
extern uint32_t __KEY_MANIFEST_START__;
#define KEY_MANIFEST_START ((uint32_t)&__KEY_MANIFEST_START__)
extern uint32_t __KEY_MANIFEST_END__;
#define KEY_MANIFEST_END ((uint32_t)&__KEY_MANIFEST_END__)
extern uint32_t __L0_IMAGE_START__;
#define L0_IMAGE_START ((uint32_t)&__L0_IMAGE_START__)
extern uint32_t __L0_IMAGE_END__;
#define L0_IMAGE_END ((uint32_t)&__L0_IMAGE_END__)
extern uint32_t __L1_IMAGE_START__;
#define L1_IMAGE_START ((uint32_t)&__L1_IMAGE_START__)
extern uint32_t __L1_IMAGE_END__;
#define L1_IMAGE_END ((uint32_t)&__L1_IMAGE_END__)
