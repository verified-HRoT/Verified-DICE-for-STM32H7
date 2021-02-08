/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stddef.h>
#include <stdint.h>

#include "common/xoroshiro128plus.h"

int RNG_Seed(uint64_t a, uint64_t b) {
  xoroshiro128plus_seed(a, b);

  return 0;
}

int RNG_Rand(uint8_t *buffer, size_t size) {
  // TODO: This can be optimized
  for (size_t i = 0; i < size; i++)
    *buffer++ = (uint8_t)(xoroshiro128plus_rand() >> 48);

  return 0;
}

int RNG_RandCtx(void *ctx, unsigned char *buffer, size_t size) {
  (void)ctx;

  return RNG_Rand(buffer, size);
}
