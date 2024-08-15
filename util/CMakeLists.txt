include(src.cmake)

add_executable(util-plff plff.c)
target_link_libraries(util-plff freetype util-font util-data-structure)
