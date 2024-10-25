#pragma once
#include <define.h>

// 声明

#if NO_STD
#  define IAPI finline
#  define OAPI dlimport
#  define SAPI static
#else
#  define IAPI dlimport
#  define OAPI dlimport
#  define SAPI dlimport
#endif

//; 标准库函数

/**
 *\brief 复制字符串
 * 
 *\param d 目标字符串
 *\param s 源字符串
 *\return 目标字符串
 */
IAPI char *strcpy(char *_rest d, cstr _rest s) __THROW __nnull(1, 2);
IAPI char *strncpy(char *_rest d, cstr _rest s, size_t n) __THROW __nnull(1, 2);
IAPI char *strcat(char *_rest _d, cstr _rest _s) __THROW __nnull(1, 2);
IAPI char *strncat(char *_rest _d, cstr _rest _s, size_t _n) __THROW __nnull(1, 2);
IAPI int   strcmp(cstr _s1, cstr _s2) __THROW;
IAPI int   tolower(int c) __THROW;
IAPI int   toupper(int c) __THROW;
IAPI int   strncmp(cstr _s1, cstr _s2, size_t n) __THROW;
IAPI char *strdup(cstr s) __THROW;
IAPI char *strndup(cstr s, size_t n) __THROW;
#ifdef __cplusplus
extern "C++" {
IAPI cstr  strchr(cstr _s, int _c) __THROW;
IAPI char *strchr(char *_s, int _c) __THROW;
IAPI cstr  strrchr(cstr _s, int _c) __THROW;
IAPI char *strrchr(char *_s, int _c) __THROW;
IAPI cstr  strchrnul(cstr _s, int _c) __THROW;
IAPI char *strchrnul(char *_s, int _c) __THROW;
IAPI cstr  strpbrk(cstr _s, cstr accept) __THROW;
IAPI char *strpbrk(char *_s, cstr accept) __THROW;
IAPI cstr  strstr(cstr _s, cstr _t) __THROW;
IAPI char *strstr(char *_s, cstr _t) __THROW;
IAPI cstr  strcasestr(cstr _s, cstr _t) __THROW;
IAPI char *strcasestr(char *_s, cstr _t) __THROW;
IAPI cstr  basename(cstr __filename) __THROW;
IAPI char *basename(char *__filename) __THROW;
}
#else
IAPI char *strchr(cstr _s, int _c) __THROW;
IAPI char *strrchr(cstr _s, int _c) __THROW;
IAPI char *strchrnul(cstr _s, int _c) __THROW;
IAPI char *strpbrk(cstr _s, cstr accept) __THROW;
IAPI char *strstr(cstr _s, cstr _t) __THROW;
IAPI char *strcasestr(cstr _s, cstr _t) __THROW;
IAPI char *basename(cstr __filename) __THROW;
#endif
IAPI size_t strcspn(cstr s, cstr reject) __THROW;
IAPI size_t strspn(cstr s, cstr accept) __THROW;
IAPI char  *strtok(char *_rest _s, cstr _rest delim) __THROW;
IAPI char  *strtok_r(char *_rest _s, cstr _rest delim, char **_rest save_ptr) __THROW;
IAPI size_t strlen(cstr _s) __THROW;
IAPI size_t strnlen(cstr _s, size_t _l) __THROW;
OAPI char  *strerror(int e) __THROW;
OAPI cstr   strerrordesc_np(int __err) __THROW;
OAPI cstr   strerrorname_np(int __err) __THROW;
IAPI char  *strsep(char **_rest _sp, cstr _rest delim) __THROW;
OAPI char  *strsignal(int __sig) __THROW;
IAPI cstr   sigabbrev_np(int __sig) __THROW;
IAPI cstr   sigdescr_np(int __sig) __THROW;
IAPI char  *stpcpy(char *_rest _d, cstr _rest _s) __THROW;
IAPI char  *stpncpy(char *_rest _d, cstr _rest _s, size_t _n) __THROW;
IAPI int    strverscmp(cstr __s1, cstr __s2) __THROW;
IAPI char  *strfry(char *__string) __THROW;

#if defined __USE_XOPEN2K && !defined __USE_GNU
OAPI int strerror_r(int e, char *buf, size_t n) __THROW; // POSIX 版本
#else
OAPI char *strerror_r(int e, char *buf, size_t n) __THROW; // GNU 版本
#endif

//; 自定义函数

