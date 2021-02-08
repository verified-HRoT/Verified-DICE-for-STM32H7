/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <cassert>
#include <string>

#include "common/Blob.h"
#include "common/Buffer.h"
#include "common/common.h"
#include "common/errors.h"
#include "common/id_types.h"
#include "common/rng.h"

#include "crypto_config.h"

#pragma pack(push, 1)

/**
 * PEM encoded X.509 certificates
 */
class Certificate : public CharBuffer<DER_MAX_PEM> {
  using CharBuffer::CharBuffer;

public:
  std::string toString() {
    std::string str(data(), length());
    return str;
  }
};

/**
 * PEM encoded X.509 Certificate Signing Requests
 */
class CertSignRequest : public CharBuffer<DER_MAX_PEM> {
  using CharBuffer::CharBuffer;

public:
  std::string toString() {
    std::string str(data(), length());
    return str;
  }
};

class SerialNumber : public Blob<SERIAL_NUMBER_LENGTH> {
  using Blob::Blob;
};

class HWMeasurement : public Blob<HW_MEASUREMENT_LEN> {
  using Blob::Blob;
};

class Digest : public Blob<digest_LENGTH> {
  using Blob::Blob;
};

class Signature : public Blob<ED25519_SIGNATURE_LEN> {
  using Blob::Blob;
};

class PublicKey : public Blob<ED25519_PUBLIC_KEY_LEN> {
  using Blob::Blob;

public:
  int Verify(const Digest &digest, const Signature &sig);
  int Verify(const CMemRef &data, const Signature &signature);
};

class PrivateKey : public Blob<ED25519_PRIVATE_KEY_LEN> {
  using Blob::Blob;

public:
  int Sign(const Digest &digest, Signature &signature);
  int Sign(const CMemRef &buffer, Signature &signature);
};

class KeyMaterial : public Blob<KEY_MATERIAL_LENGTH> {
  using Blob::Blob;

public:
  // Generates fresh key material
  void GenerateFresh() { RNG_Rand(data(), length()); }

  // Loads data from the digest to be used as key material
  KeyMaterial(Digest &digest) {
    for (size_t i = 0; i < length(); i++)
      array().at(i) = digest.array().at(i);
  }
};

static_assert(sizeof(KeyMaterial) == sizeof(Digest));
static_assert(OkAsParam<KeyMaterial, KEY_MATERIAL_LENGTH>::value);

class SymmetricKey : public Blob<SYMMETRIC_KEY_LENGTH> {
  using Blob::Blob;

public:
  // Loads data from the key material to be used as symmetric key
  SymmetricKey(KeyMaterial &keyMaterial) {
    for (size_t i = 0; i < length(); i++)
      array().at(i) = keyMaterial.array().at(i);
  }
};

static_assert(sizeof(KeyMaterial) == sizeof(SymmetricKey));
static_assert(OkAsParam<SymmetricKey, SYMMETRIC_KEY_LENGTH>::value);

#pragma pack(pop)