/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <stdint.h>

#include "common/machine.h"
#include "crypto/ImageSigning.h"

EXTERNAL_C_BEGIN

typedef enum {
  VERIFY_SUCCESS = 0,
  VERIFY_ERROR_MAGIC = -1,
  VERIFY_ERROR_VERSION = -2,
  VERIFY_ERROR_SIZE = -3,
  VERIFY_ERROR_HASH = -4,
  VERIFY_ERROR_SIGNATURE = -5
} verify_status;

// Verifies the image at the provided address and returns a VERIFY_* error code.
verify_status VerifyImage(const void *addr, const fw_pub_key *key);

// Verifies the key manifest at the provided address and returns a VERIFY_*
// error code.
verify_status VerifyKeyManifest(const void *addr, const fw_pub_key *key);

EXTERNAL_C_END
