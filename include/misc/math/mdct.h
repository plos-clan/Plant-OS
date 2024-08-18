#pragma once
#include <libc-base.h>

typedef struct mdctf {
  bool   inverse;
  size_t len;
  f32   *buf;
  f32   *block;
  size_t bufp;
  f32   *output;
  void (*callback)(f32 *block, void *userdata);
  void *userdata;
} *mdctf_t;

typedef struct mdct {
  bool   inverse;
  size_t len;
  f64   *buf;
  f64   *block;
  size_t bufp;
  f64   *output;
  void (*callback)(f64 *block, void *userdata);
  void *userdata;
} *mdct_t;

f32    *mdctf_a(f32 *s, size_t l, bool r);
mdctf_t mdctf_alloc(size_t length, bool inverse, void (*callback)(f32 *, void *));
void    mdctf_free(mdctf_t mdct);
void    mdctf_put(mdctf_t mdct, f32 *data, size_t length);
void    mdctf_final(mdctf_t mdct);
f32    *mdctf_block(mdctf_t mdct);

f64   *mdct_a(f64 *s, size_t l, bool r);
mdct_t mdct_alloc(size_t length, bool inverse, void (*callback)(f64 *, void *));
void   mdct_free(mdct_t mdct);
void   mdct_put(mdct_t mdct, f64 *data, size_t length);
void   mdct_final(mdct_t mdct);
f64   *mdct_block(mdct_t mdct);

#if defined(__clang__) && defined(__x86_64__) && !NO_EXTFLOAT
typedef struct mdctl {
  bool   inverse;
  size_t len;
  f128  *buf;
  f128  *block;
  size_t bufp;
  f128  *output;
  void (*callback)(f128 *block, void *userdata);
  void *userdata;
} *mdctl_t;

f128   *mdctl_a(f128 *s, size_t l, bool r);
mdctl_t mdctl_alloc(size_t length, bool inverse, void (*callback)(f128 *, void *));
void    mdctl_free(mdctl_t mdct);
void    mdctl_put(mdctl_t mdct, f128 *data, size_t length);
void    mdctl_final(mdctl_t mdct);
f128   *mdctl_block(mdctl_t mdct);
#endif
