#pragma once
#include "00-include.h"

#define __PACKED__ __attribute__((packed)) // plos kernel 使用

#define _rest __restrict

#undef __THROW
#undef __wur
#undef __nonnull
#define __THROW           __attribute__((nothrow, leaf))
#define __wur             __attribute__((warn_unused_result))
#define __nonnull(params) __attribute__((nonnull params))

#define __attr(...)       __attribute__((__VA_ARGS__))
#define __attr_deprecated __attr(deprecated)
#define __attr_nthrow     __attr(nothrow)
#define __attr_leaf       __attr(leaf)
#define __attr_nnull(...) __attr(nonnull(__VA_ARGS__))
#define __attr_malloc     __attr(warn_unused_result, malloc)
#ifdef __clang__
#  define __attr_dealloc(func, nparam) __attr_malloc
#else
#  define __attr_dealloc(func, nparam) __attr(warn_unused_result, malloc(func, nparam))
#endif

#define deprecated __attr_deprecated

// __attribute__((overloadable)) 是 clang 扩展，使 C 函数可以被重载

#ifdef __cplusplus
#  define overload
#  define dlexport    __attribute__((visibility("default")))
#  define dlimport    extern
#  define dlimport_c  extern "C"
#  define dlimport_x  extern
#  define dlhidden    __attribute__((visibility("hidden")))
#  define dlinternal  __attribute__((visibility("internal")))
#  define dlprotected __attribute__((visibility("protected")))
#else
#  define overload    __attribute__((overloadable))
#  define dlexport    __attribute__((visibility("default")))
#  define dlimport    extern
#  define dlimport_c  extern
#  define dlimport_x  extern overload
#  define dlhidden    __attribute__((visibility("hidden")))
#  define dlinternal  __attribute__((visibility("internal")))
#  define dlprotected __attribute__((visibility("protected")))
#endif
#if DEBUG
#  define finline static
#else
#  define finline static inline __attribute__((always_inline))
#endif
