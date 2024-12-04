add_subdirectory(c)
add_subdirectory(cpp)
add_subdirectory(math)

add_subdirectory(plui)

add_subdirectory(plac)

testexec(ima-adpcm ima-adpcm.c)
testlib(
  ima-adpcm
  util-sound
  asound
  avcodec
  avformat
  avutil
  swresample)

add_subdirectory(algs)
add_subdirectory(misc)

testexec(plmp plmp.c)
testexec(plmp-cpp plmp.cpp)
testlib(plmp-cpp util-libc-base)
