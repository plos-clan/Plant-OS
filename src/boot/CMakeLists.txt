add_binary_asm_no_elf(boot mbrboot.asm)

add_custom_target(
  img ALL
  COMMAND cd "${EXECUTABLE_OUTPUT_PATH}" && "${CMAKE_CURRENT_SOURCE_DIR}/make.sh"
  DEPENDS loader boot kernel apps)
add_custom_target(
  run
  COMMAND cd "${EXECUTABLE_OUTPUT_PATH}" && "${CMAKE_CURRENT_SOURCE_DIR}/run.sh"
  DEPENDS img)
