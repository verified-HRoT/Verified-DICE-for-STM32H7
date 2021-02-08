#include "common.h"

void HexByte(uint8_t byte, char &c1, char &c2) {
  int r1{}, r2{};
  const int msb = byte >> 4;
  if (msb < 10)
    r1 = '0' + msb;
  else
    r1 = 'a' + msb - 10;

  const int lsb = byte & 0x0F;
  if (lsb < 10)
    r2 = '0' + lsb;
  else
    r2 = 'a' + lsb - 10;

  c1 = gsl::narrow_cast<char>(r1);
  c2 = gsl::narrow_cast<char>(r2);
}

size_t HexDump(CMemRef src, SignedMemRef dst) {
  // the sizes must match, dst is two chars per bytes and a zero term.
  const CMemRef::size_type length = src.size() * 2;
  if (length + 1 != dst.size())
    return 0;
  for (CMemRef::size_type pos = 0; pos < src.size(); pos++) {
    const uint8_t byte = gsl::at(src, pos);
    char c1, c2;
    HexByte(byte, c1, c2);
    gsl::at(dst, pos * 2) = c1;
    gsl::at(dst, pos * 2 + 1) = c2;
  }
  gsl::at(dst, length) = 0;
  return length;
}
