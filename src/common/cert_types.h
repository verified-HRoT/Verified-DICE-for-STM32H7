/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include "common/Blob.h"
#include "common/common.h"
#include "common/id_types.h"

#define Vendor_Size (64)

using PlatformSVN = uint32_t;

class Vendor : public CharBlob<Vendor_Size> {
  using CharBlob::CharBlob;

public:
  // to be used only by code running on x86
  std::vector<uint8_t> uvector() const {
    CMemRef aspan = MakeCMemRef(this->data(), this->length());
    return std::vector<uint8_t>(std::begin(aspan), std::end(aspan));
  }

  std::vector<char> vector() const {
    CSignedMemRef aspan = MakeCSignedMemRef(this->data(), this->length());
    return std::vector<char>(std::begin(aspan), std::end(aspan));
  }
};
