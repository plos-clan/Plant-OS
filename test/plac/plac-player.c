// This code is released under the MIT License

// author: copi143

#include <alsa/asoundlib.h>

#define NO_STD 0
#include <audio.h>
#include <data-structure.h>
#include <misc.h>

snd_pcm_t *pcm_out;

f32 volume = 1;

void play_audio(f32 *block, size_t len, void *userdata) {
  if (volume != 1) {
    for (size_t i = 0; i < len; i++) {
      block[i] *= volume;
    }
  }
  snd_pcm_writei(pcm_out, block, len);
}

int main(int argc, char **argv) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s <input.plac> [volume]\n", argv[0]);
    return 1;
  }

  size_t bufsize;
  void  *buf = read_from_file(argv[1], &bufsize);
  if (buf == null) {
    fprintf(stderr, "Failed to read file\n");
    return 1;
  }

  if (argc == 3) {
    volume = atof(argv[2]);
    if (volume < 0 || volume > 2) {
      fprintf(stderr, "Volume must be between 0 and 2\n");
      return 1;
    }
  }

  plac_decompress_t dctx = plac_decompress_alloc(buf, bufsize);
  dctx->callback         = play_audio;
  dctx->userdata         = dctx;

  u32 samplerate;
  u64 nsamples;
  plac_read_header(dctx, &samplerate, &nsamples);

  snd_pcm_open(&pcm_out, "default", SND_PCM_STREAM_PLAYBACK, 0);
  snd_pcm_set_params(pcm_out, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED, 1, samplerate, 0,
                     .5e6);

  waitif(plac_decompress_block(dctx));

  snd_pcm_close(pcm_out);

  return 0;
}
