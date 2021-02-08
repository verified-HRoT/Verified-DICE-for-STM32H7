/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

// Derived from: https://github.com/Microsoft/RIoT
#pragma once

#include "common/machine.h"

#include <stdint.h>

#include "core/l0_params.h"
#include "core/l1_params.h"

// Key derivation labels used by both L0 Devices and External Infrastructure
#define L0_LABEL_IDENTITY "Identity"
#define L0_LABEL_SIGNING "Signing"
#define L0_LABEL_ALIAS "Alias"
// #define L0_LABEL_PROTECTOR    "Encrypt"
// #define L0_LABEL_INTEGRITY    "HMAC"
// #define L0_LABEL_AIK          "AikProtector"
// #define L0_LABEL_SK           "Sealing"
// #define L0_LABEL_MK           "Migration"
// #define L0_LABEL_AK           "Attestation"

// Function that performs the L0 core operations and deals with extra handling
// of L0 arguments. It gets as inputs the outputs of the L0 core, the base of
// the next firmware image, the size of the next firmware image, and a struct
// where it stores the results.
EXTERNAL_C void L0HandleArgs(uint8_t *l1_base, const size_t l1_size,
                             l0_params_t *l0_params, l1_params_t *l1_params);
