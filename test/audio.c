#include <alsa/asoundlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#define NO_STD 0
#include <misc.h>

snd_pcm_t *pcm_out;

mdctf_t mdct, imdct;

#define N 1024

i16 buf[N];

#include "../src/plac/quantize.h"
#include "../src/plac/volume.h"

struct quantized q;

void mulaw_compress(f32 *data, size_t len);
void mulaw_expand(f32 *data, size_t len);

cstr url = "机巧少女不会受伤_片尾_旋转吧！雪花.mp3";

f32 vol = 0;

void do_imdct(f32 *block, void *userdata) {
  volume_fine_tuning(block, N, &vol);

  mulaw_compress(block, N);

  q.nbit  = 6;
  q.len   = N;
  q.dataf = block;
  q.datai = buf;
  best_quantize(&q, 0, 16, 0.001);

  dequantize(&q);

  mulaw_expand(block, N);

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
