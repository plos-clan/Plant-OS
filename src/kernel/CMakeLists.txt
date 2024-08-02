add_exe_link_flag("-static -Wl,-Ttext -Wl,0x280000 -e kernel_main")
add_binary_exclude(kernel)
target_link_libraries(kernel c-base)
target_compile_definitions(kernel PRIVATE KERNEL)
