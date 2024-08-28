#pragma once
#include <define.h>

typedef struct vsound *vsound_t;

typedef int (*vsound_open_t)(vsound_t vsound);
typedef int (*vsound_close_t)(vsound_t vsound);
typedef ssize_t (*vsound_read_t)(vsound_t vsound, void *addr, size_t size);
typedef ssize_t (*vsound_write_t)(vsound_t vsound, const void *addr, size_t size);
typedef ssize_t (*vsound_callback_t)(vsound_t vsound, const void *addr, size_t size);

typedef struct vsound {
  bool              is_using; //
  cstr              name;     //
  vsound_open_t     open;     //
  vsound_close_t    close;    //
  vsound_read_t     read;     //
  vsound_write_t    write;    //
  vsound_callback_t played;   // 音频播放完毕(前)，请求下一段音频
  void             *buf;      // 音频缓冲区
  size_t            bufsize;  //
  void             *userdata; // 可自定义
} *vsound_t;
