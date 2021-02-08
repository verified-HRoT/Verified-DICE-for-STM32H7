/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

// Refer to https://tools.ietf.org/html/rfc8032#section-5.1.5

// ED25519 used for image signing
// ---------------------------------------------

#define FW_ED25519_PRIVATE_KEY_LEN (32)
#define FW_ED25519_PUBLIC_KEY_LEN (32)

#define FW_ED25519_SIGNATURE_LEN (64)

// ED25519 used for certificates
// --------------------------------------------

#define ED25519_PRIVATE_KEY_LEN (32)
#define ED25519_PUBLIC_KEY_LEN (32)

#define ED25519_SIGNATURE_LEN (64)
