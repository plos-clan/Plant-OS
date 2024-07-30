add_exe_link_flag("-static -Wl,-Ttext -Wl,0x70000000 -e _start")
file(
  GLOB SUBDIRS
  RELATIVE "${CMAKE_SOURCE_DIR}/apps"
  "${CMAKE_SOURCE_DIR}/apps/*")
foreach(SUBDIR ${SUBDIRS})
  if(IS_DIRECTORY ${SUBDIR})
    add_subdirectory(${SUBDIR})
    set(TARGETS ${TARGETS} ${SUBDIR})
  endif()
endforeach()
add_custom_target(apps DEPENDS ${TARGETS})
