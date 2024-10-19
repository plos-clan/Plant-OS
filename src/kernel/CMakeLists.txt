add_exe_link_flag("-static -Wl,-Ttext -Wl,0x288000 -Wl,--section-start=.multiboot2=0x280000 -e _start")
add_binary_exclude(kernel cpu/v86_service.asm)
target_link_libraries(kernel c-base data-structure fs elf)
target_link_libraries(
  kernel
  font
  plty
  audio
  c-base
  plreadln
  sound
  magic)
target_compile_definitions(kernel PRIVATE RING0=1 KERNEL=1)
add_binary_asm_no_elf(v86_service cpu/v86_service.asm)
