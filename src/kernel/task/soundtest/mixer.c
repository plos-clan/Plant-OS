#include <kernel.h>

sound_mixer_t mixer;

void init_sound_mixer() {
  size_t len = 1024;
  f32   *buf = malloc(len * 4);
  mixer      = sound_mixer_alloc(buf, len);
}

void sound_mixer_task() {
  vsound_t snd  = vsound_find("sb16");
  snd->fmt      = SOUND_FMT_S16;
  snd->channels = 1;
  snd->rate     = 44100;
  snd->volume   = 1;
  vsound_open(snd);

  i16 *data = malloc(mixer->len * 2);
  while (1) {
    sound_fmt_conv(data, SOUND_FMT_S16, mixer->buf, SOUND_FMT_F32, mixer->len);
    vsound_write(snd, data, mixer->len);
    sound_mixer_clear(mixer);
    task_next();
  }
  free(data);

  vsound_close(snd);
  syscall_exit(0);
}
