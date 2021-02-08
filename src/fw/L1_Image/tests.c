/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "common/logger.h"
#include "fw/common/flash.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/shared_mem_volatile.h"

int Test_ReadDICEImage(void) {
  HW_LOG_DEBUG("TEST: Read DICE Image");

  uintptr_t address = (uintptr_t)GetDICEImageStart();
  uint8_t value;
  int value_read = 0;

  // If the RSS protections are set correctly, reading should return just zero

  HW_LOG_TRACE("Reading 256 bytes at 0x%08lX", address);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 32; j++) {
      value = *(uint8_t *)address++;
      if (value != 0)
        value_read = 1;
    }
  }

  // If the Secure User Memory is set up, DICE can't be read. If it hasn't been
  // set up, it can be read.

  if (value_read) {
    HW_LOG_DEBUG("    PROTECTION NOT SET!");
  } else {
    HW_LOG_DEBUG("    PROTECTION SET!");
  }

  // This has to be inspected manually, return success
  return 0;
}

int Test_ReadKeyManifest(void) {
  HW_LOG_DEBUG("TEST: Read key manifest");

  uintptr_t address = (uintptr_t)GetKeyManifestStart();
  uint8_t value;
  int value_read = 0;

  // If the RSS protections are set correctly, reading should return just zero

  HW_LOG_TRACE("Reading 256 bytes at 0x%08lX", address);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 32; j++) {
      value = *(uint8_t *)address++;
      if (value != 0)
        value_read = 1;
    }
  }

  // If the Secure User Memory is set up, the memory can't be read. If it
  // hasn't been set up, it can be read.

  if (value_read) {
    HW_LOG_DEBUG("    PROTECTION NOT SET!");
  } else {
    HW_LOG_DEBUG("    PROTECTION SET!");
  }

  // This has to be inspected manually, return success
  return 0;
}

int Test_ReadDICEL0SharedMemory(void) {
  HW_LOG_DEBUG("TEST: Read DICE->L0 shared memory");

  // This should have been cleared by L0 and return zeroes

  uintptr_t address = (uintptr_t)GetShared_DICE_L0();
  size_t size = (uintptr_t)GetShared_L0_L1() - (uintptr_t)GetShared_DICE_L0();
  uint8_t value;
  int value_read = 0;

  HW_LOG_TRACE("Reading 0x%lX bytes at 0x%08lX", size, address);

  for (uint32_t i = 0; i < size; i++) {
    value = *(uint8_t *)address++;
    if (value != 0) {
      value_read = 1;
      break;
    }
  }

  if (value_read) {
    HW_LOG_DEBUG("    FAIL!");
    return -1;
  } else {
    HW_LOG_DEBUG("    SUCCESS!");
    return 0;
  }
}
