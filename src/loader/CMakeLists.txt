include_directories(${PROJECT_SOURCE_DIR}/include/loader)

add_exe_link_flag("-static -Wl,-Ttext -Wl,0x100000 -e loader_main")

add_binary(loader)
