file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
addlib(plui ${c_files} ${cpp_files})
tgtlib(plui c c++ pl2d)
