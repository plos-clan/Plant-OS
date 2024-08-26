
#pragma GCC optimize("O3")

#include <data-structure.h>
#include <libc-base.h>
#include <sound.h>

typedef struct sound_mixer {
  f32     *buf;
  size_t   bufsize;
  queue_t *tracks;
  size_t   ntracks;
} *sound_mixer_t;

sound_mixer_t sound_mixer_alloc(void *buf, size_t bufsize) {
  sound_mixer_t mixer = malloc(sizeof(struct sound_mixer));
  if (!mixer) return null;
  mixer->buf     = buf;
  mixer->bufsize = bufsize;
  mixer->tracks  = null;
  mixer->ntracks = 0;
  return mixer;
}

void sound_mixer_free(sound_mixer_t mixer) {
  if (mixer == null) return;
  for (size_t i = 0; i < mixer->ntracks; i++) {
    queue_free_with(mixer->tracks[i], free);
  }
  free(mixer->tracks);
  free(mixer);
}

#if 0

// 写入音频数据到混合器的一个轨道
int sound_mixer_write(sound_mixer_t mixer, float *data, size_t num_samples) {
  if (mixer->ntracks >= mixer->max_tracks || num_samples != mixer->num_samples) {
    return -1; // 超出轨道限制或样本数量不匹配
  }

  memcpy(mixer->tracks + mixer->ntracks * mixer->num_samples, data, num_samples * sizeof(float));
  mixer->ntracks++;

  return 0;
}

// 读取混合后的音频数据
void sound_mixer_read(sound_mixer_t mixer, float *output) {
  memset(output, 0, mixer->num_samples * sizeof(float));

  for (size_t i = 0; i < mixer->ntracks; ++i) {
    for (size_t j = 0; j < mixer->num_samples; ++j) {
      output[j] += mixer->tracks[i * mixer->num_samples + j];
    }
  }

  // 平均化音频样本值
  for (size_t j = 0; j < mixer->num_samples; ++j) {
    output[j] /= mixer->ntracks;
  }
}

#endif
