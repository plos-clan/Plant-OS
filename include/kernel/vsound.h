#pragma once
#include <define.h>
#include <sound.h>

typedef struct vsound         *vsound_t;
typedef struct sound_settings *sound_settings_t;

typedef int (*vsound_open_t)(vsound_t vsound, sound_settings_t settings);
typedef void (*vsound_close_t)(vsound_t vsound);
typedef int (*vsound_func_t)(vsound_t vsound);
typedef int (*vsound_setvol_t)(vsound_t vsound, f32 volume);
typedef ssize_t (*vsound_read_t)(vsound_t vsound, void *addr, size_t len);
typedef ssize_t (*vsound_write_t)(vsound_t vsound, const void *addr, size_t len);
typedef ssize_t (*vsound_callback_t)(vsound_t vsound, const void *addr, size_t len);

struct sound_settings {
  u16 fmt;              // 采样格式
  u16 channels;         // 声道数
  u32 rate;             // 采样率
  f32 volume;           // 音量
  u16 bytes_per_sample; // 每个采样的字节数
};

typedef struct vsound {
  bool                  is_using;        // 是否正在使用
  bool                  is_registed;     // 是否已注册
  bool                  is_rwmode;       // 读写模式还是共享内存模式
  cstr                  name;            // 设备名
  vsound_open_t         open;            // 打开设备
  vsound_close_t        close;           // 关闭设备
  vsound_func_t         play;            // 开始播放
  vsound_func_t         pause;           // 暂停播放
  vsound_func_t         drop;            // 停止播放并丢弃缓冲
  vsound_func_t         drain;           // 等待播放完毕后停止播放
  vsound_read_t         read;            //
  vsound_write_t        write;           //
  vsound_callback_t     played;          // 音频播放完毕(前)，请求下一段音频
  vsound_setvol_t       setvol;          // 设置音量
  u32                   supported_fmts;  // 支持的采样格式
  u32                   supported_rates; // 支持的采样率
  u32                   supported_chs;   // 支持的声道数
  struct sound_settings settings;        //
  struct queue          bufs;            // 缓冲区队列
  size_t                bufsize;         // 每个缓冲区大小
} *vsound_t;

bool vsound_regist(vsound_t device);
bool vsound_set_supported_fmt(vsound_t device, i16 fmt);
bool vsound_set_supported_rate(vsound_t device, i32 rate);
bool vsound_set_supported_ch(vsound_t device, i16 chs);
bool vsound_set_supported_fmts(vsound_t device, const i16 *fmt, ssize_t len);
bool vsound_set_supported_rates(vsound_t device, const i32 *rate, ssize_t len);
bool vsound_set_supported_chs(vsound_t device, const i16 *chs, ssize_t len);

void vsound_addbuf(vsound_t device, void *buf);

void sb16_init();
void sb16_regist();

vsound_t vsound_find(cstr name);
int      vsound_open(vsound_t name);
int      vsound_close(vsound_t snd);
int      vsound_play(vsound_t snd);
int      vsound_pause(vsound_t snd);
int      vsound_drop(vsound_t snd);
int      vsound_drain(vsound_t snd);
ssize_t  vsound_read(vsound_t snd, void *buf, size_t len);
ssize_t  vsound_write(vsound_t snd, const void *buf, size_t len);
f32      vsound_getvol(vsound_t snd);
int      vsound_setvol(vsound_t snd, f32 vol);
