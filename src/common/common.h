/*
 * Copyright (c) 2020 Microsoft. All rights reserved.
 */

#pragma once

// a place for frequently used types, especially ones relied on for interfaces
// eg gsl::span<uint8_t> etc

#include "machine.h"

#include <vector>

#include <gsl/span>
#include <gsl/span_ext>

#include <gsl/util>

#include <type_traits> // for make_unsigned

template <typename T>
T RemoveUnsigned(typename std::make_unsigned<T>::type val) {
  return gsl::narrow_cast<T>(val);
}

template <typename T> typename std::make_unsigned<T>::type MakeUnsigned(T val) {
  return gsl::narrow_cast<typename std::make_unsigned<T>::type>(val);
}

// encapsulate some memory which may be written, use instead of a ptr, len
using MemRef = gsl::span<uint8_t>;
using MemRefSizeType = MemRef::size_type;

using SignedMemRef = gsl::span<char>;
using SignedMemRefSizeType = SignedMemRef::size_type;

using MemVector = std::vector<uint8_t>;

// encapsulate some read only memory
using CMemRef = gsl::span<const uint8_t>;
using CSignedMemRef = gsl::span<const char>;

// from any old class T make a span covering uint8_ts
// intended for the Params classes.
template <class T> MemRef MakeMemRef(T *p) noexcept {
  void *ptr = p;
  static_assert(sizeof(*p) != 1);
  return MemRef(static_cast<uint8_t *>(ptr), sizeof(*p));
}

template <class T> MemRef MakeMemRef(T *p, size_t len) noexcept {
  void *ptr = p;
  return MemRef(static_cast<uint8_t *>(ptr), len);
}

template <class T> CMemRef MakeCMemRef(const T *p, size_t len) noexcept {
  const void *ptr = p;
  return CMemRef(static_cast<const uint8_t *>(ptr), len);
}

template <class T> CMemRef MakeCMemRef(const T *p) noexcept {
  const void *ptr = p;
  return CMemRef(static_cast<const uint8_t *>(ptr), sizeof(*p));
}

template <class T, size_t TLength>
CMemRef MakeCMemRef2(const T (&p)[TLength]) noexcept {
  const void *ptr = p;
  return CMemRef(static_cast<const uint8_t *>(ptr), TLength);
}

template <class T> SignedMemRef MakeSignedMemRef(T *p) noexcept {
  void *ptr = p;
  static_assert(sizeof(*p) != 1);
  return SignedMemRef(static_cast<char *>(ptr), sizeof(*p));
}

template <class T> SignedMemRef MakeSignedMemRef(T *p, size_t len) noexcept {
  void *ptr = p;
  return SignedMemRef(static_cast<char *>(ptr), len);
}

template <class T> CSignedMemRef MakeCSignedMemRef(T *p) noexcept {
  const void *ptr = p;
  static_assert(sizeof(*p) != 1);
  return CSignedMemRef(static_cast<const char *>(ptr), sizeof(*p));
}

template <class T> CSignedMemRef MakeCSignedMemRef(T *p, size_t len) noexcept {
  const void *ptr = p;
  return CSignedMemRef(static_cast<const char *>(ptr), len);
}

// T will needs top be copyable
template <class T> T ReHydrate(CMemRef source) noexcept {
  const void *ptr = source.data();
  const T *tInPlace = static_cast<const T *>(ptr);
  T ret = *tInPlace;
  return ret;
}

// Work around C4686
using _FixedLengthString = std::array<char, 1024>;
class FixedLengthString : public _FixedLengthString {
  using _FixedLengthString::_FixedLengthString;
};

// helper for HexDump
void HexByte(uint8_t byte, char &c1, char &c2);
// hex dump a block of memory into another block of memory.
size_t HexDump(CMemRef src, SignedMemRef dst);

template <class T, int LEN> struct IsPODEtc {
  static constexpr bool value =
      sizeof(T) == LEN && std::is_pod<T>::value && std::is_trivial<T>::value &&
      std::is_trivially_copyable<T>::value && std::is_standard_layout<T>::value;
};

/*
    Compile time protection against objects being different between the
    host and HW.

    usage - put something like the following outside the class:

    static_assert(OkAsParam<MyClass, 666>::value);

*/

template <class T, int LEN> struct SizeIsOK {
  static constexpr bool value = sizeof(T) == LEN;
};

template <class T> struct LayoutIsOK {
  // These two checks fail for most of our types as they have user provided
  // default ctors or members with default initialisers.
  // static_assert(std::is_pod<T>::value);
  // static_assert(std::is_trivial<T>::value);

  static constexpr bool value =

      // but anything we can serialise by copying must be trivially copyable
      std::is_trivially_copyable<T>::value &&

      // and must be standard layout, ie if you can't pass it to a C program
      // what hope is there that you can pass it over the wire to the HW.
      std::is_standard_layout<T>::value;
};

template <class T, int LEN> struct OkAsParam {
  static constexpr bool value = SizeIsOK<T, LEN>::value && LayoutIsOK<T>::value;
};

// Thinking about labeling classes explicitly. TBC, ignore for now.

class CannotBeSerialsed {
public:
  constexpr bool canBeSerialised() const { return false; }
};

class CanBeSerialsed {
public:
  constexpr bool canBeSerialised() const { return true; }
};
