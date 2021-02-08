#include "HWAbstraction.h"

#include "fw/DICE_Image/secret.h"

#include "common/logger.h"
#include "common/machine.h"
#include "core/l0_params.h"
#include "fw/common/image_auth.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"
#include "fw/common/shared_mem_volatile.h"

uint32_t uds_len = UDS_LENGTH;

HWState_state st() {
  const void *base = GetFwActiveStart();

  const key_manifest_t *manifest = base;

  // Get public key of L0 from manifest
  const fw_pub_key *l0_public_key = &(manifest->l0_pub_key);

  const void *l0_base = (void *)((uintptr_t)base + KEY_MANIFEST_SIZE);
  uintptr_t l0_base_addr = (uintptr_t)l0_base;

  const image_header_t *l0_header = l0_base;

  uint8_t *fw_start = (uint8_t *)((uintptr_t)l0_base + FIRMWARE_IMAGE_OFFSET);

  // Calculate header size excluding signature, which is the last field
  uint32_t header_size =
      sizeof(image_header_t) - sizeof(l0_header->header_signature);

  uint8_t *image_header = (uint8_t *)l0_header;
  uint8_t *image_hash = (uint8_t *)&(l0_header->fw_hash).data;
  uint8_t *header_sig = (uint8_t *)l0_header->header_signature.data;

  uint32_t image_size = (uint32_t)l0_header->fw_size;
  uint8_t *image_base = (uint8_t *)fw_start;
  uint8_t *pubkey = (uint8_t *)l0_public_key->data;

  HWState_l0_image_t img = {header_size, image_header, header_sig, image_size,
                            image_base,  image_hash,   pubkey};

  l0_params_t *l0_params = (l0_params_t *)GetShared_DICE_L0();

  HWState_state sta = {l0_params->CDI.data, img};
  return sta;
}

void read_uds(uint8_t *uds_out) {
  int ret;

  ret = DeviceSecretGet((hw_uds *)uds_out);
  if (ret != 0) {
    HW_LOG_FAIL("DeviceSecretGet() failed: %d", ret);
    return;
  }
}

void platform_zeroize_stack() {

  // This method is not implemented.
  //
  // Each layer clears their stack and zeroes the registers
  // before jumping to the next layer.
  //
  // Refer to -
  //          src/fw/common/arm/image_launch.c
  //          src/fw/common/arm/image_launch_asm.s

  return;
}

void disable_uds() {

  // This method is not implemented.
  //
  // Each layer clears their stack and zeroes the registers
  // before jumping to the next layer.
  //
  // Refer to -
  //          src/fw/common/arm/image_launch.c
  //          src/fw/common/arm/image_launch_asm.s

  // UDS is stored in the flash memory region (along with the stack).
  // After DICE, BootToL0 is called with "RSS_TABLE->exitSecureArea"
  // which exits the secure mode and secures the UDS (before jumping to L0)
  // Exiting the secure mode prevents L0 from being able to access the UDS.

  return;
}

void zeroize(uint32_t len, uint8_t *buf) {
  platform_zeroize(buf, len);
  return;
}
