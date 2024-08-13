cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------------------------------------------------------
# 编译器配置

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm ASM)

# ----------------------------------------------------------------------------------------------------
# 项目配置

project(Plant-OS VERSION 0.0.0)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

include(cmake/mklib)
include(cmake/util)
include(cmake/add_binary)
include(cmake/mkheader)

add_subdirectory(include)

# ---------------------------------------------------------------------------------------------------
# 以下设定 本机 编译配置

set_compile_flag("-std=gnu2x" "-std=gnu++2b")

set_optim_flag("-g -O1")

message("本机 环境")
include(cmake/show-flags)

add_subdirectory(util)

# ----------------------------------------------------------------------------------------------------
# 以下设定 PLOS 编译配置

set_compile_flag("-std=gnu2x" "-std=gnu++2b") # 使用新的 C/C++ 版本

set_optim_flag("-g -O1")
# set_optim_flag("-O1") # 测试通过
# set_optim_flag("-O2") # 测试通过
# set_optim_flag("-O3") # 编译错误

# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#   add_compile_flag("-emit-obj") # 不输出 llvm-ir
# endif()

# 告诉编译器没有标准库 (但似乎不需要这个)
# set_link_flag("-nolibc -Qn")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_flag("-fmodulo-sched") # 在第一次调度过程之前立即执行摇摆模调度 (gcc 默认关闭)
  add_compile_flag("-fsplit-loops") # 对于循环内的 if 语句，将其分割为两个循环 (gcc 默认关闭)
  add_compile_flag("-funswitch-loops") # 将具有循环不变条件的分支移出循环 (gcc 默认关闭)
  add_compile_flag("-fversion-loops-for-strides") # 迭代的步幅为变量时创建另一个版本的循环，该循环假定步幅始终为 1 (gcc 默认关闭)
  add_compile_flag("-fstdarg-opt") # 对于可变参数函数，优化这些参数的使用 (gcc 默认关闭)
  add_compile_flag("-fgcse") # 全局公共子表达式消除 (gcc O2)
  add_compile_flag("-fgcse-sm") #
  add_compile_flag("-fipa-icf") # 对函数和只读变量执行相同的代码折叠 (gcc O2)
  add_compile_flag("-fipa-pta") # 过程间指针分析 (gcc 默认关闭)
endif()
add_compile_flag("-fmerge-all-constants") # 合并所有常量
add_compile_flag("-fno-keep-static-consts") # 不保留 static 常量
add_compile_flag("-fomit-frame-pointer") # 删除小函数的栈帧 (gcc O1)

# 开启 lto
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_flag("-flto=16 -ffat-lto-objects")
  add_link_flag("-Wl,-O1 -fuse-linker-plugin")
endif()

# 删除重复代码
add_compile_flag("-ffunction-sections -fdata-sections")
add_link_flag("-Wl,--gc-sections")

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

# 禁用部分 warning
# 这是我们希望的特性，但默认情况下会报 warning
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_flag("-Wno-compound-token-split-by-macro")
  add_compile_flag("-Wno-pointer-integer-compare")
endif()
add_compile_flag("-Wno-address-of-packed-member")

message("PLOS 环境")
include(cmake/show-flags)

add_subdirectory(src)
add_subdirectory(apps)
