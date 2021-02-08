/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include <assert.h>
#include <stdlib.h>

#include "crypto/ImageSigning.h"
#include "crypto/crypto.h"
#include "fw/common/image_header.h"

int image_header_build(image_header_t *header, uint32_t version, void *fw_data,
                       uint32_t fw_size, uint32_t offset) {
  assert(header);
  assert(fw_data);

  digest fw_hash;

  int ret = Crypto_Hash(&fw_hash, fw_data, fw_size);
  if (ret != 0)
    return -1;

  memset(header, 0, sizeof(image_header_t));

  header->magic = IMAGE_HEADER_MAGIC;
  header->fw_version = version;
  header->fw_size = fw_size;
  memcpy(&(header->fw_hash), &fw_hash, sizeof(digest));
  header->next_image_offset = offset;

  return 0;
}

int image_header_sign(image_header_t *header, fw_priv_key *priv_key) {
  assert(header);
  assert(priv_key);

  size_t size = sizeof(image_header_t) - sizeof(header->header_signature);

  int ret = ED25519_Sign(&(header->header_signature), header, size, priv_key);
  if (ret != 0)
    return -1;

  return 0;
}
