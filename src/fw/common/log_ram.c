/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "fw/common/log_ram.h"

static log_ram_t *GetLogRam(void) { return (log_ram_t *)GetLogRamStart(); }

void CleanLogRam(void) { platform_zeroize(GetLogRamStart(), GetLogRamSize()); }

void LogToMemory(const char *str) {
  log_ram_t *ram = GetLogRam();

  const size_t max_log_size = sizeof(ram->log);

  // Make sure that the values are valid. If they are not, reset them
  if ((ram->log_write_ptr >= max_log_size) || (ram->log_size > max_log_size)) {
    ram->log_write_ptr = 0;
    ram->log_size = 0;
  }

  while (1) {
    const char c = *str++;
    if (c == '\0')
      break;

    ram->log[ram->log_write_ptr] = c;

    ram->log_write_ptr++;

    if (ram->log_write_ptr >= max_log_size)
      ram->log_write_ptr = 0;

    if (ram->log_size < max_log_size)
      ram->log_size++;
  }
}

// This function checks the circular buffer and returns the contents in two
// parts sorted in the right order.
static void GetMemoryLogParts(const char **str1, size_t *size1,
                              const char **str2, size_t *size2) {
  log_ram_t *ram = GetLogRam();

  const size_t max_log_size = sizeof(ram->log);

  if (ram->log_size < max_log_size) {
    *str1 = &(ram->log[0]);
    *size1 = ram->log_size;
    *str2 = NULL;
    *size2 = 0;
  } else {
    *str1 = &(ram->log[ram->log_write_ptr]);
    *size1 = max_log_size - ram->log_write_ptr;
    *str2 = &(ram->log[0]);
    *size2 = ram->log_write_ptr;
  }
}

void GetMemoryLog(const char **str, size_t *size) {
  log_ram_t *ram = GetLogRam();

  const char *str1, *str2;
  size_t size1, size2;
  GetMemoryLogParts(&str1, &size1, &str2, &size2);

  if (size1 > 0)
    memcpy(&(ram->reconstructed_log[0]), str1, size1);

  if (size2 > 0)
    memcpy(&(ram->reconstructed_log[size1]), str2, size2);

  ram->reconstructed_log[size1 + size2] = '\0';

  *str = &(ram->reconstructed_log[0]);
  *size = size1 + size2 + 1;
}

void ResetMemoryLog(void) {
  log_ram_t *ram = GetLogRam();

  ram->log_size = 0;
  ram->log_write_ptr = 0;
}
