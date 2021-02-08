/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common/rng.h"
#include "global/crypto_defs.h"

#include "Hacl_Lib.h"
#include "crypto.h"

int Crypto_Hash(digest *result, void *data, size_t dataSize) {
  Hacl_Hash_SHA2_hash_256((uint8_t *)data, (uint32_t)dataSize, result->data);
  return 0;
}
