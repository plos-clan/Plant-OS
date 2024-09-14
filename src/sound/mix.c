
#pragma GCC optimize("O3")

#include <libc-base.h>
#include <sound.h>

sound_mixer_t sound_mixer_alloc(void *buf, size_t len) {
  sound_mixer_t mixer = malloc(sizeof(struct sound_mixer));
  if (!mixer) return null;
  mixer->buf     = buf;
  mixer->len     = len;
  mixer->tracks  = null;
  mixer->ntracks = 0;
  return mixer;
}

void sound_mixer_free(sound_mixer_t mixer) {
  if (mixer == null) return;
  list_free_with(mixer->tracks, free);
  free(mixer);
}

void sound_mixer_clear(sound_mixer_t mixer) {
  if (mixer == null) return;
  for (size_t i = 0; i < mixer->len; i++) {
    mixer->buf[i] = 0;
  }
  list_foreach(mixer->tracks, track) {
    size_t *len_p = track->data;
    *len_p        = 0;
  }
}

void *sound_mixer_track(sound_mixer_t mixer) {
  size_t *len_p = malloc(mixer->len);
  *len_p        = 0;
  list_prepend(mixer->tracks, len_p);
  return len_p;
}

void sound_mixer_remove(sound_mixer_t mixer, void *track) {
  list_delete(mixer->tracks, track);
}

ssize_t sound_mixer_write(sound_mixer_t mixer, void *track, f32 *data, size_t len) {
  size_t *len_p  = track;
  size_t  nwrite = min(mixer->len - *len_p, len);
  for (size_t i = 0; i < nwrite; i++) {
    mixer->buf[*len_p + i] += data[i];
  }
  *len_p += nwrite;
  return nwrite;
}
