/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include <stdint.h>

EXTERNAL_C_BEGIN

// Seed RNG
int RNG_Seed(uint64_t a, uint64_t b);

// Fill buffer with 'size' bytes of random numbers
int RNG_RandCtx(void *ctx, unsigned char *buffer, size_t size);
int RNG_Rand(uint8_t *buffer, size_t size);

EXTERNAL_C_END
