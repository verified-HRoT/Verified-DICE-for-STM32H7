/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <cstdint>

// placeholder for the various ID types we might need as per the host API

typedef uint64_t PublicKey_ID;
typedef uint64_t GU_ID;
typedef uint32_t IPU_ID;
typedef uint32_t HW_ID;

// doesn't belong here.
typedef uint8_t Byte;

enum class LogType : uint32_t { Persistent, Complete };

enum class CSRType : uint32_t { PlatformSigning, PlatformIdentity };

enum class CertificateType : uint32_t {
  PlatformSigning,
  PlatformIdentity,
  Attestation
};
