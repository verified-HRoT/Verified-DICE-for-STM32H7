/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "crypto/crypto.h"

#pragma pack(push, 1)
typedef struct {
  // Full version numbers of DICE and L0 reported by each image
  uint32_t VersionDICE;
  uint32_t VersionL0;

  // HW Measurement obtained by DICE
  uint8_t HWMeasurement[HW_MEASUREMENT_LEN];

  pub_key_ed25519 AliasKeyPub;
  priv_key AliasKeyPriv;
  char AliasKeyCert[DER_MAX_PEM];
  size_t AliasKeyCertLen;

  pub_key_ed25519 DeviceIDPub;
  char DeviceIDCsr[DER_MAX_PEM];
  size_t DeviceIDCsrLen;
} l1_params_t;
#pragma pack(pop)
