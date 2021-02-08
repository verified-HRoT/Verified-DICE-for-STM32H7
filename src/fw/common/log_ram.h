/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include "memmap.h"

EXTERNAL_C_BEGIN

// We need to hold two copies of the log in RAM. The first one is the circular
// buffer where the log is actually written. The second one is the reconstructed
// version of the log, which needs to be put together in the right order to be
// sent to the host.
#define LOG_SIZE                                                               \
  (((LOG_RAM_SIZE - sizeof(uint32_t) - sizeof(uint32_t)) / 2) - 1)

#pragma pack(push, 1)
typedef struct {
  // Space for persistent logs
  uint32_t log_write_ptr;
  uint32_t log_size;
  char log[LOG_SIZE];
  // Space to reconstruct logs before sending them to the host. It's better to
  // have a static array than to allocate it in the stack because it's quite
  // big and it wouldn't fit.
  char reconstructed_log[LOG_SIZE];
} log_ram_t;
#pragma pack(pop)

static_assert(sizeof(log_ram_t) < LOG_RAM_SIZE, "Incorrect struct size");

void CleanLogRam(void);

// Log string to persistent memory.
void LogToMemory(const char *str);

// This function copies the current circular buffer state to a static buffer and
// returns the static buffer and the total size of the log in it. The size
// includes a null terminator (so the minimum returned size is 1).
void GetMemoryLog(const char **str, size_t *size);

// Empty the circular buffer. It doesn't clear the static buffer returned by
// GetPersistentMemoryLog().
void ResetMemoryLog(void);

EXTERNAL_C_END
