cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------------------------------------------------------
# 编译器配置

set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm ASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)

# ----------------------------------------------------------------------------------------------------
# 项目配置

project(Plant-OS VERSION 0.0.0)

enable_language(ASM_NASM)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

enable_testing()
include(GoogleTest)

include_directories(${PROJECT_SOURCE_DIR}/include)

include(cmake/benchmark)
include(cmake/gtest)
include(cmake/mklib)
include(cmake/util)
include(cmake/add_binary)
include(cmake/mkheader)

add_subdirectory(include)

# ---------------------------------------------------------------------------------------------------
# 以下设定 本机 编译配置

set_compile_flag("-std=gnu2x" "-std=gnu++2b")
set_link_flag("")

set_optim_flag("-g -O1") # 调试开 O1
# set_optim_flag("-Ofast") # 编译开 O3

add_compile_flag("-march=native")

include(cmake/optim) # 优化选项
include(cmake/lto) # 链接时优化
include(cmake/code-gc) # 删除重复代码

include(cmake/disable-warning)

message("本机 环境")
include(cmake/show-flags)

add_subdirectory(test)
add_subdirectory(util)

# ----------------------------------------------------------------------------------------------------
# 以下设定 PLOS 编译配置

set_compile_flag("-std=gnu2x" "-std=gnu++2b") # 使用新的 C/C++ 版本
set_link_flag("")

set_optim_flag("-g -O1")
# set_optim_flag("-O1") # 测试通过
# set_optim_flag("-O2") # 测试通过
# set_optim_flag("-O3") # 编译错误

# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#   add_compile_flag("-emit-obj") # 不输出 llvm-ir
# endif()

# 告诉编译器没有标准库 (但似乎不需要这个)
# add_link_flag("-nolibc -Qn")

include(cmake/optim) # 优化选项
# include(cmake/lto) # 链接时优化
# include(cmake/code-gc) # 删除重复代码

# 其它选项
add_compile_flag("-fno-optimize-sibling-calls") # 关闭尾递归优化
add_compile_flag("-fno-delete-null-pointer-checks") # 不删除程序员进行的空指针检查 (gcc 默认关闭)
add_compile_flag("-fno-strict-aliasing") # 禁用严格别名规则 (gcc 默认关闭)
add_compile_flag("-m32 -mfpmath=387")
add_compile_flag("-mno-mmx -mno-sse")
add_compile_flag("-nostdlib -nostdinc") # 不使用标准库和标准头
add_compile_flag("-finput-charset=UTF-8 -fexec-charset=UTF-8") # 使用 utf-8 (默认)
add_compile_flag("-ffreestanding") # 我们希望我们的标准库函数能够有和标准库一样的行为，所以我们希望可以不设置此项
add_compile_flag("-fno-stack-protector") # 不使用栈保护 (默认使用)
add_compile_flag("-fno-pic -fno-pie") # 不使用 pic 和 pie
add_compile_flag("-fno-asynchronous-unwind-tables")
add_compile_flag("-fno-exceptions")

include(cmake/disable-warning)

message("PLOS 环境")
include(cmake/show-flags)

add_subdirectory(src)
add_subdirectory(apps)
