include(../../cmake/use-ubscan)
add_compile_flag("-fvisibility=hidden") # 防止过多的导出条目 (动态链接)

file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library(misc STATIC ${c_files} ${cpp_files})
target_link_libraries(misc c++ c sys c-base)
target_compile_definitions(misc PRIVATE RING3=1)
