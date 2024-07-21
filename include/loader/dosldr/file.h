#pragma once
#include <define.h>
#include <type.h>

#define EOF      -1
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct FILE {
  u32   mode;
  u32   fileSize;
  u8   *buffer;
  u32   bufferSize;
  u32   p;
  char *name;
} FILE;