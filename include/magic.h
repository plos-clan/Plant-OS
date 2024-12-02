#pragma once
#include <define.h>

enum {
  MIME_UNKNOWN,
  MIME_TEXT,
  MIME_IMAGE,
  MIME_AUDIO,
  MIME_VIDEO,
  MIME_APPLICATION,
  MIME_FONT,
  MIME_MESSAGE,
  MIME_MODEL,
  MIME_MULTIPART,
};

typedef struct filetype {
  const i32  id;   // ID
  const i32  type; // 类型
  const cstr mime; // MIME 类型
  const cstr desc; // 描述
} *filetype_t;

dlimport cstr filetype(const void *data, size_t size);

dlexport cstr filetype_by_ext(cstr ext);
