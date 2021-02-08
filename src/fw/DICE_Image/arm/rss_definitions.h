/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>

// One area per flash bank
#define RSS_NUM_SECURE_AREAS (2)

typedef struct __attribute__((packed)) {
  // If size is 0, the area is unused. Size can go from 512 bytes to the full
  // bank with 256-bytes granularity.
  uint32_t size;
  // Pass NULL for an empty area
  uint32_t address;
  // If 0, during a bank or mass erase the contents of the area will be
  // preserved. If not 0, they will be deleted.
  uint32_t remove_during_bank_erase;
} rss_secure_area_t;

typedef struct __attribute__((packed)) {
  // Exit secure user execution mode and jump to non-secure code
  void (*exitSecureArea)(uint32_t vectors);

  // Setup secure areas. A system reset is triggered after it completes
  void (*resetAndInitializeSecureAreas)(uint32_t num, rss_secure_area_t *areas);
} rss_table_t;

// Location of the function pointer table of RSS services
#define RSS_TABLE ((rss_table_t *)0x1FF09514)
