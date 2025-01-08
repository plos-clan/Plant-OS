add_binary_asm_no_elf(boot mbrboot.asm)

add_efi(boot efi.c -e efi_main)

add_custom_target(
  make ALL
  COMMAND cd "${EXECUTABLE_OUTPUT_PATH}" && "${CMAKE_CURRENT_SOURCE_DIR}/make.sh"
  DEPENDS boot loader kernel apps v86_service ld-plos)
add_custom_target(
  run
  COMMAND cd "${EXECUTABLE_OUTPUT_PATH}" && "${CMAKE_CURRENT_SOURCE_DIR}/run.sh"
  DEPENDS make)

add_custom_target(
  make-efi ALL
  COMMAND cd "${EXECUTABLE_OUTPUT_PATH}" && "${CMAKE_CURRENT_SOURCE_DIR}/make-efi.sh"
  DEPENDS boot-efi)
add_custom_target(
  run-efi
  COMMAND cd "${EXECUTABLE_OUTPUT_PATH}" && "${CMAKE_CURRENT_SOURCE_DIR}/run-efi.sh"
  DEPENDS make-efi)
