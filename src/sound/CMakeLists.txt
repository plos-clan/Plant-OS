include(../../cmake/use-ubscan)
set_optim_flag("-O3 -ffast-math")
mklib(sound ${c_files} ${cpp_files})
tgtlib(sound c-base)
