#include <kernel.h>

// plac 测试

#include <audio.h>
#include <sound.h>

finline f32 my_sqrt(f32 x) {
  f32 guess = .6;
  for (size_t i = 0; i < 16; i++) {
    guess = (guess + x / guess) / 2;
  }
  return guess;
}

static void draw(f32 *block, size_t len, void *userdata) {
  static int x = 0;
  struct __PACKED__ {
    byte b, g, r, a;
  } *const buf = vbe_frontbuffer;
  for (int y = 0; y < screen_h; y++) {
    int i    = y * screen_w + x;
    int k    = my_sqrt(1 - y / (f32)screen_h) * len / 2;
    f32 v1   = block[len - k];
    f32 v2   = block[k];
    f32 v    = my_sqrt(v1 * v1 + v2 * v2);
    v        = v > 1 ? 1 : v;
    v        = my_sqrt(v);
    buf[i].r = v * 255;
    buf[i].g = v * 255;
    buf[i].b = v * 255;
  }
  if (++x == screen_w) x = 0;
}

extern sound_mixer_t mixer;

static void play_audio(f32 *block, size_t len, void *track) {
  while (len > 0) {
    size_t nwrite  = sound_mixer_write(mixer, track, block, len);
    block         += nwrite;
    len           -= nwrite;
    if (nwrite == 0) task_next();
  }
}

void sound_test2() {
  klogd("sound test has been started");

  auto  file = vfs_open("/fatfs0/audio.plac");
  byte *buf  = malloc(file->size);
  vfs_read(file, buf, 0, file->size);

  void *track = sound_mixer_track(mixer);

  plac_decompress_t dctx = plac_decompress_alloc(buf, file->size);
  dctx->callback         = play_audio;
  dctx->userdata         = track;
  dctx->cb_mdct_data     = draw;

  u32 samplerate;
  u64 nsamples;
  plac_read_header(dctx, &samplerate, &nsamples);

  waitif(plac_decompress_block(dctx));

  sound_mixer_remove(mixer, track);
  plac_decompress_free(dctx);
  syscall_exit(0);
}
