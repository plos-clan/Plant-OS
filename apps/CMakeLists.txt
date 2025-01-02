add_subdirectory(include)

include(../cmake/use-ubscan)

include_directories("${CMAKE_SOURCE_DIR}/apps/include")
link_directories("${CMAKE_SOURCE_DIR}/apps/lib")

add_exe_link_flag("-static -Wl,-Ttext=0x70000000 -e __libc_start_main")

file(
  GLOB SUBDIRS
  RELATIVE "${CMAKE_SOURCE_DIR}/apps"
  "${CMAKE_SOURCE_DIR}/apps/*")
list(REMOVE_ITEM SUBDIRS "include" "lib")
foreach(SUBDIR ${SUBDIRS})
  if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    if(EXISTS "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}/CMakeLists.txt")
      add_subdirectory(${SUBDIR})
      set(TARGETS ${TARGETS} ${SUBDIR})
    endif()
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}/Makefile")
    add_custom_target(
      ${SUBDIR}
      COMMAND make
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    add_custom_target(
      clean-${SUBDIR}
      COMMAND make clean
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    set(TARGETS ${TARGETS} ${SUBDIR})
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}/build.sh")
    add_custom_target(
      ${SUBDIR}
      COMMAND sh build.sh
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    add_custom_target(
      clean-${SUBDIR}
      COMMAND sh build.sh clean
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    set(TARGETS ${TARGETS} ${SUBDIR})
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}/build.py")
    add_custom_target(
      ${SUBDIR}
      COMMAND python build.py
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    add_custom_target(
      clean-${SUBDIR}
      COMMAND python build.py clean
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}")
    set(TARGETS ${TARGETS} ${SUBDIR})
  endif()
endforeach()
add_custom_target(apps DEPENDS ${TARGETS})
