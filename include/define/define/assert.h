// This code is released under the MIT License

#pragma once
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
      _val_;                                                                                       \
    })

#  define assert(expr, ...)                                                                        \
    ({                                                                                             \
      __auto_type _val_ = (expr);                                                                  \
      if (!_val_) fatal("assert " #expr " failed: " __VA_ARGS__);                                  \
      _val_;                                                                                       \
    })

#  define assert_eq(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      __auto_type _val1_ = (expr1);                                                                \
      __auto_type _val2_ = (expr2);                                                                \
      if (!(_val1_ == _val2_)) fatal("assert " #expr1 " == " #expr2 " failed: " __VA_ARGS__);      \
      _va1_;                                                                                       \
    })

#  define assert_ne(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      __auto_type _val1_ = (expr1);                                                                \
      __auto_type _val2_ = (expr2);                                                                \
      if (!(_val1_ != _val2_)) fatal("assert " #expr1 " == " #expr2 " failed: " __VA_ARGS__);      \
      _va1_;                                                                                       \
    })

#else

#  define logged_assert(expr, ...)                                                                 \
    ({                                                                                             \
      const auto &_val_ = (expr);                                                                  \
      if (_val_) {                                                                                 \
        info("assert " #expr " success.");                                                         \
      } else {                                                                                     \
        fatal("assert " #expr " failed: " __VA_ARGS__);                                            \
      }                                                                                            \
      _val_;                                                                                       \
    })

#  define assert(expr, ...)                                                                        \
    ({                                                                                             \
      const auto &_val_ = (expr);                                                                  \
      if (!_val_) fatal("assert " #expr " failed: " __VA_ARGS__);                                  \
      _val_;                                                                                       \
    })

#  define assert_eq(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      const auto &_val1_ = (expr1);                                                                \
      const auto &_val2_ = (expr2);                                                                \
      if (!(_val1_ == _val2_)) fatal("assert " #expr1 " == " #expr2 " failed: " __VA_ARGS__);      \
      _va1_;                                                                                       \
    })

#  define assert_ne(expr1, expr2, ...)                                                             \
    ({                                                                                             \
      const auto &_val1_ = (expr1);                                                                \
      const auto &_val2_ = (expr2);                                                                \
      if (!(_val1_ != _val2_)) fatal("assert " #expr1 " == " #expr2 " failed: " __VA_ARGS__);      \
      _va1_;                                                                                       \
    })

#endif
