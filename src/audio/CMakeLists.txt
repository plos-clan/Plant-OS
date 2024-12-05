if(USE_UBSCAN)
  include(../../cmake/use-ubscan)
endif()

mklib(audio)
tgtlib(audio misc)
libdef(audio RING3=1)
