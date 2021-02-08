/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <array>

#include <algorithm>
#include <gsl/span>

#include "common/common.h"
#include "common/logger.h"

// A wrapper around std::array that is specialized to uint8_t and enforces
// initialization
template <size_t TLength> class Blob : public CanBeSerialsed {
  using CArrayType = uint8_t[TLength];
  using CppArrayType = std::array<uint8_t, TLength>;

  CppArrayType _data;

public:
  static constexpr size_t array_size = TLength;
  using array_type = CppArrayType;

  constexpr Blob(const CArrayType &data) {
    std::copy(std::begin(data), std::end(data), _data.begin());
  }

  constexpr Blob(const gsl::span<const uint8_t> &data) {
    const size_t size = std::min(_data.size(), data.size());
    std::copy_n(data.begin(), size, _data.begin());
  }

  constexpr Blob() noexcept : _data() {
    for (auto &b : _data) {
      b = 0;
    }
  }

  CppArrayType &array() { return _data; }

  const CppArrayType &array() const { return _data; }

  MemRef span() { return MakeMemRef(&_data); }

  CMemRef span() const { return MakeCMemRef(&_data); }

  uint8_t *data() noexcept { return _data.data(); }

  const uint8_t *data() const noexcept { return _data.data(); }

  uint8_t *end() {
    const MemRef span = gsl::make_span(data(), TLength + 1);
    const MemRef sub = span.subspan(TLength, 1);
    return sub.data();
  }

  size_t length() const { return TLength; }

  bool operator==(const Blob<TLength> &d) const { return d.array() == array(); }

  bool operator!=(const Blob<TLength> &d) const { return d.array() != array(); }

  // we cannot check the whole object, but we can check the array.
  static_assert(OkAsParam<CppArrayType, TLength>::value);
};

// A wrapper around std::array that is specialized to char and enforces
// initialization
template <size_t TLength> class CharBlob : public CannotBeSerialsed {
  using CArrayType = char[TLength];
  using CppArrayType = std::array<char, TLength>;

private:
  CppArrayType _data;
  size_t _length; // Danger!

public:
  constexpr CharBlob(const gsl::span<const char> &data) {
    const size_t size = data.size();

    if (size > TLength) {
      HW_LOG_FAIL("Blob not long enough");
      while (1)
        ;
    }

    _data.fill(0);
    std::copy_n(data.begin(), size, _data.begin());
    _length = size;
  }

  constexpr CharBlob(const char *data, size_t length) {
    if (length > TLength) {
      HW_LOG_FAIL("Blob not long enough");
      while (1)
        ;
    }

    const CSignedMemRef aspan = MakeCSignedMemRef(data, length);
    _data.fill(0);
    std::copy_n(aspan.begin(), length, _data.begin());
    _length = length;
  }

  constexpr CharBlob() noexcept : _data() {
    _data.fill(0);
    _length = 0;
  }

  CppArrayType &array() { return _data; }

  const CppArrayType &array() const { return _data; }

  SignedMemRef span() { return MakeSignedMemRef(&_data); }

  CSignedMemRef span() const { return MakeCSignedMemRef(&_data); }

  char *data() { return _data.data(); }

  const char *data() const { return _data.data(); }

  size_t maxlength() const { return TLength; }

  size_t length() const { return _length; }

  bool operator==(const CharBlob<TLength> &d) {
    if (d.length() != length()) {
      return false;
    }

    // not using the strcmp as these buffers may not have null-terminating
    // character
    const size_t len = length();
    const CSignedMemRef _dspan = MakeCSignedMemRef(data(), len);
    const CSignedMemRef dspan = MakeCSignedMemRef(d.data(), len);

    return (_dspan == dspan);
  }

  bool operator!=(const CharBlob<TLength> &d) {
    if (d.length() != length()) {
      return true;
    }

    // not using the strcmp as these buffers may not have null-terminating
    // character
    const size_t len = length();
    const CSignedMemRef _dspan = MakeCSignedMemRef(data(), len);
    const CSignedMemRef dspan = MakeCSignedMemRef(d.data(), len);

    return (_dspan != dspan);
  }
};
