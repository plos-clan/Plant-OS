#pragma once

// 是否没有标准库
// 没有标准库你也需要提供一部分函数
#define NO_STD 1

// 优化选项，是否不使用编译器内置函数
// 一些内置函数会被编译器替换为对标准库函数的调用
// 这对于自制操作系统来说并不友好
#define NO_BUILTIN 1

// 优化选项，是否使用 C++ 的迭代器
// C++ 的迭代器一些情况下无法得到较好的优化
#define USE_ITERATOR 0

// 优化选项，修改循环以加速运算
// 会将部分的内外层循环倒置
#define LOOP_REORDER 1

// 忽略非法的参数
#define STD_SAFE_API 1 // 调用标准库函数时检查参数
#define SAFE_API     1 // 调用本库的函数时检查参数

// 使用16进制写颜色值的时候使用 0xRRGGBBAA 的顺序
// 而不是使用内存中的顺序，比如 RGBA 小端序 中的 0xAABBGGRR
#define COLOR_READABLE_HEX 1

// 使用 BGR 而不是 RGB 的通道顺序
// 此项对通道分离的 FrameBuffer 无效，其通道顺序仍然按照 RGBA
#define COLOR_USE_BGR 1

// 使用线性 RGB 而不是 sRGB 来存储颜色
// 这对 8 位深度来说并不好，若开启此项，建议使用浮点存储颜色
// 未完成，请勿设置此项
#define COLOR_RGB_LINEAR 0

// 是否启用快速的色彩空间变换等
#define FAST_COLOR_TRANSFORM 1

// 是否启用更好的颜色插值算法（更好但更慢）
#define BETTER_COLOR_INTERPOLATE 1
// 是否使用优化的插值算法
// 目前并没有实现
#define FAST_COLOR_INTERPOLATE   1

//** 在没有标准库的情况下 OSAPI 宏才有效 **

#define OSAPI_MALLOC 1 // 是否支持 malloc 函数
#define OSAPI_PALLOC 0 // 是否支持 palloc 函数

#define OSAPI_SHM 0 // 是否支持共享内存

// 其实你可以做一个只有内核且只有一个进程的 OS (
// 如果你想将整个图形化界面放到一个程序中，请将 OSAPI_MP 设置为 0
#define OSAPI_MP 0 // 是否在多进程模式下运行
#define OSAPI_MT 0 // 是否支持多线程

#define OSAPI_FILE_UNIX 1 // 类似 unix 的文件操作函数
#define OSAPI_FILE_EASY 0 // 简单的文件操作函数
