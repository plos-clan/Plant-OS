testexec(audio audio.c)
testlib(
  audio
  util-misc
  util-audio
  asound
  avcodec
  avformat
  avutil
  swresample)

testexec(plac-encoder plac-encoder.c)
testlib(
  plac-encoder
  util-misc
  util-audio
  avcodec
  avformat
  avutil
  swresample)

testexec(plac-player plac-player.c)
testlib(plac-player util-misc util-audio asound)
