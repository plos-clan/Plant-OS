// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "00-include.hpp"

#define assert_type_is_int(T)   static_assert(std::is_integral_v<T>, "类型 " #T " 必须是整数")
#define assert_type_is_float(T) static_assert(std::is_floating_point_v<T>, "类型 " #T " 必须是浮点")
#define assert_type_is_ptr(T)   static_assert(std::is_pointer_v<T>, "类型 " #T " 必须是指针")
#define assert_type_is_class(T) static_assert(std::is_class_v<T>, "类型 " #T " 必须是类")
#define assert_type_is_num(T)                                                                      \
  static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, #T " 必须是数字")
