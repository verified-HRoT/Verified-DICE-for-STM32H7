/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <string.h>

#include "common/logger.h"
#include "crypto/crypto.h"
#include "fw/common/flash.h"
#include "fw/common/memmap.h"

#include "../secret.h"

// ----------------------------------------------------------------------------
// Unique Device Secret
// ----------------------------------------------------------------------------

// By placing the secret in the rodata section of DICE we make sure it is placed
// in flash by the linker. It has to be aligned to a flash word size so that we
// can modify it without afecting any data around it.
//
// The initial value must be 0xFF, as this can be overwriten without any need
// for erasing it first. Flash can only be erased in sectors (128 KB), so we
// would need to reserve a full flash sector for the device secret.
//
// There are two copies of the device secret to protect against the situation in
// which power is lost while writing the value. With just one copy of the secret
// there is no way to detect this situation. With two copies, if power is lost
// during the write of either copy, that copy will be corrupted. When reading
// the secret it is needed to check both values and compare them. If they don't
// match, it should be assumed that the secret is not valid. This means that the
// HW is unusable, as the secret can't be cleared without reflashing DICE.
static const hw_uds device_secret_1 __attribute__((aligned(FLASH_WORD_SIZE)))
__attribute__((section(".rodata.device_secret"))) = {{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
}};

static const hw_uds device_secret_2 __attribute__((aligned(FLASH_WORD_SIZE)))
__attribute__((section(".rodata.device_secret"))) = {{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
}};

int DeviceSecretGet(hw_uds *dest) {
  uintptr_t device_secret_addr_1 = (uint32_t)&device_secret_1;
  uintptr_t device_secret_addr_2 = (uint32_t)&device_secret_2;
  uintptr_t dice_start = (uintptr_t)GetDICEImageStart();
  uintptr_t dice_end = (uintptr_t)GetDICEImageEnd();

  if ((device_secret_addr_1 < dice_start) ||
      (device_secret_addr_1 >= (dice_end - UDS_LENGTH)) ||
      (device_secret_addr_2 < dice_start) ||
      (device_secret_addr_2 >= (dice_end - UDS_LENGTH))) {
    HW_LOG_FAIL("Device secret location outside of DICE image");
    return -1;
  }

  HW_LOG_DEBUG("Device secret locations: 0x%08lX 0x%08lX", device_secret_addr_1,
               device_secret_addr_2);

  if (memcmp(&device_secret_1, &device_secret_2, UDS_LENGTH) != 0) {
    HW_LOG_FAIL("Device secret values mismatch. This HW is unusable.");
    return -1;
  }

  memcpy(dest, &device_secret_1, UDS_LENGTH);

  return 0;
}

int DeviceSecretSet(hw_uds *src) {
  int ret = Flash_Write((void *)&device_secret_1, src, UDS_LENGTH);
  if (ret != 0) {
    HW_LOG_FAIL("Flash_Write(): %d", ret);
    return -1;
  }

  ret = Flash_Write((void *)&device_secret_2, src, UDS_LENGTH);
  if (ret != 0) {
    HW_LOG_FAIL("Flash_Write(): %d", ret);
    return -1;
  }

  return 0;
}
