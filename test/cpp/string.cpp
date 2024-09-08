#include <gtest/gtest.h>

#define NO_STD 0
#include <misc.h>

using String = cpp::String;

// 测试构造函数和基本功能
TEST(BaseStringTest, ConstructorAndBasicFunctions) {
  String str1("Hello");
  EXPECT_EQ(str1.size(), 5);
  EXPECT_STREQ(str1.c_str(), "Hello");

  String str2("World", 3);
  EXPECT_EQ(str2.size(), 3);
  EXPECT_STREQ(str2.c_str(), "Wor");

  String str3;
  EXPECT_EQ(str3.size(), 0);
  EXPECT_EQ(str3.c_str(), nullptr);
}

// 测试拷贝构造函数
TEST(BaseStringTest, CopyConstructor) {
  String str1("Hello");
  String str2(str1);

  EXPECT_EQ(str2.size(), 5);
  EXPECT_STREQ(str2.c_str(), "Hello");
}

// 测试移动构造函数
TEST(BaseStringTest, MoveConstructor) {
  String str1("Hello");
  String str2(std::move(str1));

  EXPECT_EQ(str1.size(), 0); // 移动后原对象应为空
  EXPECT_EQ(str1.c_str(), nullptr);
  EXPECT_EQ(str2.size(), 5);
  EXPECT_STREQ(str2.c_str(), "Hello");
}

// 测试拷贝赋值运算符
TEST(BaseStringTest, CopyAssignmentOperator) {
  String str1("Hello");
  String str2;
  str2 = str1;

  EXPECT_EQ(str2.size(), 5);
  EXPECT_STREQ(str2.c_str(), "Hello");
}

// 测试移动赋值运算符
TEST(BaseStringTest, MoveAssignmentOperator) {
  String str1("Hello");
  String str2;
  str2 = std::move(str1);

  EXPECT_EQ(str1.size(), 0); // 移动后原对象应为空
  EXPECT_EQ(str1.c_str(), nullptr);
  EXPECT_EQ(str2.size(), 5);
  EXPECT_STREQ(str2.c_str(), "Hello");
}

// 测试下标访问
TEST(BaseStringTest, SubscriptOperator) {
  String str("Hello");

  EXPECT_EQ(str[0], 'H');
  EXPECT_EQ(str[1], 'e');
  EXPECT_EQ(str[4], 'o');

  str(1) = 'a';
  EXPECT_EQ(str[1], 'a');
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
