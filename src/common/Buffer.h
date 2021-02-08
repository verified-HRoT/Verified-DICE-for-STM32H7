/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

#include <array>

#include <algorithm>

#include "common/common.h"
#include "common/logger.h"

// A wrapper around std::array that knows how much of the internal buffer is
// used
template <typename TStorage, size_t TMaxLength>
class basic_Buffer // : public CanBeSerialsed
{
  using CArrayType = TStorage[TMaxLength];
  using CppArrayType = std::array<TStorage, TMaxLength>;

  uint32_t _length;
  CppArrayType _data;

  static_assert(OkAsParam<CppArrayType, TMaxLength * sizeof(TStorage)>::value);

public:
  basic_Buffer(const TStorage (&data)[TMaxLength], bool isString = false)
      : _length(TMaxLength) {
    std::copy(std::begin(data), std::end(data), std::begin(getArray()));
    if (isString) {
      setLengthAsString();
    }
  }

#if BIG_BUILD
  basic_Buffer(const std::vector<TStorage> &data) {
    if (data.size() > TMaxLength) {
      throw std::logic_error("Buffer not long enough");
    }
    getArray().fill(0);
    std::copy(std::begin(data), std::end(data), std::begin(getArray()));
    setLength(data.size());
  }
#endif

  basic_Buffer() noexcept : _length(0), _data() {
    for (auto &b : getArray()) {
      b = 0;
    }
  }

  // TODO think about how to return a buffer of just length
  CppArrayType &array() { return _data; }

  CMemRef getData() const { return CMemRef(getArray().data(), getLength()); }

  const CppArrayType &array() const { return _data; }

  CppArrayType &getArray() noexcept { return _data; }

  TStorage *data() { return _data.data(); }

  const TStorage *data() const { return _data.data(); }

  size_t maxlength() const { return TMaxLength; }

  size_t length() const { return getLength(); }

  void setLength(size_t length) {
    if (length > TMaxLength) {
      // throw std::logic_error("New length greater than buffer length");
      HW_LOG_FAIL("New length greater than buffer length");
      while (1)
        ;
    }

    // we are sure this is < 4GB
    _length = gsl::narrow_cast<uint32_t>(length);
  }

  void setLengthAsString() {
    setLength(TMaxLength);
    for (size_t i = 0; i < TMaxLength; i++) {
      if (getArray().at(i) == 0) {
        setLength(i);
        break;
      }
    }
  }

  void zero() {
    getArray().fill(0);
    setLength(0);
  }

  void copyTo(gsl::span<TStorage, TMaxLength> span) {
    std::copy(std::begin(getArray()), std::end(getArray()), std::begin(span));
  }
  size_t getLength() const { return static_cast<size_t>(_length); }
};

template <size_t TMaxLength>
class Buffer : public basic_Buffer<uint8_t, TMaxLength> {
  using basic_Buffer<uint8_t, TMaxLength>::basic_Buffer;
};

template <size_t TMaxLength>
class CharBuffer : public basic_Buffer<char, TMaxLength> {
  using base_type = basic_Buffer<char, TMaxLength>;

  // gcc forces me to do this, but just following a minor refactor
  // while MSVC is quite ok.
  using base_type::array;
  using base_type::getArray;
  using base_type::setLength;
  // get the ctor - clang especially fussy here
  using base_type::base_type;

public:
// can't use string and vector
#if BIG_BUILD
  CharBuffer(const std::string &str) {
    if (str.size() > TMaxLength) {
      throw std::logic_error("CharBuffer not long enough");
    }
    getArray().fill(0);
    std::copy(std::begin(str), std::end(str), std::begin(getArray()));
    setLength(str.size());
    // cannot have this code on the arm side as it uses a string.
    static_assert(BIG_BUILD != 0);
  }

  CharBuffer(const std::vector<uint8_t> &src) {
    const size_t size = src.size();
    if (size > TMaxLength) {
      throw std::logic_error("CharBuffer not long enough");
    }
    getArray().fill(0);
    std::copy(std::begin(src), std::end(src), std::begin(getArray()));
    setLength(size);
    // cannot have this code on the arm side as it uses a vector.
    static_assert(BIG_BUILD != 0);
  }
#endif

  CharBuffer(const gsl::span<const char> &data) {
    size_t size = data.size();
    if (size > TMaxLength) {
      throw std::logic_error("CharBuffer not long enough");
    }
    getArray().fill(0);
    std::copy_n(data.begin(), size, std::begin(getArray()));
    this->setLength(size);
  }

  uint8_t *raw() { return reinterpret_cast<uint8_t *>(getArray().data()); }

  const uint8_t *raw() const {
    return reinterpret_cast<const uint8_t *>(array().data());
  }
};
