// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "00-include.h"

#ifndef __cplusplus

#  define logged_assert(expr, ...)                                                                 \
    ({                                                                                             \
      __auto_type _val_ = (expr);                                                                  \
      if (_val_) {                                                                                 \
        info("assert " #expr " success.");                                                         \
      } else {                                                                                     \
        fatal("assert " #expr " failed: " __VA_ARGS__);                                            \
      }                                                                                            \
      __builtin_assume(_val_);                                                                     \
      _val_;                                                                                       \
    })

#  define assert(expr, ...)                                                                        \
    ({                                                                                             \
      __auto_type _val_ = (expr);                                                                  \
      if (!_val_) fatal("assert " #expr " failed: " __VA_ARGS__);                                  \
      __builtin_assume(_val_);                                                                     \
      _val_;                                                                                       \
    })

#  define assert_eq(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      __auto_type _val1_ = (expr1);                                                                \
      __auto_type _val2_ = (expr2);                                                                \
      __auto_type _val_  = _val1_ == _val2_;                                                       \
      if (!_val_) fatal("assert " #expr1 " == " #expr2 " failed: " __VA_ARGS__);                   \
      __builtin_assume(_val_);                                                                     \
    })

#  define assert_ne(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      __auto_type _val1_ = (expr1);                                                                \
      __auto_type _val2_ = (expr2);                                                                \
      __auto_type _val_  = _val1_ != _val2_;                                                       \
      if (!_val_) fatal("assert " #expr1 " != " #expr2 " failed: " __VA_ARGS__);                   \
      __builtin_assume(_val_);                                                                     \
    })

#  define TODO(...) ({ fatal("TODO: " __VA_ARGS__); })

#else

#  define logged_assert(expr, ...)                                                                 \
    ({                                                                                             \
      val &_val_ = (expr);                                                                         \
      if (_val_) {                                                                                 \
        info("assert " #expr " success.");                                                         \
      } else {                                                                                     \
        fatal("assert " #expr " failed: " __VA_ARGS__);                                            \
      }                                                                                            \
      __builtin_assume(_val_);                                                                     \
      _val_;                                                                                       \
    })

#  define assert(expr, ...)                                                                        \
    ({                                                                                             \
      val &_val_ = (expr);                                                                         \
      if (!_val_) fatal("assert " #expr " failed: " __VA_ARGS__);                                  \
      __builtin_assume(_val_);                                                                     \
      _val_;                                                                                       \
    })

#  define assert_eq(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      val &_val1_ = (expr1);                                                                       \
      val &_val2_ = (expr2);                                                                       \
      val &_val_  = _val1_ == _val2_;                                                              \
      if (!_val_) fatal("assert " #expr1 " == " #expr2 " failed: " __VA_ARGS__);                   \
      __builtin_assume(_val_);                                                                     \
    })

#  define assert_ne(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      val &_val1_ = (expr1);                                                                       \
      val &_val2_ = (expr2);                                                                       \
      val &_val_  = _val1_ != _val2_;                                                              \
      if (!_val_) fatal("assert " #expr1 " != " #expr2 " failed: " __VA_ARGS__);                   \
      __builtin_assume(_val_);                                                                     \
    })

#  define TODO(...) ({ fatal("TODO: " __VA_ARGS__); })

#endif
