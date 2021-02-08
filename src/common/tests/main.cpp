/*
 * Copyright(c) 2020 Microsoft. All rights reserved.
 */

#include "common/testing.h"

#include "common/common.h"
#include "common/logger.h"

namespace CodeUtilsTests {
struct Bob {
  int anInt = 123;
  char str[5];

  void print() const { printf("%d %s\n", anInt, &str[0]); }

  Bob() noexcept { memcpy(&str[0], "AAAA", sizeof(str)); }

  bool operator==(const Bob &other) const {
    if (strcmp(&str[0], &other.str[0]) != 0)
      return false;
    if (anInt != other.anInt)
      return false;
    return true;
  }
};

DEF_TEST(CodeUtilsTests, ReHydrate) {
  const Bob bob = {};

  const CMemRef bobSpan = MakeCMemRef(&bob);
  const Bob bobCopy = ReHydrate<Bob>(bobSpan);

  bob.print();
  bobCopy.print();

  GT_ASSERT_TRUE(bob == bobCopy);

  constexpr unsigned char stuff[] = {0x78, 0x56, 0x34, 0x12, 'a',
                                     'b',  'c',  'd',  0};
  const CMemRef span = gsl::span(&stuff[0], sizeof(stuff));
  const Bob fred = ReHydrate<Bob>(span);

  fred.print();

  GT_ASSERT_EQ(fred.anInt, 0x12345678);
  GT_ASSERT_EQ(strcmp(fred.str, "abcd"), 0);
}
} // namespace CodeUtilsTests

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
