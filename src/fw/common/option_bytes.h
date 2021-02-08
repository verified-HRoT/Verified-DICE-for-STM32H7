/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdint.h>

#pragma once

// There are 8 32-bit registers that we include in the measurement
#define OPTION_BYTES_SIZE (sizeof(uint32_t) * 8)

typedef struct {
  uint8_t data[OPTION_BYTES_SIZE];
} option_bytes;

// This function reads the option bytes of the H7 and returns them as an array
// of bytes. This is only meant to be used as input to a hash function or
// something similar, it's not meant to be used as actual information.
void ReadOptionBytes(option_bytes *ob);
