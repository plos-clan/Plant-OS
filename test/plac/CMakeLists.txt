testexec(audio audio.c)
testlib(
  audio
  util-misc
  util-plac
  asound
  avcodec
  avformat
  avutil
  swresample)

testexec(plac-encoder plac-encoder.c)
testlib(
  plac-encoder
  util-misc
  util-plac
  avcodec
  avformat
  avutil
  swresample)

testexec(plac-player plac-player.c)
testlib(plac-player util-misc util-plac asound)
