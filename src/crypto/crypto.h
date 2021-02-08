/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>
#include <string.h>

#include "common/machine.h"
#include "crypto_config.h"

EXTERNAL_C_BEGIN

// NOTE: All the functions below return 0 on success and an error code on
// failure. The error code is made out of the error code specific to the
// functions of this file, which is in the top 15 bits (sign bit excluded).

// Check src/global/crypto_defs.h for information about the format of the
// public/private keys and the signature.

typedef struct {
  uint8_t data[UDS_LENGTH];
} hw_uds;

typedef struct {
  uint8_t data[ED25519_PUBLIC_KEY_LEN];
} pub_key;

typedef struct {
  uint8_t data[ED25519_PUBLIC_KEY_LEN];
} pub_key_ed25519;

typedef struct {
  uint8_t data[ED25519_PRIVATE_KEY_LEN];
} priv_key;

typedef struct {
  uint8_t data[digest_LENGTH];
} digest;

typedef struct {
  uint8_t data[ED25519_SIGNATURE_LEN];
} signature;

// Calculate hash of the provided data.
int Crypto_Hash(digest *result, void *data, size_t dataSize);

EXTERNAL_C_END
