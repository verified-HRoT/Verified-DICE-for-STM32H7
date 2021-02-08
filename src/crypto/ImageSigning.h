/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>

#include "common/machine.h"
#include "global/crypto_defs.h"

typedef struct {
  uint8_t data[FW_ED25519_PUBLIC_KEY_LEN];
} fw_pub_key;

typedef struct {
  uint8_t data[FW_ED25519_PRIVATE_KEY_LEN];
} fw_priv_key;

typedef struct {
  uint8_t data[FW_ED25519_SIGNATURE_LEN];
} fw_signature;

// Generate signature for the provided data using the provided private key.
EXTERNAL_C int ED25519_Sign(fw_signature *sig, const void *data,
                            size_t dataSize, const fw_priv_key *key);

// Check that the public key provided can verify that the signature provided is
// valid for the provided data.
EXTERNAL_C int ED25519_Verify(const fw_signature *sig, const void *data,
                              size_t dataSize, const fw_pub_key *key);
