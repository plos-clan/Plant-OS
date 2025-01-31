#pragma once

// 动态链接器错误
#define LDE_UNKNOWN            0   // 未知错误
#define LDE_ENTRY_POINT        1   // 未定义入口点
#define LDE_FILE_NOT_FOUND     2   // 无法找到文件
#define LDE_FILE_UNREADABLE    3   // 无法读取文件
#define LDE_FILE_UNPARSABLE    4   // 无法解析文件
#define LDE_DEP_LIB_NOT_FOUND  5   // 无法找到依赖库
#define LDE_DEP_LIB_UNREADABLE 6   // 无法读取依赖库
#define LDE_DEP_LIB_UNPARSABLE 7   // 无法解析依赖库
#define LDE_OUT_OF_MEMORY      8   // 内存不足
#define LDE_ARCH_MISSMATCH     9   // 架构不匹配
#define LDE_INVALID_INPUT      255 // 传入参数错误