OAPI u32    utf8to32c(cstr8 *sp);
OAPI size_t utf8to32s(u32 *d, cstr8 s);
OAPI u32   *utf8to32a(cstr8 s);
OAPI u32    utf16to32c(cstr16 *sp);
OAPI size_t utf16to32s(u32 *d, cstr16 s);
OAPI u32   *utf16to32a(cstr16 s);
OAPI size_t utf32to8c(u32 c, u8 *s);
OAPI size_t utf32to8s(u8 *d, cstr32 s);
OAPI u8    *utf32to8a(cstr32 s);
OAPI size_t utf32to16c(u32 c, u16 *s);
OAPI size_t utf32to16s(u16 *d, cstr32 s);
OAPI u16   *utf32to16a(cstr32 s);

#undef IAPI
#undef OAPI
#undef SAPI

#define streq(s1, s2)                                                                              \
  ({                                                                                               \
    cstr _s1 = (s1), _s2 = (s2);                                                                   \
    (_s1 && _s2) ? strcmp(_s1, _s2) == 0 : _s1 == _s2;                                             \
  })

#define strneq(s1, s2, n)                                                                          \
  ({                                                                                               \
    cstr _s1 = (s1), _s2 = (s2);                                                                   \
    (_s1 && _s2) ? strncmp(_s1, _s2, n) == 0 : _s1 == _s2;                                         \
  })

#define xstreq(s1, s2)                                                                             \
  ({                                                                                               \
    xstr _s1 = (s1), _s2 = (s2);                                                                   \
    (_s1 && _s2) ? (_s1->hash == _s2->hash ? xstrcmp(_s1, _s2) == 0 : false) : _s1 == _s2;         \
  })

#define memeq(s1, s2, n)                                                                           \
  ({                                                                                               \
    cstr _s1 = (s1), _s2 = (s2);                                                                   \
    (_s1 && _s2) ? memcmp(_s1, _s2, n) == 0 : _s1 == _s2;                                          \
  })

typedef struct _xstr {
  size_t len;
  size_t hash;
  char   data[];
} *xstr;

// 定义

#if NO_STD

finline char *strcpy(char *_rest d, cstr _rest s) noexcept {
#  if __has(strcpy)
  return __builtin_strcpy(d, s);
#  else
  char *_d = d;
  while ((*d++ = *s++) != '\0') {}
  return _d;
#  endif
}

finline char *strncpy(char *_rest d, cstr _rest s, size_t n) noexcept {
#  if __has(strncpy)
  return __builtin_strncpy(d, s, n);
#  else
  char *_d = d;
  cstr  e  = s + n;
  while (s < e && (*d++ = *s++) != '\0') {}
  return _d;
#  endif
}

finline char *strcat(char *_rest d, cstr _rest s) noexcept {
#  if __has(strcat)
  return __builtin_strcat(_d, _s);
#  else
  char *const _d = d;
  while (*d++ != '\0') {}
  d--;
  while ((*d++ = *s++) != '\0') {}
  return _d;
#  endif
}

finline char *strncat(char *_rest d, cstr _rest s, size_t n) noexcept {
#  if __has(strncat)
  return __builtin_strncat(d, _s, _n);
#  else
  char *const _d = d;
  while (*d++ != '\0') {}
  d--;
  cstr e = s + n;
  while (s < e && (*d++ = *s++) != '\0') {}
  return _d;
#  endif
}

finline int strcmp(cstr _s1, cstr _s2) noexcept {
#  if __has(strcmp)
  return __builtin_strcmp(_s1, _s2);
#  else
  __std_safe__({
    if (!_s1 && !_s2) return 0;
    if (!_s1) return -1;
    if (!_s2) return 1;
  });
  auto s1 = (const byte *)_s1;
  auto s2 = (const byte *)_s2;
  int  c1, c2;
  do {
    c1 = *s1++;
    c2 = *s2++;
    if (!c1) return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
#  endif
}

finline int tolower(int c) noexcept {
  return ('A' <= c && c <= 'Z') ? c - 'A' + 'a' : c;
}

finline int toupper(int c) noexcept {
  return ('a' <= c && c <= 'a') ? c - 'a' + 'A' : c;
}

finline int strcasecmp(cstr _s1, cstr _s2) noexcept {
  const byte *s1 = (const byte *)_s1;
  const byte *s2 = (const byte *)_s2;
  int         c1, c2;
  do {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if (!c1) return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
}

finline int strncasecmp(cstr _s1, cstr _s2, size_t n) noexcept {
  const byte *s1 = (const byte *)_s1;
  const byte *e1 = (const byte *)_s1 + n;
  const byte *s2 = (const byte *)_s2;
  int         c1, c2;
  do {
    if (s1 == e1) return 0;
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if (!c1) return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
}

finline int strncmp(cstr _s1, cstr _s2, size_t n) noexcept {
#  if __has(strncmp)
  return __builtin_strncmp(_s1, _s2, n);
#  else
  const byte *s1 = (const byte *)_s1;
  const byte *e1 = (const byte *)_s1 + n;
  const byte *s2 = (const byte *)_s2;
  byte        c1, c2;
  while (s1 != e1) {
    c1 = *s1++;
    c2 = *s2++;
    if (!c1) return (int)c1 - (int)c2;
    if (c1 != c2) return c1 - c2;
  }
  return 0;
#  endif
}

#  ifdef __cplusplus // 参考 C 的实现，复制粘贴而已，C++标准非得要重载
extern "C++" {
finline cstr strchr(cstr _s, int _c) noexcept {
#    if __has(strchr)
  return __builtin_strchr(_s, _c);
#    else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return null;
#    endif
}
finline char *strchr(char *_s, int _c) noexcept {
#    if __has(strchr)
  return __builtin_strchr(_s, _c);
#    else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return null;
#    endif
}

finline cstr strrchr(cstr _s, int _c) noexcept {
#    if __has(strrchr)
  return __builtin_strrchr(_s, _c);
#    else
  cstr finded = null;
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) finded = _s;
  }
  return (char *)finded;
#    endif
}

finline char *strrchr(char *_s, int _c) noexcept {
#    if __has(strrchr)
  return __builtin_strrchr(_s, _c);
#    else
  cstr finded = null;
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) finded = _s;
  }
  return (char *)finded;
#    endif
}

