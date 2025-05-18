#pragma once
#include <define.h>
#include <sound.h>

typedef struct vsound *vsound_t;

typedef int (*vsound_open_t)(vsound_t vsound);
typedef void (*vsound_close_t)(vsound_t vsound);
typedef int (*vsound_func_t)(vsound_t vsound);
typedef int (*vsound_setvol_t)(vsound_t vsound, f32 volume);
typedef ssize_t (*vsound_read_t)(vsound_t vsound, void *addr, size_t len);
typedef ssize_t (*vsound_write_t)(vsound_t vsound, const void *addr, size_t len);
typedef int (*vsound_cb_dma_t)(vsound_t vsound, void *addr);

// vsound
//   open 后 is_using 被设为 true ，close 后 is_using 被设为 false
//   play 后 is_running 被设为 true ，pause 后 is_running 被设为 false
//   drop 后 is_running 被设为 false ，drain 后 is_running 被设为 false

typedef struct vsound {
  bool            is_registed  : 1; // 是否已注册
  bool            is_output    : 1; // 是否是输出设备
  bool            is_using     : 1; // 是否正在使用
  bool            is_rwmode    : 1; // 读写模式还是共享内存模式
  bool            is_dma_ready : 1; // 是否已经开始 DMA
  bool            is_running   : 1; // 是否正在播放或录音
  cstr            name;             // 设备名
  vsound_open_t   open;             // 打开设备
  vsound_close_t  close;            // 关闭设备
  vsound_func_t   play;             // 开始播放
  vsound_func_t   pause;            // 暂停播放
  vsound_func_t   drop;             // 停止播放并丢弃缓冲
  vsound_func_t   drain;            // 等待播放完毕后停止播放
  vsound_read_t   read;             // 读取数据
  vsound_write_t  write;            // 写入数据
  vsound_cb_dma_t start_dma;        // 缓冲区已填满，开始 DMA
  vsound_setvol_t setvol;           // 设置音量
  u32             supported_fmts;   // 支持的采样格式
  u32             supported_rates;  // 支持的采样率
  u32             supported_chs;    // 支持的声道数
  u16             fmt;              // 采样格式
  u16             channels;         // 声道数
  u32             rate;             // 采样率
  f32             volume;           // 音量
  u16             bytes_per_sample; // 每个采样的字节数
  void           *buf;              // 当前使用的缓冲区
  size_t          bufpos;           //
  struct queue    bufs0;            // 空缓冲区的队列
  struct queue    bufs1;            // 已写入缓冲区的队列
  size_t          bufsize;          // 每个缓冲区大小
} *vsound_t;

/**
 *\brief 注册音频设备
 *
 *\param device 要注册的音频设备
 *\return 注册是否成功
 */
bool vsound_regist(vsound_t device);

/**
 *\brief 设置音频设备支持的格式
 *
 *\param device 音频设备
 *\param fmt 支持的格式
 *\return 设置是否成功
 */
bool vsound_set_supported_fmt(vsound_t device, i16 fmt);

/**
 *\brief 设置音频设备支持的采样率
 *
 *\param device 音频设备
 *\param rate 支持的采样率
 *\return 设置是否成功
 */
bool vsound_set_supported_rate(vsound_t device, i32 rate);

/**
 *\brief 设置音频设备支持的通道数
 *
 *\param device 音频设备
 *\param ch 支持的通道数
 *\return 设置是否成功
 */
bool vsound_set_supported_ch(vsound_t device, i16 ch);

/**
 *\brief 设置音频设备支持的多种格式
 *
 *\param device 音频设备
 *\param fmts 支持的格式数组
 *\param len 数组长度
 *\return 设置是否成功
 */
bool vsound_set_supported_fmts(vsound_t device, const i16 *fmts, ssize_t len);

/**
 *\brief 设置音频设备支持的多种采样率
 *
 *\param device 音频设备
 *\param rates 支持的采样率数组
 *\param len 数组长度
 *\return 设置是否成功
 */
bool vsound_set_supported_rates(vsound_t device, const i32 *rates, ssize_t len);

/**
 *\brief 设置音频设备支持的多种通道数
 *
 *\param device 音频设备
 *\param chs 支持的通道数数组
 *\param len 数组长度
 *\return 设置是否成功
 */
bool vsound_set_supported_chs(vsound_t device, const i16 *chs, ssize_t len);

void vsound_addbuf(vsound_t device, void *buf);
void vsound_addbufs(vsound_t device, void *const *bufs, ssize_t len);

int vsound_played(vsound_t snd);      // 播放完一个缓冲区调用一次
int vsound_clearbuffer(vsound_t snd); // 清空所有缓冲区

/**
 *\brief 查找声音对象
 *
 *\param name 声音对象的名称
 *\return vsound_t 返回找到的声音对象
 */
vsound_t vsound_find(cstr name);

/**
 *\brief 打开声音对象
 *
 *\param snd 声音对象
 *\return int 成功返回0，失败返回错误码
 */
int vsound_open(vsound_t snd);

/**
 *\brief 关闭声音对象
 *
 *\param snd 声音对象
 *\return int 成功返回0，失败返回错误码
 */
int vsound_close(vsound_t snd);

/**
 *\brief 播放声音对象
 *
 *\param snd 声音对象
 *\return int 成功返回0，失败返回错误码
 */
int vsound_play(vsound_t snd);

/**
 *\brief 暂停声音对象
 *
 *\param snd 声音对象
 *\return int 成功返回0，失败返回错误码
 */
int vsound_pause(vsound_t snd);

/**
 *\brief 丢弃声音对象
 *
 *\param snd 声音对象
 *\return int 成功返回0，失败返回错误码
 */
int vsound_drop(vsound_t snd);

/**
 *\brief 排空声音对象
 *
 *\param snd 声音对象
 *\return int 成功返回0，失败返回错误码
 */
int vsound_drain(vsound_t snd);

/**
 *\brief 读取声音数据
 *
 *\param snd 声音对象
 *\param data 数据缓冲区
 *\param len 要读取的数据长度
 *\return ssize_t 读取的数据长度，失败返回-1
 */
ssize_t vsound_read(vsound_t snd, void *data, size_t len);

/**
 *\brief 写入声音数据
 *
 *\param snd 声音对象
 *\param data 数据缓冲区
 *\param len 要写入的数据长度
 *\return ssize_t 写入的数据长度，失败返回-1
 */
ssize_t vsound_write(vsound_t snd, const void *data, size_t len);

/**
 *\brief 获取声音对象的音量
 *
 *\param snd 声音对象
 *\return f32 返回音量值
 */
f32 vsound_getvol(vsound_t snd);

/**
 *\brief 设置声音对象的音量
 *
 *\param snd 声音对象
 *\param vol 音量值
 *\return int 成功返回0，失败返回错误码
 */
int vsound_setvol(vsound_t snd, f32 vol);
