#include <kernel.h>

#if 0 // plos 启动音效

#  define buffer_len 32768
static const char l[] = "  QQffQQLLLfLLDDQQQfff  rrff``UU  QQDDQQLLLfLLDD<<f333  r\x98rf`r`U  "
                        "<<<9<LUUU\x80U\x80UL[[rLLL  rrff`frr  <<<93U999`9U3+--9&&&  rrff``UU";

static int s(const char *a, int b, int c) {
  return c / a[b];
}
static int d(int a) {
  return a & 1 ? -1 : 1;
}
static int e(int a, int b, int t) {
  return t >> 22 & 1 ? b : a;
}
static int r(int a, int t) {
  return 112 >> (a >> 20 & 7) & 1 ? t >> 14 & 15 : (t >> 17 & 1) * 8;
}
static int b(const char *a, int p, int t) {
  return (s(a, r(t, t) + p * 16, t << 2) * (254 >> (t >> 20 & 7) & 1)) & 20;
}
static int q(int a) {
  return 120 >> (a >> 20 & 7) & 1 ? 1 : (-a >> 16 & 1);
}
static int g(int t) {
  int p = 178 >> (t >> 19 & 7) & 1;
  int n =
      b("rrLr99rrUUrU99rULLyL<<yy``\x80`@@\x80`", p, t) * 2 +
      b("qqKq88qqTTqT88qTKKxK;;xx__\x7f_??\x7f_", p, t) +
      (s("rf[<r`L@\x98\x88yQ\x80\146UL", (t & 3) + (t >> 17 & 1) * 4 + p * 8, t << 2) & 7) *
          (t * d(t >> 16) >> 12 & 15 ^ e(0, 5, t)) * 3 / 4 +
      ((s("rf[L<9-\x1e&-3&-3-3\xab\x98\x90r`UH0+&9+&\x1d&+\x98\x88yfQL<(3<D3<3-(&"
          "09HL`ULU`r`UL@9",
          (t >> 13 & 31) + p * 32, t << (5 - (t >> 11 & 3))) |
        t >> 8) *
           q(t) &
       31) +
      (((s(l, t >> 14 & 127, t << 6) & s(l, t >> 14 & 127, (t * e(89 / 88, 499 / 498, t)) << 6)) *
        (63486 >> (t >> 15 & 15) & 1) * (102 >> (t >> 20 & 7) & 1)) &
       e(42, 32, t)) +
      ((((253507989 >> (t >> 6 & 31)) * (1 >> (t >> 11 & 3)) * (19593 >> (t >> 13 & 15) & 1) & 1) *
        50) +
       ((((t * t / 21 + t & (t >> 3)) | t >> 7 | t >> 8) - 7) * (3 >> (t >> 11 & 3) & 1) *
            (2450526224 >> (t >> 13 & 31) & 1) &
        31) *
           5 / 2) *
          (112 >> (t >> 20 & 7) & 1);
  return n;
}

static int       F, G;
static const int T = 125000;
static const int K = 10;

static byte gen(int t) {
  if (t < (128 - F) * K) return 128 - t / K;
  t -= (128 - F) * K;
  if (t < T) return g(t);
  t -= T;
  if (t < (128 - G) * K) return G + t / K;
  return 128;
}

vsound_t snd;

void sound_test() {
  klogd("sound test has been started");
  F                      = g(0);
  G                      = g(T);
  const int total        = T + ((128 - F) + (128 - G)) * K;
  snd                    = vsound_find("sb16");
  snd->settings.fmt      = SOUND_FMT_U8;
  snd->settings.channels = 1;
  snd->settings.rate     = 44100;
  snd->settings.volume   = 1;
  vsound_open(snd);
  byte *buffer = malloc(buffer_len);
  for (int offset = 0;; offset++) {
    if (offset * buffer_len >= total) break;
    for (int i = 0; i < buffer_len; i++)
      buffer[i] = gen(i + offset * buffer_len);
    vsound_write(snd, buffer, buffer_len);
  }
  free(buffer);
  vsound_close(snd);
  syscall_exit(0);
}

#else // plac 测试
#  define QOA_IMPLEMENTATION
#  define QOA_NO_STDIO
#  include "../../../audio/qoa.h"
#  include <audio.h>
#  include <sound.h>

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
vsound_t snd;

static void play_audio(f32 *block, size_t len, void *userdata) {
  i16 *data = malloc(len * 2);
  int  rets = sound_fmt_conv(data, SOUND_FMT_S16, block, SOUND_FMT_F32, len);
  vsound_write(snd, data, len);
  free(data);
}
void qoa_player(cstr path) {
  vfs_node_t n = vfs_open(path);
  qoa_desc   qoa;
  void      *buf1 = malloc(n->size);
  vfs_read(n, buf1, 0, n->size);
  short *data = qoa_decode(buf1, n->size, &qoa);
  klogd("data size: %p samples: %d channels: %d samplerate: %d", data, qoa.samples, qoa.channels,
        qoa.samplerate);
  snd           = vsound_find("hda");
  snd->fmt      = SOUND_FMT_S16;
  snd->channels = qoa.channels;
  snd->rate     = qoa.samplerate;
  snd->volume   = 1;
  vsound_open(snd);
  for (int i = 0; i < qoa.samples; i += 2048) {
    klogd("writing %d samples", i);
    vsound_write(snd, data + i * qoa.channels, 2048);
    printf("\r%d/%d sec", (int)((float)i / (float)qoa.samplerate),
           (int)((float)qoa.samples / (float)qoa.samplerate));
  }
  vsound_close(snd);
  free(data);
  free(buf1);
  printf("\n");
}
void plac_player(cstr path) {
  auto  file = vfs_open(path);
  byte *buf  = malloc(file->size);
  vfs_read(file, buf, 0, file->size);

  plac_decompress_t dctx = plac_decompress_alloc(buf, file->size);
  dctx->callback         = play_audio;
  dctx->userdata         = dctx;
  dctx->cb_mdct_data     = draw;

  u32 samplerate;
  u64 nsamples;
  plac_read_header(dctx, &samplerate, &nsamples);

  snd           = vsound_find("hda");
  snd->fmt      = SOUND_FMT_S16;
  snd->channels = 1;
  snd->rate     = samplerate;
  snd->volume   = 1;
  vsound_open(snd);

  waitif(plac_decompress_block(dctx));

  vsound_close(snd);
  plac_decompress_free(dctx);
}
void sound_test() {
  klogd("sound test has been started");
  auto  file = vfs_open("/fatfs1/audio.plac");
  byte *buf  = malloc(file->size);
  vfs_read(file, buf, 0, file->size);

  plac_decompress_t dctx = plac_decompress_alloc(buf, file->size);
  dctx->callback         = play_audio;
  dctx->userdata         = dctx;
  dctx->cb_mdct_data     = draw;

  u32 samplerate;
  u64 nsamples;
  plac_read_header(dctx, &samplerate, &nsamples);

  snd           = vsound_find("hda");
  snd->fmt      = SOUND_FMT_S16;
  snd->channels = 1;
  snd->rate     = samplerate;
  snd->volume   = 1;
  vsound_open(snd);

  waitif(plac_decompress_block(dctx));

  vsound_close(snd);
  plac_decompress_free(dctx);
  syscall_exit(0);
}

#endif
