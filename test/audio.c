#include <alsa/asoundlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#define NO_STD 0
#include <misc.h>

snd_pcm_t *pcm_out;

float *mdctf_a(float *s, size_t l, bool r);

typedef struct mdctf {
  bool   inverse;
  size_t len;
  f32   *buf;
  f32   *block;
  size_t bufp;
  f32   *output;
  void (*callback)(f32 *block, void *userdata);
  void *userdata;
} *mdctf_t;

mdctf_t mdctf_alloc(size_t length, bool inverse, void (*callback)(f32 *, void *));
void    mdctf_free(mdctf_t mdct);
void    mdctf_put(mdctf_t mdct, f32 *data, size_t length);
f32    *mdctf_final(mdctf_t mdct);

mdctf_t mdct, imdct;

typedef struct quantized {
  f32    max;
  f32    min;
  size_t nbit;
  size_t len;
  f32   *dataf;
  i32   *datai;
} *quantized_t;

void quantize(quantized_t q) {
  if (q->nbit == 0) goto zero;
  q->max = q->dataf[0];
  q->min = q->dataf[0];
  for (size_t i = 1; i < q->len; i++) {
    if (q->dataf[i] > q->max) q->max = q->dataf[i];
    if (q->dataf[i] < q->min) q->min = q->dataf[i];
  }
  if (q->max == q->min) goto zero;
  const f32 k = ((1 << q->nbit) - 1) / (q->max - q->min);
  for (size_t i = 0; i < q->len; i++) {
    q->datai[i] = (i32)((q->dataf[i] - q->min) * k);
  }
  return;

zero:
  for (size_t i = 0; i < q->len; i++) {
    q->datai[i] = 0;
  }
}

void dequantize(quantized_t q) {
  if (q->nbit == 0) {
    for (size_t i = 0; i < q->len; i++) {
      q->dataf[i] = q->min;
    }
    return;
  }
  f32 k = (q->max - q->min) / ((1 << q->nbit) - 1);
  for (size_t i = 0; i < q->len; i++) {
    q->dataf[i] = q->datai[i] * k + q->min;
  }
}

f32 quantize_diff(quantized_t q) {
  if (q->nbit == 0) {
    f32 diff = 0;
    for (size_t i = 0; i < q->len; i++) {
      diff += (q->dataf[i] - q->min) * (q->dataf[i] - q->min);
    }
    return diff / q->len;
  }
  f32       diff = 0;
  const f32 k    = (q->max - q->min) / ((1 << q->nbit) - 1);
  for (size_t i = 0; i < q->len; i++) {
    f32 d  = q->datai[i] * k + q->min;
    diff  += (q->dataf[i] - d) * (q->dataf[i] - d);
  }
  return diff / q->len;
}

void best_quantize(quantized_t q, size_t from, size_t to, f32 target) {
  for (size_t nbit = from; nbit <= to; nbit++) {
    q->nbit = nbit;
    quantize(q);
    f32 diff = quantize_diff(q);
    if (diff < target) {
      printf("nbit: %d, diff: %f\n", nbit, diff);
      break;
    }
  }
}

f32 diff(f32 *a, f32 *b, size_t len) {
  f32 d = 0;
  for (size_t i = 0; i < len; i++) {
    d += (a[i] - b[i]) * (a[i] - b[i]);
  }
  return d / len;
}

#define N 1024

i32 buf[N];

struct quantized q;

#define MU 1023

f32 mulaw_compress(f32 x) {
  bool sign = x < 0;
  if (sign) x = -x;
  x = log(1 + MU * x) / log(1 + MU);
  return sign ? -x : x;
}

f32 mulaw_expand(f32 x) {
  bool sign = x < 0;
  if (sign) x = -x;
  x = (pow(1 + MU, x) - 1) / MU;
  return sign ? -x : x;
}

cstr url = "audio.mp3";

f32 vol = 0;

