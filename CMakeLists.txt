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

set_compile_flag("-std=gnu2x" "-std=gnu++2b")

set_optim_flag("-g -O1")
# set_optim_flag("-O1")
# set_optim_flag("-O2") # 目前无法在 O2 下正常编译

# 告诉编译器没有标准库 (但似乎不需要这个)
# set_link_flag("-nolibc -Qn")

# if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#   add_compile_flag("-emit-obj") # 不输出 llvm-ir
# endif()

add_compile_flag("-fno-delete-null-pointer-checks") # 不删除程序员进行的空指针检查
add_compile_flag("-fno-strict-aliasing") # 禁用严格别名规则
add_compile_flag("-m32 -mfpmath=387")
add_compile_flag("-mno-mmx -mno-sse")
add_compile_flag("-nostdlib -nostdinc")
add_compile_flag("-finput-charset=UTF-8 -fexec-charset=UTF-8")
# add_compile_flag("-ffreestanding") # 我们希望我们d的标准库函数能够有和标准库一样的行为 (所以默认不设置此项)
add_compile_flag("-fno-stack-protector")
add_compile_flag("-fno-pic -fno-pie")
add_compile_flag("-fno-asynchronous-unwind-tables")
add_compile_flag("-fomit-frame-pointer")
add_compile_flag("-fno-exceptions")

# 禁用部分 warning
# 这是我们希望的特性，但默认情况下会报 warning
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_flag("-Wno-compound-token-split-by-macro")
endif()

message("PLOS 环境")
include(cmake/show-flags)

add_subdirectory(src)
add_subdirectory(apps)
