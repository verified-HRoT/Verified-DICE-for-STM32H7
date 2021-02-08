/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <cstdint>

// Consider making param types' validate() method return this type and so one of
// the values provided rather than just true/false.

enum class CCEngineError : uint16_t {
  Success = 0,
  OutOfRange = 1,
  CryptoError = 2,
  DecryptKeyError = 3,
  InvalidObject = 4, // we asked an object for an opinion of its own state and
                     // it said it was bad.
};

enum class CryptoError : uint16_t {
  Success = 0,
  InvalidSize = 1,
};

#pragma pack(push, 1)

class CCStatus {
  CCEngineError CCEngine = CCEngineError::Success;
  CryptoError Crypto = CryptoError::Success;

  void SetCrypto(CryptoError val) noexcept { Crypto = val; }
  void SetCCEngine(CCEngineError val) noexcept { CCEngine = val; }

public:
  bool success() const noexcept {
    if ((GetCCEngine() != CCEngineError::Success) ||
        (GetCrypto() != CryptoError::Success)) {
      return false;
    }

    return true;
  }

  void setErrorCCEngine(CCEngineError ccEngineError) noexcept {
    SetCCEngine(ccEngineError);
  }

  void setErrorCrypto(CryptoError cryptoError) noexcept {
    SetCrypto(cryptoError);
  }

  // Some constructors to avoid writing more code than needed

  CCStatus(CCEngineError ccEngineError) noexcept
      : Crypto(CryptoError::Success) {
    SetCCEngine(ccEngineError);
  }

  CCStatus(CryptoError cryptoError) noexcept
      : CCEngine(CCEngineError::Success) {
    SetCrypto(cryptoError);
  }

  CCStatus(CryptoError cryptoError) noexcept
      : CCEngine(CCEngineError::Success) {
    SetCrypto(cryptoError);
  }

  CryptoError GetCrypto() const noexcept { return Crypto; }
  CCEngineError GetCCEngine() const noexcept { return CCEngine; }
};

#pragma pack(pop)
