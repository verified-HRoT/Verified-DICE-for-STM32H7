/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"
#include "crypto/ImageSigning.h"

EXTERNAL_C_BEGIN

typedef enum {
  FW_AUTH_SUCCESS = 0,
  FW_AUTH_INVALID_MANIFEST = -1,
  FW_AUTH_INVALID_L0 = -2,
  FW_AUTH_INVALID_L1 = -3,
  FW_AUTH_INVALID_L1_OFFSET = -4,
  FW_AUTH_EXTRA_DATA_MANIFEST_L0 = -5,
  FW_AUTH_EXTRA_DATA_L0_L1 = -6,
  FW_AUTH_EXTRA_DATA_AFTER_L1 = -7
} auth_fw_status;

// This function authenticates a package of Key manifest + L0 + L1 images. It
// returns 0 if the authentication has succeeded, an error code if not. It only
// needs the public key of the key manifest because the manifest contains the
// public keys of L0 and L1.
auth_fw_status AuthenticateFirmware(const void *base,
                                    const fw_pub_key *manifest_public_key);

EXTERNAL_C_END
