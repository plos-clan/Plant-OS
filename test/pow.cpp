#include <cmath>
#include <gtest/gtest.h>

#include <define/config.h>
#undef NO_STD
#define NO_STD 0
#include <libc++.hpp>

TEST(PowFloatTest, PositiveExponents) {
  EXPECT_NEAR(cpp::pow(2.0f, 2.0f), 4.0f, 1e-6);
  EXPECT_NEAR(cpp::pow(10.0f, 3.0f), 1000.0f, 1e-6 * 1000.0f);
}

TEST(PowFloatTest, NegativeExponents) {
  EXPECT_NEAR(cpp::pow(3.0f, -2.0f), 1.0f / 9.0f, 1e-6);
  EXPECT_NEAR(cpp::pow(7.0f, -1.5f), 1.0f / std::sqrt(7.0f * 7.0f * 7.0f), 1e-6);
}

TEST(PowDoubleTest, PositiveExponents) {
  EXPECT_NEAR(cpp::pow(2.0, 2.0), 4.0, 1e-9);
  EXPECT_NEAR(cpp::pow(10.0, 3.0), 1000.0, 1e-9);
}

TEST(PowDoubleTest, NegativeExponents) {
  EXPECT_NEAR(cpp::pow(3.0, -2.0), 1.0 / 9.0, 1e-9);
  EXPECT_NEAR(cpp::pow(7.0, -1.5), 1.0 / std::sqrt(7.0 * 7.0 * 7.0), 1e-9);
}

auto main(int argc, char **argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
