add_exe_link_flag("-static -Wl,-Ttext=0x100000 -e _start")
add_binary(loader)
target_link_libraries(loader c-base data-structure)
target_compile_definitions(loader PRIVATE RING0=1 LOADER=1)
