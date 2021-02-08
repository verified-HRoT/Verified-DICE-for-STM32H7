/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "fw/common/shared_mem_persistent.h"

persistent_shared_ram_t *GetShared_Persistent(void) {
  return (persistent_shared_ram_t *)GetPersistentRamStart();
}

void CleanShared_Persistent(void) {
  platform_zeroize(GetPersistentRamStart(), GetPersistentRamSize());
}

void LogToPersistentMemory(const char *str) {
  persistent_shared_ram_t *ram = GetShared_Persistent();

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
static void GetPersistentMemoryLogParts(const char **str1, size_t *size1,
                                        const char **str2, size_t *size2) {
  persistent_shared_ram_t *ram = GetShared_Persistent();

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

// It's better to have a static array than to allocate it in the stack because
// it's quite big (up to 4 KB in the STM32H7) so it could cause a stack
// overflow.
static char log_copy[PERSISTENT_LOG_SIZE + 1];

void GetPersistentMemoryLog(const char **str, size_t *size) {
  const char *str1, *str2;
  size_t size1, size2;
  GetPersistentMemoryLogParts(&str1, &size1, &str2, &size2);

  if (size1 > 0)
    memcpy(&(log_copy[0]), str1, size1);

  if (size2 > 0)
    memcpy(&(log_copy[size1]), str2, size2);

  log_copy[size1 + size2] = '\0';

  *str = &(log_copy[0]);
  *size = size1 + size2 + 1;
}

void ResetPersistentMemoryLog(void) {
  persistent_shared_ram_t *ram = GetShared_Persistent();

  ram->log_size = 0;
  ram->log_write_ptr = 0;
}
