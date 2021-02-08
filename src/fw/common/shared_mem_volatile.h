/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include "memmap.h"

EXTERNAL_C_BEGIN

// Non-persistent shared RAM
// -------------------------

// Located in SRAM4 (64K)
//
// - DICE->L0 information is stored in the first 32K. It must be cleared before
//   jumping to L1.
// - L0->L1 information is stored in the second 32 K

static inline void *GetShared_DICE_L0(void) { return GetSharedRamStart(); }

static inline void *GetShared_L0_L1(void) {
  uintptr_t start = (uintptr_t)GetSharedRamStart();
  uintptr_t middle = start + (GetSharedRamSize() / 2);
  return (void *)middle;
}

// This clears the memory region reserved for DICE to L0 sharing of data
static inline void CleanShared_DICE_L0(void) {
  platform_zeroize(GetShared_DICE_L0(), GetSharedRamSize() / 2);
}

// This clears the memory region reserved for L0 to L1 sharing of data
static inline void CleanShared_L0_L1(void) {
  platform_zeroize(GetShared_L0_L1(), GetSharedRamSize() / 2);
}

EXTERNAL_C_END
