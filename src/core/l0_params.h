/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "crypto/crypto.h"

#pragma pack(push, 1)
typedef struct {
  // Full version number of DICE reported by DICE
  uint32_t VersionDICE;
  // HW Measurement obtained by DICE
  uint8_t HWMeasurement[HW_MEASUREMENT_LEN];
  // Storage for Compound Device Identifier
  digest CDI;
} l0_params_t;
#pragma pack(pop)
