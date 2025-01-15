include(../../cmake/use-ubscan)
mklib(misc)
tgtlib(misc c++ c sys c-base)
libdef(misc PRIVATE RING3=1)
