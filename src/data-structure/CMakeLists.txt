file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library(data-structure STATIC ${c_files} ${cpp_files})