finline cstr strchrnul(cstr _s, int _c) noexcept {
#    if __has(strchrnul)
  return __builtin_strchrnul(_s, _c);
#    else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return (char *)_s;
#    endif
}

finline char *strchrnul(char *_s, int _c) noexcept {
#    if __has(strchrnul)
  return __builtin_strchrnul(_s, _c);
#    else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return (char *)_s;
#    endif
}

finline cstr strpbrk(cstr _s, cstr accept) noexcept {
#    if __has(strpbrk)
  return __builtin_strpbrk(_s, accept);
#    else
  for (; *_s != '\0'; _s++) {
    if (strchr(accept, *_s) != null) return (char *)_s;
  }
  return null;
#    endif
}
finline char *strpbrk(char *_s, cstr accept) noexcept {
#    if __has(strpbrk)
  return __builtin_strpbrk(_s, accept);
#    else
  for (; *_s != '\0'; _s++) {
    if (strchr(accept, *_s) != null) return (char *)_s;
  }
  return null;
#    endif
}

finline cstr strstr(cstr _s, cstr _t) noexcept {
#    if __has(strstr)
  return __builtin_strstr(_s, _t);
#    else
  size_t _sn = strlen(_s), _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s, t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#    endif
}
finline char *strstr(char *_s, cstr _t) noexcept {
#    if __has(strstr)
  return __builtin_strstr(_s, _t);
#    else
  size_t _sn = strlen(_s), _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s, t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#    endif
}

finline cstr strcasestr(cstr _s, cstr _t) noexcept {
#    if __has(strcasestr)
  return __builtin_strcasestr(_s, _t);
#    else
  size_t _sn = strlen(_s), _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s, t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncasecmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#    endif
}
finline char *strcasestr(char *_s, cstr _t) noexcept {
#    if __has(strcasestr)
  return __builtin_strcasestr(_s, _t);
#    else
  size_t _sn = strlen(_s), _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s, t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncasecmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#    endif
}
}
#  else
finline char *strchr(cstr _s, int _c) noexcept {
#    if __has(strchr)
  return __builtin_strchr(_s, _c);
#    else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return null;
#    endif
}

finline char *strrchr(cstr _s, int _c) noexcept {
#    if __has(strrchr)
  return __builtin_strrchr(_s, _c);
#    else
  cstr finded = null;
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) finded = _s;
  }
  return (char *)finded;
#    endif
}

finline char *strchrnul(cstr _s, int _c) noexcept {
#    if __has(strchrnul)
  return __builtin_strchrnul(_s, _c);
#    else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return (char *)_s;
#    endif
}

finline char *strpbrk(cstr _s, cstr accept) noexcept {
#    if __has(strpbrk)
  return __builtin_strpbrk(_s, accept);
#    else
  for (; *_s != '\0'; _s++) {
    if (strchr(accept, *_s) != null) return (char *)_s;
  }
  return null;
#    endif
}

finline char *strstr(cstr _s, cstr _t) noexcept {
#    if __has(strstr)
  return __builtin_strstr(_s, _t);
#    else
  size_t _sn = strlen(_s), _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s, t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#    endif
}

