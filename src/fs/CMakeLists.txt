file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library(fs STATIC ${c_files} ${cpp_files})
target_compile_definitions(fs PRIVATE RING0=1)
