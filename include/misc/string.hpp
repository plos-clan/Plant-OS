#pragma once
#include <libc-base.hpp>

namespace cpp {

template <typename T>
class BaseString {
public:
  class Iterator {
  private:
    BaseString &s;
    size_t      i;

  public:
    auto index() -> size_t {
      return i;
    }

    auto operator<=>(const Iterator &it) {
      return i - it.i;
    }

    auto operator=(size_t i) {
      this->i = i;
    }
  };

  T     *data = null;
  size_t pos  = 0;
  size_t len  = 0;

  BaseString() = default;

  BaseString(const T *str) {
    if (str == null) return;
    len  = strlen(str);
    data = _rc_alloc(T, len + 1);
    strcpy(data, str);
  }

  BaseString(const T *str, size_t len) {
    if (str == null || len == 0) return;
    this->len = len;
    data      = _rc_alloc(T, len + 1);
    strncpy(data, str, len);
  }

  BaseString(const BaseString &s) noexcept : data(s.data), pos(s.pos), len(s.len) {
    _rc_ref(data);
  }

  BaseString(BaseString &&s) noexcept : data(s.data), pos(s.pos), len(s.len) {
    s.data = null;
    s.pos  = 0;
    s.len  = 0;
  }

  ~BaseString() {
    _rc_unref(data);
  }

  auto operator=(const BaseString &s) -> BaseString & {
    if (this == &s) return *this;
    _rc_unref(data);
    data = s.data;
    pos  = s.pos;
    len  = s.len;
    _rc_ref(data);
    return *this;
  }

  auto operator=(BaseString &&s) noexcept -> BaseString & {
    if (this == &s) return *this;
    _rc_unref(data);
    data   = s.data;
    pos    = s.pos;
    len    = s.len;
    s.data = null;
    s.pos  = 0;
    s.len  = 0;
    return *this;
  }

  auto begin() {}

  auto end() {}

  auto size() const -> size_t {
    return len;
  }

  auto length() const -> size_t {
    return len;
  }

  auto c_str() -> T * {
    if (_rc_refcnt(data) > 1) {
      _rc_unref(data);
      T *p = _rc_alloc(T, len + 1);
      strncpy(p, data + pos, len);
      data = p;
      pos  = 0;
    }
    return data;
  }

  auto copy_as_cstr() const -> T * {
    T *s = malloc((len + 1) * sizeof(T));
    if (s == null) return null;
    strncpy(s, data + pos, len);
    return s;
  }

  auto operator[](size_t i) const -> char {
    return data[pos + i];
  }

  auto operator()(size_t i) -> char & {
    if (_rc_refcnt(data) > 1) {
      _rc_unref(data);
      T *p = _rc_alloc(T, len + 1);
      strncpy(p, data + pos, len);
      data = p;
      pos  = 0;
    }
    return data[i];
  }

  auto at(size_t i) const -> char {
    if (i >= len) return 0;
    return data[pos + i];
  }

  auto refat(size_t i) -> char & {
    if (_rc_refcnt(data) > 1) {
      _rc_unref(data);
      T *p = _rc_alloc(T, len + 1);
      strncpy(p, data + pos, len);
      data = p;
      pos  = 0;
    }
    if (i >= len) return data[len];
    return data[i];
  }

  auto substr(size_t start) const -> BaseString && {
    if (start >= len) return BaseString();
    _rc_ref(data);
    return BaseString{data, pos + start, len - start};
  }

  auto substr(size_t start, size_t length) const -> BaseString && {
    if (start >= len) return BaseString();
    length = cpp::min(length, len - start);
    _rc_ref(data);
    return BaseString{data, pos + start, length};
  }

  auto sub(size_t start) const -> BaseString && {
    return substr(start);
  }

  auto sub(size_t start, size_t length) const -> BaseString && {
    return substr(start, length);
  }

  auto repeat(size_t times) const -> BaseString && {
    if (data == null || times == 0) return BaseString();
    T *p = _rc_alloc(T, len * times + 1);
    for (size_t i = 0; i < times; i++) {
      memcpy(p + i * len, data + pos, len);
    }
    p[len * times] = 0;
    return BaseString{p, 0, len * times + 1};
  }
};

using String = BaseString<char>;

#if SAFE_API
static auto utf8_to_32(const u8 *&s) -> u32 {
  u32 code;
  if (s[0] < 0x80) { // ASCII字符（单字节）
    code = *s++;
  } else if ((s[0] & 0xe0) == 0xc0) { // 2字节UTF-8字符
    if ((s[1] & 0xc0) != 0x80) goto err;
    code  = (*s++ & 0x1f) << 6;
    code |= (*s++ & 0x3f);
  } else if ((s[0] & 0xf0) == 0xe0) { // 3字节UTF-8字符
    if ((s[1] & 0xc0) != 0x80 || (s[2] & 0xc0) != 0x80) goto err;
    code  = (*s++ & 0x0F) << 12;
    code |= (*s++ & 0x3f) << 6;
    code |= (*s++ & 0x3f);
  } else if ((s[0] & 0xf8) == 0xf0) { // 4字节UTF-8字符
    if ((s[1] & 0xc0) != 0x80 || (s[2] & 0xc0) != 0x80 || (s[3] & 0xc0) != 0x80) goto err;
    code  = (*s++ & 0x07) << 18;
    code |= (*s++ & 0x3f) << 12;
    code |= (*s++ & 0x3f) << 6;
    code |= (*s++ & 0x3f);
  } else { // 非法的UTF-8序列
    goto err;
  }
  return code;

err:
  s++;
  return 0xfffd; // 跳过非法序列
}
#else
static auto utf8_to_32(const u8 *&s) -> u32 {
  u32 code;
  if (*s < 0x80) { // ASCII字符（单字节）
    code = *s++;
  } else if ((*s & 0xe0) == 0xc0) { // 2字节UTF-8字符
    code  = (*s++ & 0x1f) << 6;
    code |= (*s++ & 0x3f);
  } else if ((*s & 0xf0) == 0xe0) { // 3字节UTF-8字符
    code  = (*s++ & 0x0F) << 12;
    code |= (*s++ & 0x3f) << 6;
    code |= (*s++ & 0x3f);
  } else if ((*s & 0xf8) == 0xf0) { // 4字节UTF-8字符
    code  = (*s++ & 0x07) << 18;
    code |= (*s++ & 0x3f) << 12;
    code |= (*s++ & 0x3f) << 6;
    code |= (*s++ & 0x3f);
  } else { // 非法的UTF-8字节，按单字节处理
    s++;
    return 0xfffd;
  }
  return code;
}
#endif

} // namespace cpp