void do_imdct(f32 *block, void *userdata) {
  f32 max = 0;
  for (size_t i = 0; i < N; i++) {
    f32 x = fabsf(block[i]);
    if (x > max) max = x;
  }
  vol = vol * .9 + max * .1;
  if (fabsf(block[0]) < 0.01 * vol) block[0] = 0;
  for (size_t i = 1; i < N - 1; i++) {
    bool s = block[i] < 0;
    f32  x = fabsf(block[i]);
    if (x < 0.01 * vol) {
      block[i] = 0;
      continue;
    }
    f32 p = fabsf(block[i - 1]);
    f32 n = fabsf(block[i + 1]);
    if (x > p * 10) {
      x            += p;
      block[i - 1]  = 0;
    }
    if (x > n * 10) {
      x            += n;
      block[i + 1]  = 0;
    }
    block[i] = s ? -x : x;
  }
  if (fabsf(block[N - 1]) < 0.01 * vol) block[N - 1] = 0;

  for (size_t i = 0; i < N; i++) {
    block[i] = mulaw_compress(block[i]);
  }

  q.nbit  = 6;
  q.len   = N;
  q.dataf = block;
  q.datai = buf;
  quantize(&q);
  best_quantize(&q, 0, 16, 0.001);

  // static i32 cnt[256];
  // for (size_t i = 0; i < 256; i++) {
  //   cnt[i] = 1;
  // }
  // for (size_t i = 0; i < N; i++) {
  //   cnt[q.datai[i]]++;
  // }
  // for (size_t i = 0; i < 256; i++) {
  //   printf("%c", cnt[i] > 1 ? '*' : ' ');
  // }
  // printf("\n");

  // printf("%f\n", quantize_diff(&q));

  dequantize(&q);

  for (size_t i = 0; i < N; i++) {
    block[i] = mulaw_expand(block[i]);
  }

  mdctf_put(imdct, block, N);
}

void play_audio(f32 *block, void *userdata) {
  snd_pcm_writei(pcm_out, block, N);
}

int main() {
  mdct  = mdctf_alloc(2 * N, false, do_imdct);
  imdct = mdctf_alloc(2 * N, true, play_audio);

  AVFormatContext *formatContext = null;
  if (avformat_open_input(&formatContext, url, null, null) < 0) return 1;
  if (avformat_find_stream_info(formatContext, null) < 0) {
    avformat_close_input(&formatContext);
    return 1;
  }
  const AVCodec *codec = null;
  int            sid   = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
  if (sid < 0) {
    avformat_close_input(&formatContext);
    return 1;
  }
  AVCodecContext *codecContext = avcodec_alloc_context3(codec);
  if (codecContext == null) {
    avformat_close_input(&formatContext);
    return 1;
  }
  if (avcodec_parameters_to_context(codecContext, formatContext->streams[sid]->codecpar) < 0) {
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return 1;
  }
  if (avcodec_open2(codecContext, codec, null) < 0) {
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return 1;
  }
  AVFrame *frame = av_frame_alloc();
  if (frame == null) {
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return 1;
  }
  snd_pcm_open(&pcm_out, "default", SND_PCM_STREAM_PLAYBACK, 0);
  // snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 2, 44100, 0, .5e6);
  snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED, 1, 44100, 0,
                     .5e6);
  for (AVPacket packet; av_read_frame(formatContext, &packet) >= 0; av_packet_unref(&packet)) {
    if (packet.stream_index != sid) continue;
    if (avcodec_send_packet(codecContext, &packet) < 0) break;
    while (avcodec_receive_frame(codecContext, frame) >= 0) {
      size_t l = frame->linesize[0] / 4;
      mdctf_put(mdct, frame->data[0], l);
      // f32   *x = fftf_r2r_a(frame->data[0], l, false);
      // fftf_r2r(frame->data[0], x, l, true);
      // f32   *x = mdctf_a(frame->data[0], l, false);
      // f32   *y = mdctf_a(x, l, true);
      // snd_pcm_writei(pcm_out, y, l);
      // free(x);
      // free(y);
      av_frame_unref(frame);
    }
  }
  av_frame_free(&frame);
  avcodec_free_context(&codecContext);
  avformat_close_input(&formatContext);
  return 0;
}
