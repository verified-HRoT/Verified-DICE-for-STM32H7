/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#include "common/Blob.h"
#include "common/common.h"
#include "common/logger.h"

#include "Hacl_Lib.h"
#include "ImageSigning.h"

class FwPublicKey : public Blob<FW_ED25519_PUBLIC_KEY_LEN> {
  using Blob::Blob;
};

class FwPrivateKey : public Blob<FW_ED25519_PRIVATE_KEY_LEN> {
  using Blob::Blob;
};

class FwSignature : public Blob<FW_ED25519_SIGNATURE_LEN> {
  using Blob::Blob;
};

EXTERNAL_C int ED25519_Sign(fw_signature *sig, const void *data,
                            size_t dataSize, const fw_priv_key *key) {
  Hacl_Ed25519_sign(sig->data, (uint8_t *)key->data, (uint32_t)dataSize,
                    (uint8_t *)data);

  return 0;
}

EXTERNAL_C int ED25519_Verify(const fw_signature *sig, const void *data,
                              size_t dataSize, const fw_pub_key *key) {
  const bool ret = Hacl_Ed25519_verify((uint8_t *)key->data, (uint32_t)dataSize,
                                       (uint8_t *)data, (uint8_t *)sig->data);

  if (ret)
    return 0;
  else
    return -1;
}
