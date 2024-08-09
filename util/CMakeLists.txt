file(GLOB_RECURSE c_files "../src/data-structure/*.c")
file(GLOB_RECURSE cpp_files "../src/data-structure/*.cpp")
add_library(util-data-structure STATIC ${c_files} ${cpp_files})

file(GLOB_RECURSE c_files "../src/font/*.c")
file(GLOB_RECURSE cpp_files "../src/font/*.cpp")
add_library(util-font STATIC ${c_files} ${cpp_files})

add_executable(util-plff plff.c)
target_link_libraries(util-plff freetype util-font util-data-structure)
