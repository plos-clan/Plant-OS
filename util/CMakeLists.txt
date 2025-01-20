set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/util)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/util)

include(src.cmake)

add_executable(util-plff plff.c)
target_link_libraries(util-plff freetype util-font util-data-structure)
