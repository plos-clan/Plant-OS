file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library(libc-base STATIC ${c_files} ${cpp_files})
set_target_properties(libc-base PROPERTIES OUTPUT_NAME base)
