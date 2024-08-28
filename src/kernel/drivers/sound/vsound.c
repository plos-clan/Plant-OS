#include <kernel.h>

static rbtree_sp_t vsound_list;

bool vsound_regist(vsound_t device) {
  if (device == null) return false;
  if (rbtree_sp_get(vsound_list, device->name)) return false;
  rbtree_sp_insert(vsound_list, device->name, device);
  return true;
}

vsound_t vsound_open(cstr name) { // 打开设备
  vsound_t device = rbtree_sp_get(vsound_list, name);
  if (device->is_using) return null;
  device->open(device);
  device->is_using = true;
  return device;
}

void vsound_close(vsound_t snd) { // 关闭设备
  if (snd == null) return;
  snd->close(snd);
  snd->is_using = false;
}

void vsound_play(vsound_t snd) { // 开始播放
}

void vsound_pause(vsound_t snd) { // 暂停播放
}

void vsound_drop(vsound_t snd) { // 停止播放并丢弃缓冲
}

void vsound_drain(vsound_t snd) { // 等待播放完毕后停止播放
}

ssize_t vsound_read(vsound_t snd, void *buf, size_t len) { // 读取 (录音)
  return -1;
}

ssize_t vsound_write(vsound_t snd, const void *buf, size_t len) { // 写入 (播放)
  return -1;
}
