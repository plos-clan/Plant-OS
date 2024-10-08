// This code is released under the MIT License

// author: copi143

#include <alsa/asoundlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#define NO_STD 0
#include <data-structure.h>
#include <sound.h>

int main() {
  cstr url = "魔女之旅_片头_リテラチュア.mp3";

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
  if (codecContext->ch_layout.nb_channels > 2) {
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
  AVChannelLayout mono_layout;
  av_channel_layout_default(&mono_layout, 1);
  SwrContext *swr_ctx = null;
  swr_alloc_set_opts2(&swr_ctx, &mono_layout, AV_SAMPLE_FMT_S16, codecContext->sample_rate,
                      &codecContext->ch_layout, codecContext->sample_fmt, codecContext->sample_rate,
                      0, null);
  if (!swr_ctx) {
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return 1;
  }
  if (swr_init(swr_ctx) < 0) {
    swr_free(&swr_ctx);
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return 1;
  }
  snd_pcm_t *pcm_out;
  snd_pcm_open(&pcm_out, "default", SND_PCM_STREAM_PLAYBACK, 0);
  snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1,
                     codecContext->sample_rate, 0, .5e6);
  size_t      bufsize  = 4096;
  void       *buf      = malloc(bufsize);
  size_t      buf2size = 1024;
  void       *buf2     = malloc(buf2size);
  ImaAdpcmCtx ctx1 = {}, ctx2 = {};
  for (AVPacket packet; av_read_frame(formatContext, &packet) >= 0; av_packet_unref(&packet)) {
    if (packet.stream_index != sid) continue;
    if (avcodec_send_packet(codecContext, &packet) < 0) break;
    while (avcodec_receive_frame(codecContext, frame) >= 0) {
      if (frame->nb_samples * 2 > bufsize) {
        bufsize = frame->nb_samples * 2;
        free(buf);
        buf = malloc(bufsize);
      }
      if (frame->nb_samples / 2 > buf2size) {
        buf2size = frame->nb_samples / 2;
        free(buf2);
        buf2 = malloc(buf2size);
      }
      int out_samples = swr_convert(swr_ctx, (uint8_t *const *)&buf, frame->nb_samples,
                                    (const uint8_t **)frame->data, frame->nb_samples);
      if (out_samples <= 0) continue;
      sound_ima_adpcm_encode(&ctx1, buf2, buf, out_samples);
      sound_ima_adpcm_decode(&ctx2, buf, buf2, out_samples);
      snd_pcm_writei(pcm_out, buf, out_samples);
      av_frame_unref(frame);
    }
  }
  snd_pcm_close(pcm_out);
  free(buf2);
  free(buf);
  swr_free(&swr_ctx);
  av_frame_free(&frame);
  avcodec_free_context(&codecContext);
  avformat_close_input(&formatContext);

  return 0;
}
