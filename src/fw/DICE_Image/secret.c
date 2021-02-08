/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "common/logger.h"
#include "common/rng.h"
#include "crypto/crypto.h"

#include "secret.h"

// Returns 1 if the secret is valid, 0 otherwise (all 0xFFs).
static int IsSecretValid(const hw_uds *secret) {
  for (size_t i = 0; i < sizeof(hw_uds); i++) {
    if (secret->data[i] != 0xFF)
      return 1;
  }
  return 0;
}

// Generate device secret if it hasn't been set.
int DeviceSecretInitialize(void) {
  int ret;
  hw_uds secret = {0};

  ret = DeviceSecretGet(&secret);
  if (ret != 0) {
    HW_LOG_FAIL("DeviceSecretGet() failed: %d", ret);
    return -1;
  }

  // If the secret has previously been set, don't do anything
  if (IsSecretValid(&secret)) {
    HW_LOG_INFO("Device secret already set");
    HW_LOG_SECRET_HEX(secret.data, sizeof(secret));
    return 0;
  }

  HW_LOG_INFO("Generating device secret...");

  // Generate secret and iterate in the unlikely case we generate a secret
  // full of 0xFFs
  do {
    ret = RNG_Rand((unsigned char *)&secret, sizeof(secret));
    if (ret != 0) {
      HW_LOG_FAIL("HW_RNG_Rand() failed: %d", ret);
      return -1;
    }
  } while (IsSecretValid(&secret) == 0);

  HW_LOG_INFO("Device secret generated");
  HW_LOG_SECRET_HEX(secret.data, sizeof(secret));

  // Try to save the device secret
  ret = DeviceSecretSet(&secret);
  if (ret != 0) {
    HW_LOG_FAIL("DeviceSecretSet() failed: %d", ret);
    return -1;
  }

  hw_uds reference = {0};
  ret = DeviceSecretGet(&reference);
  if (ret != 0) {
    HW_LOG_FAIL("DeviceSecretGet() failed: %d", ret);
    return -1;
  }

  if (memcmp(&secret, &reference, sizeof(secret)) != 0) {
    HW_LOG_FAIL("Failed to write device secret");
    return -1;
  }

  HW_LOG_INFO("Device secret saved");

  return 0;
}
