/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef USE_HW_RNG
#include "fw/common/arm/hw_rng.h"
#endif
#include "common/xoroshiro128plus.h"

int RNG_Seed(uint64_t a, uint64_t b) {
#ifdef USE_HW_RNG
  (void)a;
  (void)b;

  return 0;
#else
  xoroshiro128plus_seed(a, b);

  return 0;
#endif
}

int RNG_Rand(unsigned char *buffer, size_t size) {
#ifdef USE_HW_RNG
  return HW_RNG_Rand(buffer, size);
#else
  // TODO: This can be optimized
  for (size_t i = 0; i < size; i++)
    *buffer++ = (uint8_t)(xoroshiro128plus_rand() >> 48);

  return 0;
#endif
}

int RNG_RandCtx(void *ctx, unsigned char *buffer, size_t size) {
  (void)ctx;

  return RNG_Rand(buffer, size);
}
