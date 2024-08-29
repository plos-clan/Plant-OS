#include <kernel.h>

static rbtree_sp_t vsound_list;

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

static void calc_settings(vsound_t snd) {
  snd->settings.bytes_per_sample = sound_fmt_bytes(snd->settings.fmt) * snd->settings.channels;
}

bool vsound_regist(vsound_t device) {
  if (device == null) return false;
  if (rbtree_sp_get(vsound_list, device->name)) return false;
  rbtree_sp_insert(vsound_list, device->name, device);
  return true;
}

bool vsound_set_supported_fmt(vsound_t device, u16 fmt) {
  if (device == null) return false;
  if (fmt >= SOUND_FMT_CNT) return false;
  device->supported_fmt |= MASK32(fmt);
  return true;
}

bool vsound_set_supported_rate(vsound_t device, u32 rate) {
  if (device == null) return false;
  int id = samplerate_id(rate);
  if (id < 0) return false;
  device->supported_rate |= MASK32(id);
  return true;
}

bool vsound_set_supported_chs(vsound_t device, u16 chs) {
  if (device == null) return false;
  if (chs < 1 || chs > 16) return false;
  device->supported_chs |= MASK32(chs - 1);
  return true;
}

vsound_t vsound_find(cstr name) {
  return rbtree_sp_get(vsound_list, name);
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
  return -1;
}
