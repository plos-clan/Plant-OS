// This code is released under the MIT License

#pragma once

#pragma GCC system_header

#include "00-include.h"

// 部分数据结构不应该有 padding
#define __PACKED__   __attribute__((packed))
#define __ALIGN__(x) __attr(packed, aligned(x))
#define __ALIGN2__   __ALIGN__(2)
#define __ALIGN4__   __ALIGN__(4)
#define __ALIGN8__   __ALIGN__(8)
#define __ALIGN16__  __ALIGN__(16)

#define _rest __restrict

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
#define __attr_allocsize(...) __attr(alloc_size(__VA_ARGS__))

#ifdef __cplusplus
#  define __THROW      noexcept(true)
#  define __THROWNL    __attr(nothrow) // 使用 noexcept(true) 时出现奇怪编译错误
#  define __NTH(fct)   __LEAF_ATTR fct __THROW
#  define __NTHNL(fct) fct __THROW
#else
#  define __THROW      __attr(nothrow, leaf)
#  define __THROWNL    __attr(nothrow)
#  define __NTH(fct)   __attr(nothrow, leaf) fct
#  define __NTHNL(fct) __attr(nothrow) fct
#endif

#define __attribute_pure__ __attr(pure)
#define __attr_pure        __attr(pure)
#ifdef __clang__
#  define __attr_access(x)
#  define __attr_readonly(...)
#  define __attr_writeonly(...)
#else
#  define __attr_access(x)      __attr(access x)
#  define __attr_readonly(...)  __attr(access(read_only, ##__VA_ARGS__))
#  define __attr_writeonly(...) __attr(access(write_only, ##__VA_ARGS__))
#endif

#define __nnull(...) __attr(nonnull(__VA_ARGS__))

// C++ 属性
#ifndef __cplusplus
#  define noexcept
#endif

#define __deprecated __attr(deprecated)

#define USED __attr(used)

#define __nif __attr(no_instrument_function)

// __attribute__((overloadable)) 是 clang 扩展，使 C 函数可以被重载

#ifdef __cplusplus
#  define overload
#  define dlexport    __attr(used, visibility("default"))
#  define dlimport    extern
#  define dlimport_c  extern "C"
#  define dlimport_x  extern
#  define dlhidden    __attr(visibility("hidden"))
#  define dlinternal  __attr(visibility("internal"))
#  define dlprotected __attr(visibility("protected"))
#else
#  define overload    __attribute__((overloadable))
#  define dlexport    __attr(used, visibility("default"))
#  define dlimport    extern
#  define dlimport_c  extern
#  define dlimport_x  extern overload
#  define dlhidden    __attr(visibility("hidden"))
#  define dlinternal  __attr(visibility("internal"))
#  define dlprotected __attr(visibility("protected"))
#endif
#if DEBUG
#  define finline static __nif
#else
#  define finline static inline __attr(always_inline) __nif
#endif

#ifdef __cplusplus
#  define inline_const        finline __attr(const) constexpr
#  define inline_const_ncexpr finline __attr(const)
#  define inline_pure         finline __attr(pure)
#  define CONST               __attr(const) constexpr
#  define PURE                __attr(pure)
#  define constfn             __attr(const) constexpr auto
#  define purefn              __attr(pure) auto
#else
#  define inline_const        finline __attr(const)
#  define inline_const_ncexpr finline __attr(const)
#  define inline_pure         finline __attr(pure)
#  define CONST               __attr(const)
#  define PURE                __attr(pure)
#  define constfn             __attr(const)
#  define purefn              __attr(pure)
#endif

#ifdef __cplusplus
#  define INLINE       inline __attr(always_inline) __nif
#  define INLINE_CONST inline __attr(always_inline) __nif __attr(const) constexpr
#else
#  define INLINE       inline __attr(always_inline) __nif
#  define INLINE_CONST inline __attr(always_inline) __nif __attr(const)
#endif

#define NAKED    __attr(naked)
#define CDECL    __attr(cdecl)
#define STDCALL  __attr(stdcall)
#define FASTCALL __attr(fastcall)

#ifdef __clang__
#  define vectorize __attr(vectorize(enable))
#else
#  define vectorize
#endif

// 函数返回内存地址，调用者获得内存所有权
#define ownership_returns(type)       __attr(ownership_returns(type))
// 函数获取内存所有权并释放内存
#define ownership_takes(type, nparam) __attr(ownership_takes(type, nparam))
// 函数获取内存所有权
#define ownership_holds(type, nparam) __attr(ownership_holds(type, nparam))

// 强制函数内所有调用内联
#define FLATTEN __attr(flatten, always_inline) __nif

#define HOT  __attr(hot)  // 手动标记热点函数
#define COLD __attr(cold) // 手动标记不常用函数

#ifdef __clang__
#  define sized_by(...) __attr(sized_by(__VA_ARGS__))
#else
#  define sized_by(...)
#endif
