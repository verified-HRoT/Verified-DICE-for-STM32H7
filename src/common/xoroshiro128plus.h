/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include <stdint.h>

// This implementation is thread-safe: it has a different context per thread

EXTERNAL_C_BEGIN

// Returns a 64-bit random number. It is preferable to discard low bits than
// high bits.
uint64_t xoroshiro128plus_rand(void);

// Seed the RNG. Both a and b should never be 0 at the same time.
void xoroshiro128plus_seed(uint64_t a, uint64_t b);

EXTERNAL_C_END
