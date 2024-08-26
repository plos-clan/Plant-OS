add_exe_link_flag("-static -Wl,-Ttext -Wl,0x288000 -Wl,--section-start=.multiboot2=0x280000 -e _start")
add_binary_exclude(kernel)
target_link_libraries(kernel c-base data-structure fs elf)
target_link_libraries(
  kernel
  font
  plty
  plac
  c-base
  plreadln
  sound)
target_compile_definitions(kernel PRIVATE RING0=1 KERNEL=1)
