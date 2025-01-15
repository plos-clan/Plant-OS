add_subdirectory(include)

include(../cmake/use-ubscan)

include_directories("${CMAKE_SOURCE_DIR}/apps/include")
include_directories("${CMAKE_SOURCE_DIR}/apps/include/c")
include_directories("${CMAKE_SOURCE_DIR}/apps/include/c++")
link_directories("${CMAKE_SOURCE_DIR}/apps/lib")

add_exe_link_flag("-Wl,--export-dynamic-symbol=main")
add_exe_link_flag("-Wl,-Ttext=0x80000000 -e __libc_start_main")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
    add_exe_link_flag("-Wl,--image-base=0x78000000")
  endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_exe_link_flag("-Wl,--image-base=0x78000000")
endif()

file(
  GLOB SUBDIRS
  RELATIVE "${CMAKE_SOURCE_DIR}/apps"
  "${CMAKE_SOURCE_DIR}/apps/*")
list(REMOVE_ITEM SUBDIRS "include" "lib")
foreach(SUBDIR ${SUBDIRS})
  if(EXISTS "${CMAKE_SOURCE_DIR}/apps/${SUBDIR}/CMakeLists.txt")
    add_subdirectory(${SUBDIR})
    target_sources(${SUBDIR} PRIVATE "${CMAKE_SOURCE_DIR}/apps/crt0.asm")
    set(TARGETS ${TARGETS} ${SUBDIR})
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
