file(GLOB_RECURSE c_files "*.c")
file(GLOB_RECURSE cpp_files "*.cpp")
addlib(sound ${c_files} ${cpp_files})
tgtlib(sound c-base)
