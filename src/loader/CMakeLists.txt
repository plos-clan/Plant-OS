add_exe_link_flag("-static -Wl,-Ttext -Wl,0x100000 -e loader_main")
add_binary(loader)
target_link_libraries(loader libc-base)
target_compile_definitions(loader PRIVATE LOADER)
