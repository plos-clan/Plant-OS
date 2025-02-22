add_exe_link_flag("-static -Wl,-Ttext=0x100000 -e _start")
add_compile_flag("-mno-sse -mno-sse2 -mno-sse3")
add_compile_flag("-w") # 由于不再更新，所以忽略警告
add_binary(loader)
target_link_libraries(loader c-base-s data-structure-s ${PROJECT_SOURCE_DIR}/apps/lib/libzstd.a)
target_compile_definitions(loader PRIVATE RING0=1 LOADER=1)
