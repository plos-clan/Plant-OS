include(../../cmake/use-ubscan)
mklib(c)
tgtlib(c c-base sys)
libdef(c RING3=1 LIBC=1)
