gtest(c-rbtree c/rbtree.c c/rbtree.cpp)

gtest(cpp-rbtree cpp/rbtree.cpp)
gtest(cpp-string cpp/string.cpp)

gtest(pow pow.cpp)
testexec(gaussian-kernel gaussian-kernel.cpp)

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
