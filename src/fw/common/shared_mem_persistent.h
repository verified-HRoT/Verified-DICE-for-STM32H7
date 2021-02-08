/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"
#include "crypto/crypto.h"

#include "memmap.h"

EXTERNAL_C_BEGIN

// Struct that represent the usage of persistent RAM

#define PERSISTENT_LOG_SIZE                                                    \
  (PERSISTENT_SHARED_SRAM_SIZE - sizeof(uint32_t) - sizeof(uint32_t))

#pragma pack(push, 1)
typedef struct {
  // Space for persistent logs
  uint32_t log_write_ptr;
  uint32_t log_size;
  char log[PERSISTENT_LOG_SIZE];
} persistent_shared_ram_t;
#pragma pack(pop)

static_assert(sizeof(persistent_shared_ram_t) == PERSISTENT_SHARED_SRAM_SIZE,
              "Incorrect struct size");

// Persistent shared RAM
// ---------------------

persistent_shared_ram_t *GetShared_Persistent(void);
void CleanShared_Persistent(void);

// Persistent shared RAM logging
// -----------------------------

// Log string to persistent memory.
void LogToPersistentMemory(const char *str);

// This function copies the current circular buffer state to a static buffer and
// returns the static buffer and the total size of the log in it. The size
// includes a null terminator (so the minimum returned size is 1).
void GetPersistentMemoryLog(const char **str, size_t *size);

// Empty the circular buffer. It doesn't clear the static buffer returned by
// GetPersistentMemoryLog().
void ResetPersistentMemoryLog(void);

EXTERNAL_C_END
