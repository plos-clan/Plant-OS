#pragma once
#include <data-structure.h>
#include <define.h>

typedef struct sound_mixer {
  f32   *buf;
  size_t bufsize;
  list_t tracks;
  size_t ntracks;
} *sound_mixer_t;

sound_mixer_t sound_mixer_alloc(void *buf, size_t bufsize);
void          sound_mixer_free(sound_mixer_t mixer);
void          sound_mixer_clear(sound_mixer_t mixer);
void         *sound_mixer_track(sound_mixer_t mixer);
void          sound_mixer_remove(sound_mixer_t mixer, void *track);
ssize_t       sound_mixer_write(sound_mixer_t mixer, void *track, f32 *data, size_t len);
