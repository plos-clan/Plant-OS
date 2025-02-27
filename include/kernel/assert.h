// This code is released under the MIT License

#pragma once
#include "00-include.h"
#include "logging.h"

#define kassert(expr, ...)                                                                         \
  ({                                                                                               \
    __auto_type _val_ = (expr);                                                                    \
    if (!_val_) {                                                                                  \
      klogf("assert " #expr " failed: " __VA_ARGS__);                                              \
      infinite_loop {                                                                              \
        asm_cli, asm_hlt;                                                                          \
      }                                                                                            \
    }                                                                                              \
    __builtin_assume(_val_);                                                                       \
    _val_;                                                                                         \
  })
