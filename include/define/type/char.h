// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#ifndef __cplusplus
typedef __UINT8_TYPE__ char8_t; // 我们认为 char8 就是 uint8
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef const char     *cstr;
typedef const char8_t  *cstr8;
typedef const char16_t *cstr16;
typedef const char32_t *cstr32;
