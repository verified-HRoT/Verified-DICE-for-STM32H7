/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <stdlib.h>

#include "crypto/crypto.h"
#include "fw/DICE_Image/secret.h"

// ----------------------------------------------------------------------------
// Unique Device Secret
// ----------------------------------------------------------------------------

static hw_uds device_secret = {{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
}};

int DeviceSecretGet(hw_uds *dest) {
  memcpy(dest, &device_secret, UDS_LENGTH);

  return 0;
}

int DeviceSecretSet(hw_uds *src) {
  memcpy(&device_secret, src, UDS_LENGTH);

  return 0;
}
