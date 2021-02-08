/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "global/crypto_defs.h"

// Size, in bytes, of a Unique Device Secret
#define UDS_LENGTH 32

// Size, in bytes, of a L0 digest (SHA-256)
#define digest_LENGTH (32)

// Size, in bytes, of a symmetric key
#define SYMMETRIC_KEY_LENGTH (32)

#define KEY_MATERIAL_LENGTH (32)

// Max number of bytes in a label passed to the L0 KDF routine
#define MAX_KDF_LABEL_LENGTH digest_LENGTH

// Max number of bytes in a RIOT_AIK certificate
#define MAX_CERT_LENGTH (2048)

#define SERIAL_NUMBER_LENGTH (16)

#define X509_SNUM_LEN (0x08) // In bytes
#define DER_MAX_PEM (0x800)

#define HW_MEASUREMENT_LEN (32)
#define CONFIRMATION_MSG_SIZE (32)
