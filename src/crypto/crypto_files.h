/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/machine.h"

#include "ImageSigning.h"
#include "crypto.h"

EXTERNAL_C_BEGIN

// Load private key from a file in PEM format. Returns 0 on success.
int crypto_load_priv_key(const char *path, fw_priv_key *priv_key);
int crypto_gen_pub_key(fw_pub_key *pub_key, const fw_priv_key *priv_key);

EXTERNAL_C_END
