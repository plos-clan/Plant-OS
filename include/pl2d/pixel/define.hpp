#pragma once
#include <libc-base.hpp>

// BasePixel 结构体的模板，因为有些长就写成宏
#define BasePixelTemplate                                                                          \
  typename T /* 储存颜色值的类型（无符号整数或浮点，有符号当作无符号） */,                         \
      typename T2 /* 直接运算时的类型（至少大一倍防止溢出） */,                                    \
      typename FT /* 转换成浮点数运算时的类型 */,                                                  \
      typename std::conditional_t<std::is_floating_point_v<T>, i32, T>                             \
          T_MAX /* 最大值（浮点设置为 1） */,                                                      \
      typename std::conditional_t<std::is_floating_point_v<T>, i32, T>                             \
          T_MAX_2 /* 对应有符号类型的最大值（浮点设置为 1） */
#define BasePixelT BasePixel<T, T2, FT, T_MAX, T_MAX_2>
// 就是上面的加个下划线
#define _BasePixelTemplate                                                                         \
  typename _T /* 储存颜色值的类型（无符号整数或浮点，有符号当作无符号） */,                        \
      typename _T2 /* 直接运算时的类型（至少大一倍防止溢出） */,                                   \
      typename _FT /* 转换成浮点数运算时的类型 */,                                                 \
      typename std::conditional_t<std::is_floating_point_v<_T>, i32, _T>                           \
          _T_MAX /* 最大值（浮点设置为 1） */,                                                     \
      typename std::conditional_t<std::is_floating_point_v<_T>, i32, _T>                           \
          _T_MAX_2 /* 对应有符号类型的最大值（浮点设置为 1） */
#define _BasePixelT BasePixel<_T, _T2, _FT, _T_MAX, _T_MAX_2>

// 参数的含义参考 BasePixelTemplate 的定义
#define BasePixelBT BasePixel<u8, u32, f32, U8_MAX, I8_MAX>
#define BasePixelST BasePixel<u16, u32, f32, U16_MAX, I16_MAX>
#define BasePixelIT BasePixel<u32, u64, f64, U32_MAX, I32_MAX>
#define BasePixelFT BasePixel<f32, f32, f32, 1, 1>
#define BasePixelDT BasePixel<f64, f64, f64, 1, 1>

// 显式模板实例化
#define BasePixelInstantiation                                                                     \
  template class BasePixelBT;                                                                      \
  template class BasePixelST;                                                                      \
  template class BasePixelIT;                                                                      \
  template class BasePixelFT;                                                                      \
  template class BasePixelDT;
