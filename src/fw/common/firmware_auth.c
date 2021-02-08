/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "fw/common/firmware_auth.h"
#include "common/logger.h"
#include "crypto/crypto.h"
#include "fw/common/image_auth.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"

// This function returns 1 if the specified memory range is full of 0 and 0xFF
static int IsSpaceEmpty(const void *start, size_t size) {
  const uint8_t *p = start;

  for (size_t i = 0; i < size; i++) {
    if ((p[i] != 0xFF) && (p[i] != 0))
      return 0;
  }

  return 1;
}

auth_fw_status AuthenticateFirmware(const void *base,
                                    const fw_pub_key *manifest_public_key) {
  verify_status status;

  // Authenticate key manifest

  const key_manifest_t *manifest = base;

  status = VerifyKeyManifest(manifest, manifest_public_key);
  if (status != VERIFY_SUCCESS) {
    HW_LOG_FAIL("Key manifest is not valid: %d", status);
    return FW_AUTH_INVALID_MANIFEST;
  }

  // Check that space between manifest and L0 is empty

  const void *manifest_end = (void *)((uintptr_t)base + sizeof(key_manifest_t));
  uintptr_t manifest_l0_size = KEY_MANIFEST_SIZE - sizeof(key_manifest_t);

  if (!IsSpaceEmpty(manifest_end, manifest_l0_size)) {
    HW_LOG_FAIL("Extra data between Key manifest and L0");
    return FW_AUTH_EXTRA_DATA_MANIFEST_L0;
  }

  // Get public keys of L0 and L1 from manifest

  const fw_pub_key *l0_public_key = &(manifest->l0_pub_key);
  const fw_pub_key *l1_public_key = &(manifest->l1_pub_key);

  // Authenticate L0

  const void *l0_base = (void *)((uintptr_t)base + KEY_MANIFEST_SIZE);
  uintptr_t l0_base_addr = (uintptr_t)l0_base;

  HW_LOG_INFO("L0 image address is 0x%08X", l0_base_addr);

  status = VerifyImage(l0_base, l0_public_key);
  if (status != VERIFY_SUCCESS) {
    HW_LOG_FAIL("L0 image is not valid: %d", status);
    return FW_AUTH_INVALID_L0;
  }

  // Get location of L1 from L0 header

  const image_header_t *l0_header = l0_base;

  const size_t first_valid_l1_offset =
      IMAGE_HEADER_SIZE + (size_t)l0_header->fw_size;
  const size_t last_valid_l1_offset = GetFwSlotSize() - IMAGE_HEADER_SIZE;

  if ((l0_header->next_image_offset < first_valid_l1_offset) ||
      (l0_header->next_image_offset > last_valid_l1_offset)) {
    HW_LOG_FAIL("L1 offset invalid 0x%08X (range: 0x%08X-0x%08X)",
                l0_header->next_image_offset, first_valid_l1_offset,
                last_valid_l1_offset);
    return FW_AUTH_INVALID_L1_OFFSET;
  }

  uintptr_t l1_base_addr = l0_base_addr + l0_header->next_image_offset;

  HW_LOG_INFO("L1 image address is 0x%08X", l1_base_addr);

  void *l1_base = (void *)l1_base_addr;

  // Authenticate L1

  status = VerifyImage(l1_base, l1_public_key);
  if (status != VERIFY_SUCCESS) {
    HW_LOG_FAIL("L1 image is not valid: %d", status);
    return FW_AUTH_INVALID_L1;
  }

  // Check that the space between L0 and L1 is empty

  uintptr_t l0_end_addr = l0_base_addr + IMAGE_HEADER_SIZE + l0_header->fw_size;
  size_t l0_l1_gap_size = l1_base_addr - l0_end_addr;

  if (!IsSpaceEmpty((void *)l0_end_addr, l0_l1_gap_size)) {
    HW_LOG_FAIL("Extra data between L0 and L1");
    return FW_AUTH_EXTRA_DATA_L0_L1;
  }

  // Check that the space after L1 is empty

  const image_header_t *l1_header = l1_base;

  uintptr_t l1_end_addr = l1_base_addr + IMAGE_HEADER_SIZE + l1_header->fw_size;
  size_t l1_end_gap_size = ((uintptr_t)base + GetFwSlotSize()) - l1_end_addr;

  if (!IsSpaceEmpty((void *)l1_end_addr, l1_end_gap_size)) {
    HW_LOG_FAIL("Extra data after L1");
    return FW_AUTH_EXTRA_DATA_AFTER_L1;
  }

  // None of the tests failed

  return FW_AUTH_SUCCESS;
}
