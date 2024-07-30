add_exe_link_flag("-static -Wl,-Ttext -Wl,0x70000000 -e _start")
add_subdirectory(testapp)
