include(../../cmake/use-ubscan)
add_exe_link_flag("-static -Wl,-Ttext=0x288000 -Wl,--section-start=.multiboot2=0x280000 -e _start")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
    add_exe_link_flag("-Wl,--image-base=0x200000")
  endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_exe_link_flag("-Wl,--image-base=0x200000")
endif()
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
