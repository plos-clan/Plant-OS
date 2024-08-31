#include <kernel.h>

static rbtree_sp_t vsound_list;

// 将采样率转换为 id
static int samplerate_id(int rate) {
  switch (rate) {
  case 8000: return 0;
  case 11025: return 1;
  case 16000: return 2;
  case 22050: return 3;
  case 24000: return 4;
  case 32000: return 5;
  case 44100: return 6;
  case 47250: return 7;
  case 48000: return 8;
  case 50000: return 9;
  case 88200: return 10;
  case 96000: return 11;
  case 176400: return 12;
  case 192000: return 13;
  case 352800: return 14;
  case 384000: return 15;
  case 768000: return 16;
  default: return -1;
  }
}

bool vsound_regist(vsound_t device) {
  if (device == null) return false;
  if (device->is_registed || device->is_using) return false;
  if (rbtree_sp_get(vsound_list, device->name)) return false;
  rbtree_sp_insert(vsound_list, device->name, device);
  device->is_registed = true;
  return true;
}

bool vsound_set_supported_fmt(vsound_t device, i16 fmt) {
  if (device == null) return false;
  if (fmt >= SOUND_FMT_CNT) return false;
  device->supported_fmts |= MASK32(fmt);
  return true;
}

bool vsound_set_supported_rate(vsound_t device, i32 rate) {
  if (device == null) return false;
  int id = samplerate_id(rate);
  if (id < 0) return false;
  device->supported_rates |= MASK32(id);
  return true;
}

bool vsound_set_supported_ch(vsound_t device, i16 ch) {
  if (device == null) return false;
  if (ch < 1 || ch > 16) return false;
  device->supported_chs |= MASK32(ch - 1);
  return true;
}

bool vsound_set_supported_fmts(vsound_t device, const i16 *fmts, ssize_t len) {
  if (device == null) return false;
  size_t nseted = 0;
  if (len < 0) {
    for (size_t i = 0; fmts[i] > 0; i++) {
      if (fmts[i] >= SOUND_FMT_CNT) continue;
      device->supported_fmts |= MASK32(fmts[i] - 1);
      nseted++;
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      if (fmts[i] >= SOUND_FMT_CNT) continue;
      device->supported_fmts |= MASK32(fmts[i] - 1);
      nseted++;
    }
  }
  return nseted > 0;
}

bool vsound_set_supported_rates(vsound_t device, const i32 *rates, ssize_t len) {
  if (device == null) return false;
  size_t nseted = 0;
  if (len < 0) {
    for (size_t i = 0; rates[i] > 0; i++) {
      int id = samplerate_id(rates[i]);
      if (id < 0) continue;
      device->supported_rates |= MASK32(id - 1);
      nseted++;
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      int id = samplerate_id(rates[i]);
      if (id < 0) continue;
      device->supported_rates |= MASK32(id - 1);
      nseted++;
    }
  }
  return nseted > 0;
}

bool vsound_set_supported_chs(vsound_t device, const i16 *chs, ssize_t len) {
  if (device == null) return false;
  size_t nseted = 0;
  if (len < 0) {
    for (size_t i = 0; chs[i] > 0; i++) {
      if (chs[i] < 1 || chs[i] > 16) continue;
      device->supported_chs |= MASK32(chs[i] - 1);
      nseted++;
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      if (chs[i] < 1 || chs[i] > 16) continue;
      device->supported_chs |= MASK32(chs[i] - 1);
      nseted++;
    }
  }
  return nseted > 0;
}

vsound_t vsound_find(cstr name) {
  return rbtree_sp_get(vsound_list, name);
}

static void calc_settings(vsound_t snd) {
  snd->settings.bytes_per_sample = sound_fmt_bytes(snd->settings.fmt) * snd->settings.channels;
}

int vsound_open(vsound_t snd) { // 打开设备
  if (snd->is_using) return -1;
  calc_settings(snd);
  snd->open(snd, &snd->settings);
  snd->is_using = true;
  return 0;
}

int vsound_close(vsound_t snd) { // 关闭设备
  if (snd == null) return -1;
  snd->close(snd);
  snd->is_using = false;
  return 0;
}

int vsound_play(vsound_t snd) { // 开始播放
  if (snd && snd->play) return snd->play(snd);
  return -1;
}

int vsound_pause(vsound_t snd) { // 暂停播放
  if (snd && snd->pause) return snd->pause(snd);
  return -1;
}

int vsound_drop(vsound_t snd) { // 停止播放并丢弃缓冲
  if (snd && snd->drop) return snd->drop(snd);
  return -1;
}

int vsound_drain(vsound_t snd) { // 等待播放完毕后停止播放
  if (snd && snd->drain) return snd->drain(snd);
  return -1;
}

ssize_t vsound_read(vsound_t snd, void *buf, size_t len) { // 读取 (录音)
  if (snd && snd->read) return snd->read(snd, buf, len);
  return -1;
}

ssize_t vsound_write(vsound_t snd, const void *buf, size_t len) { // 写入 (播放)
  if (snd && snd->write) return snd->write(snd, buf, len);
  return -1;
}

f32 vsound_getvol(vsound_t snd) {
  if (snd) return snd->settings.volume;
  return -1;
}

int vsound_setvol(vsound_t snd, f32 vol) {
  if (snd && snd->setvol) {
    snd->settings.volume = vol;
    return snd->setvol(snd, vol);
  }
  return -1;
}
