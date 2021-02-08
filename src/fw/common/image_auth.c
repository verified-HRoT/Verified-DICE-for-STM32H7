/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <string.h>

#include "common/logger.h"
#include "crypto/crypto.h"
#include "fw/common/image_auth.h"
#include "fw/common/image_header.h"
#include "fw/common/memmap.h"

// Returns 0 if the hash of (buffer, size) matches reference_hash
static int VerifyHash(const digest *reference_hash, void *buffer, size_t size) {
  digest fw_hash_output;

  if (Crypto_Hash(&fw_hash_output, buffer, size) != 0)
    return -1;

  if (memcmp(&fw_hash_output, reference_hash, sizeof(fw_hash_output)) == 0)
    return 0;

  return -1;
}

// Returns 0 if the header is correctly signed
static int VerifyHeaderSignature(const image_header_t *header,
                                 const fw_pub_key *public_key) {
  // Calculate header size excluding signature, which is the last field
  size_t header_size =
      sizeof(image_header_t) - sizeof(header->header_signature);

  return ED25519_Verify(&(header->header_signature), header, header_size,
                        public_key);
}

verify_status VerifyImage(const void *addr, const fw_pub_key *key) {
  const image_header_t *header = (image_header_t *)addr;

  if (header->magic != IMAGE_HEADER_MAGIC)
    return VERIFY_ERROR_MAGIC;

  uint32_t fw_size = header->fw_size;

  // This is just a sanity check. However, the header may be
  // corrupt. In case the header says the firmware is bigger than the space
  // allocated for it, this check will prevent the hashing code to go past
  // the end of the actual image.
  if ((fw_size == 0) || (fw_size > L1_IMAGE_SIZE))
    return VERIFY_ERROR_SIZE;

  uint8_t *fw_start = (uint8_t *)((uintptr_t)addr + FIRMWARE_IMAGE_OFFSET);

  if (VerifyHash(&(header->fw_hash), fw_start, fw_size) != 0)
    return VERIFY_ERROR_HASH;

  if (VerifyHeaderSignature(header, key) != 0)
    return VERIFY_ERROR_SIGNATURE;

  return VERIFY_SUCCESS;
}

verify_status VerifyKeyManifest(const void *addr, const fw_pub_key *key) {
  const key_manifest_t *manifest = addr;

  if (manifest->version != KEY_MANIFEST_VERSION)
    return VERIFY_ERROR_VERSION;

  // Calculate manifest size excluding signature, which is the last field
  size_t size = sizeof(key_manifest_t) - sizeof(manifest->signature);

  if (ED25519_Verify(&(manifest->signature), manifest, size, key) != 0)
    return VERIFY_ERROR_SIGNATURE;

  return VERIFY_SUCCESS;
}
