#pragma once
#include <define.h>
#include <type.h>

// 声明

finline char  *strcpy(char *_rest d, cstr _rest s);
finline char  *strncpy(char *_rest d, cstr _rest s, size_t n);
finline char  *strcat(char *_rest _d, cstr _rest _s);
finline char  *strncat(char *_rest _d, cstr _rest _s, size_t _n);
finline int    strcmp(cstr _s1, cstr _s2);
finline int    tolower(int c);
finline int    toupper(int c);
finline int    strncmp(cstr _s1, cstr _s2, size_t n);
finline char  *strdup(cstr _s);
finline char  *strndup(cstr _s, size_t _n);
finline char  *strchr(cstr _s, int _c);
finline char  *strrchr(cstr _s, int _c);
finline char  *strchrnul(cstr _s, int _c);
finline size_t strcspn(cstr __s, cstr __reject);
finline size_t strspn(cstr s, cstr accept);
finline char  *strpbrk(cstr __s, cstr __accept);
finline char  *strstr(cstr _s, cstr _t);
finline char  *strtok(char *_rest __s, cstr _rest __delim);
finline char  *strtok_r(char *_rest __s, cstr _rest __delim, char **_rest __save_ptr);
finline char  *strcasestr(cstr _s, cstr _t);
finline size_t strlen(cstr _s);
finline size_t strnlen(cstr _s, size_t _l);
dlimport char *strerror(int e);
dlimport char *strerror_r(int e, char *buf, size_t n);
dlimport cstr  strerrordesc_np(int __err);
dlimport cstr  strerrorname_np(int __err);
dlimport char *strsep(char **_rest __stringp, cstr _rest __delim);
dlimport char *strsignal(int __sig);
finline cstr   sigabbrev_np(int __sig);
finline cstr   sigdescr_np(int __sig);
finline char  *stpcpy(char *_rest __dest, cstr _rest __src);
finline char  *stpncpy(char *_rest __dest, cstr _rest __src, size_t __n);
finline int    strverscmp(cstr __s1, cstr __s2);
finline char  *strfry(char *__string);
finline char  *basename(cstr __filename);

#define streq(s1, s2) (((s1) && (s2)) ? strcmp(s1, s2) == 0 : (s1) == (s2))

// 定义

#if NO_STD

finline char *strcpy(char *_rest d, cstr _rest s) {
#  if __has(strcpy)
  return __builtin_strcpy(d, s);
#  else
  char *_d = d;
  while ((*d++ = *s++) != '\0') {}
  return _d;
#  endif
}

finline char *strncpy(char *_rest d, cstr _rest s, size_t n) {
#  if __has(strncpy)
  return __builtin_strncpy(d, s, n);
#  else
  char *_d = d;
  cstr  e  = s + n;
  while (s < e && (*d++ = *s++) != '\0') {}
  return _d;
#  endif
}

finline char *strcat(char *_rest _d, cstr _rest _s) {
#  if __has(strcat)
  return __builtin_strcat(_d, _s);
#  else
  while (*_d++ != '\0') {}
  while ((*_d++ = *_s) != '\0') {}
  return _d;
#  endif
}

finline char *strncat(char *_rest _d, cstr _rest _s, size_t _n) {
  // 未完成
  return null;
}

finline int strcmp(cstr _s1, cstr _s2) {
#  if __has(strcmp)
  return __builtin_strcmp(_s1, _s2);
#  else
  __std_safe__({
    if (!_s1 && !_s2) return 0;
    if (!_s1) return -1;
    if (!_s2) return 1;
  });
  const byte *s1 = (const void *)_s1;
  const byte *s2 = (const void *)_s2;
  byte        c1, c2;
  do {
    c1 = *s1++;
    c2 = *s2++;
    if (!c1) return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
#  endif
}

finline int tolower(int c) {
  return ('A' <= c && c <= 'Z') ? c - 'A' + 'a' : c;
}

finline int toupper(int c) {
  return ('a' <= c && c <= 'a') ? c - 'a' + 'A' : c;
}

int strcmp_ci(cstr _s1, cstr _s2); // case-insensitive

finline int strncmp(cstr _s1, cstr _s2, size_t n) {
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

finline char *strchr(cstr _s, int _c) {
#  if __has(strchr)
  return __builtin_strchr(_s, _c);
#  else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return null;
#  endif
}

finline char *strrchr(cstr _s, int _c) {
#  if __has(strrchr)
  return __builtin_strrchr(_s, _c);
#  else
  cstr finded = null;
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) finded = _s;
  }
  return (char *)finded;
#  endif
}

finline char *strchrnul(cstr _s, int _c) {
#  if __has(strchrnul)
  return __builtin_strchrnul(_s, _c);
#  else
  for (; *_s != '\0'; _s++) {
    if (*_s == _c) return (char *)_s;
  }
  return (char *)_s;
#  endif
}

finline size_t strcspn(cstr __s, cstr __reject);

finline size_t strspn(cstr s, cstr accept);

finline char *strpbrk(cstr __s, cstr __accept);

finline char *strstr(cstr _s, cstr _t) {
#  if __has(strstr)
  return __builtin_strstr(_s, _t);
#  else
  size_t _sn = strlen(_s);
  size_t _tn = strlen(_t);
  if (_tn == 0) return (char *)_s;
  if (_sn < _tn) return null;
  cstr s = _s;
  cstr t = _t;
  for (size_t i = 0; i <= _sn - _tn; i++) {
    if (strncmp(s + i, t, _tn) == 0) return (char *)(s + i);
  }
  return null;
#  endif
}

finline char *strtok(char *_rest __s, cstr _rest __delim);

finline char *strtok_r(char *_rest __s, cstr _rest __delim, char **_rest __save_ptr);

finline char *strcasestr(cstr _s, cstr _t);

finline size_t strlen(cstr _s) {
#  if __has(strlen)
  return __builtin_strlen(_s);
#  else
  size_t len = 0;
  while (*_s++ != '\0')
    len++;
  return len;
#  endif
}

finline size_t strnlen(cstr _s, size_t _l) {
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

char *strerror(int e);

char *strerror_r(int e, char *buf, size_t n);

extern cstr strerrordesc_np(int __err);

extern cstr strerrorname_np(int __err);

extern char *strsep(char **_rest __stringp, cstr _rest __delim);

extern char *strsignal(int __sig);

extern cstr sigabbrev_np(int __sig);

extern cstr sigdescr_np(int __sig);

extern char *stpcpy(char *_rest __dest, cstr _rest __src);

extern char *stpncpy(char *_rest __dest, cstr _rest __src, size_t __n);

extern int strverscmp(cstr __s1, cstr __s2);

extern char *strfry(char *__string);

extern char *basename(cstr __filename);

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
