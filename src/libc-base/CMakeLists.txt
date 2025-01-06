# include(../../cmake/use-ubscan)
add_compile_flag("-ffreestanding")
mklib(c-base)
libdef(c-base LIBC_BASE=1)
