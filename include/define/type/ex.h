// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "base.h"
#include "int.h"

typedef union {
  struct {
    byte r, g, b, a;
  };
  u32 v;
} color_t;
