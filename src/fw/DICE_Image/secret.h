/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include "crypto/crypto.h"

EXTERNAL_C_BEGIN

// Returns 0 if the secret is already set or if it has been correctly generated,
// an error code otherwise.
int DeviceSecretInitialize(void);

// The following functions return 0 on success.
int DeviceSecretGet(hw_uds *dest);
int DeviceSecretSet(hw_uds *src);

EXTERNAL_C_END