finline char *strcasestr(cstr _s, cstr _t) noexcept {
#    if __has(strcasestr)
  return __builtin_strcasestr(_s, _t);
#    else
  size_t _sn = strlen(_s), _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s, t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncasecmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#    endif
}
#  endif

finline size_t strcspn(cstr s, cstr reject) noexcept {
#  if __has(strcspn)
  return __builtin_strcspn(s, reject);
#  else
  cstr _s = s;
  for (; *s != '\0' && strchr(reject, *s) == null; s++) {}
  return s - _s;
#  endif
}

finline size_t strspn(cstr s, cstr accept) noexcept {
#  if __has(strspn)
  return __builtin_strspn(s, accept);
#  else
  cstr _s = s;
  for (; *s != '\0' && strchr(accept, *s) != null; s++) {}
  return s - _s;
#  endif
}

finline char *strtok(char *_rest _s, cstr _rest delim) noexcept {
#  if __has(strtok)
  return __builtin_strtok(_s, delim);
#  else
  static char *save_ptr;
  return strtok_r(_s, delim, &save_ptr);
#  endif
}

finline char *strtok_r(char *_rest _s, cstr _rest delim, char **_rest save_ptr) noexcept {
#  if __has(strtok_r)
  return __builtin_strtok_r(_s, delim, save_ptr);
#  else
  char *sbegin  = _s ?: *save_ptr;
  sbegin       += strspn(sbegin, delim);
  if (*sbegin == '\0') {
    *save_ptr = sbegin;
    return null;
  }
  char *send = sbegin + strcspn(sbegin, delim);
  if (*send != '\0') {
    *send = '\0';
    send++;
  }
  *save_ptr = send;
  return sbegin;
#  endif
}

finline size_t strlen(cstr _s) noexcept {
#  if __has(strlen)
  return __builtin_strlen(_s);
#  else
  size_t len = 0;
  while (*_s++ != '\0')
    len++;
  return len;
#  endif
}

finline size_t strnlen(cstr _s, size_t _l) noexcept {
#  if __has(strnlen)
  return __builtin_strnlen(_s, _l);
#  else
  size_t len = 0;
  while (*_s++ != '\0' && len < _l) {
    len++;
  }
  return len;
#  endif
}

// char *strerror(int e);

// int strerror_r(int e, char *buf, size_t n);

// extern cstr strerrordesc_np(int __err);

// extern cstr strerrorname_np(int __err);

finline char *strsep(char **_rest _sp, cstr _rest delim) noexcept {
#  if __has(strsep)
  return __builtin_strsep(_sp, delim);
#  else
  char *s = *_sp;
  if (!s) return null;
  char *e = strpbrk(s, delim);
  if (e) {
    *e   = '\0';
    *_sp = e + 1;
  } else {
    *_sp = null;
  }
  return s;
#  endif
}

// extern char *strsignal(int __sig);

// extern cstr sigabbrev_np(int __sig);

// extern cstr sigdescr_np(int __sig);

finline char *stpcpy(char *_rest d, cstr _rest s) noexcept {
#  if __has(stpcpy)
  return __builtin_stpcpy(d, s);
#  else
  while ((*d++ = *s++) != '\0') {}
  return d - 1;
#  endif
}

finline char *stpncpy(char *_rest d, cstr _rest s, size_t n) noexcept {
#  if __has(stpncpy)
  return __builtin_stpncpy(d, s, n);
#  else
  cstr e = s + n;
  while (s < e && (*d++ = *s++) != '\0') {}
  return d - 1;
#  endif
}

// extern int strverscmp(cstr __s1, cstr __s2);

// extern char *strfry(char *__string);

// extern char *basename(cstr __filename);

finline int isspace(int c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

finline int isdigit(int c) {
  return c >= '0' && c <= '9';
}

finline int isalpha(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

finline int isupper(int c) {
  return c >= 'A' && c <= 'Z';
}

#endif

finline size_t strlen8(cstr8 s) {
  size_t len = 0;
  while (*s++ != '\0')
    len++;
  return len;
}
finline size_t strlen16(cstr16 s) {
  size_t len = 0;
  while (*s++ != '\0')
    len++;
  return len;
}
finline size_t strlen32(cstr32 s) {
  size_t len = 0;
  while (*s++ != '\0')
    len++;
  return len;
}

finline void strupper(char *str) {
  for (; *str != '\0'; str++) {
    if ('a' <= *str && *str <= 'z') *str = *str - 'a' + 'A';
  }
}

finline void strlower(char *str) {
  for (; *str != '\0'; str++) {
    if ('A' <= *str && *str <= 'Z') *str = *str - 'A' + 'a';
  }
}
