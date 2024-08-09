file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library(c-base STATIC ${c_files} ${cpp_files})
target_compile_definitions(c-base PRIVATE LIBC_BASE=1)
