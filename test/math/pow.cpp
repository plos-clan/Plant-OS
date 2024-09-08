#include <cmath>
#include <gtest/gtest.h>

#define NO_STD 0
#include <libc++.hpp>

TEST(PowFloatTest, PositiveExponents) {
  EXPECT_NEAR(cpp::pow(2.0f, 2.0f), 4.0f, cpp::F32_EPSILON * 4.0f);
  EXPECT_NEAR(cpp::pow(10.0f, 3.0f), 1000.0f, cpp::F32_EPSILON * 1000.0f);
}

TEST(PowFloatTest, NegativeExponents) {
  EXPECT_NEAR(cpp::pow(3.0f, -2.0f), 0.1111111111f, cpp::F32_EPSILON);
  EXPECT_NEAR(cpp::pow(7.0f, -1.5f), 0.0539949247f, cpp::F32_EPSILON);
}

TEST(PowDoubleTest, PositiveExponents) {
  EXPECT_NEAR(cpp::pow(2.0, 2.0), 4.0, cpp::F64_EPSILON * 4.0);
  EXPECT_NEAR(cpp::pow(10.0, 3.0), 1000.0, cpp::F64_EPSILON * 1000.0);
}

TEST(PowDoubleTest, NegativeExponents) {
  EXPECT_NEAR(cpp::pow(3.0, -2.0), 0.111111111111111, cpp::F64_EPSILON);
  EXPECT_NEAR(cpp::pow(7.0, -1.5), 0.053994924715604, cpp::F64_EPSILON);
}

auto main(int argc, char **argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
