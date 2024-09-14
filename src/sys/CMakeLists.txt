file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library(sys STATIC ${c_files} ${cpp_files})
target_compile_definitions(sys PRIVATE RING3=1)
