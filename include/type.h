#pragma once
#ifndef NO_CONFIG
#  include <config.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define _CONST_(_type_) typedef const _type_ $##_type_

#ifdef __cplusplus
static const auto null = nullptr;
#else
#  define null 0
#endif

#if NO_STD
#  define NULL 0
#endif

#define $void const void

#ifndef __cplusplus
#  define bool  _Bool
#  define true  ((bool)1)
#  define false ((bool)0)
#endif

_CONST_(bool);

// gcc 扩展
#ifndef __cplusplus
#  define auto __auto_type
#endif

// 获取表达式的类型，类似于 auto
#define typeof(arg) __typeof__((void)0, arg)

#define $auto const auto

#undef INT8_MIN
#undef INT8_MAX
#undef UINT8_MAX
#undef INT16_MIN
#undef INT16_MAX
#undef UINT16_MAX
#undef INT32_MIN
#undef INT32_MAX
#undef UINT32_MAX
#undef INT64_MIN
#undef INT64_MAX
#undef UINT64_MAX

#define INT8_MIN   (-__INT8_MAX__ - 1)
#define INT8_MAX   __INT8_MAX__
#define UINT8_MAX  __UINT8_MAX__
#define INT16_MIN  (-__INT16_MAX__ - 1)
#define INT16_MAX  __INT16_MAX__
#define UINT16_MAX __UINT16_MAX__
#define INT32_MIN  (-__INT32_MAX__ - 1)
#define INT32_MAX  __INT32_MAX__
#define UINT32_MAX __UINT32_MAX__
#define INT64_MIN  (-__INT64_MAX__ - 1)
#define INT64_MAX  __INT64_MAX__
#define UINT64_MAX __UINT64_MAX__

#undef INTMAX_MIN
#undef INTMAX_MAX
#undef UINTMAX_MAX

#define INTMAX_MIN  (-__INTMAX_MAX__ - 1)
#define INTMAX_MAX  __INTMAX_MAX__
#define UINTMAX_MAX __UINTMAX_MAX__

#define I8_MIN  (-__INT8_MAX__ - 1)
#define I8_MAX  __INT8_MAX__
#define U8_MAX  __UINT8_MAX__
#define I16_MIN (-__INT16_MAX__ - 1)
#define I16_MAX __INT16_MAX__
#define U16_MAX __UINT16_MAX__
#define I32_MIN (-__INT32_MAX__ - 1)
#define I32_MAX __INT32_MAX__
#define U32_MAX __UINT32_MAX__
#define I64_MIN (-__INT64_MAX__ - 1)
#define I64_MAX __INT64_MAX__
#define U64_MAX __UINT64_MAX__

#define IMAX_MIN (-__INTMAX_MAX__ - 1)
#define IMAX_MAX __INTMAX_MAX__
#define UMAX_MAX __UINTMAX_MAX__

typedef __INTPTR_TYPE__  intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__  ssize_t;
typedef __UINTPTR_TYPE__ size_t;
typedef __INTPTR_TYPE__  ptrdiff_t;
typedef size_t           usize;
typedef ssize_t          ssize;

_CONST_(intptr_t);
_CONST_(uintptr_t);
_CONST_(ssize_t);
_CONST_(size_t);
_CONST_(ptrdiff_t);
_CONST_(usize);
_CONST_(ssize);

// 在大多数环境下 schar 就是 char
typedef signed char        schar;
typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
typedef long long          llong;
typedef unsigned long long ullong;

_CONST_(schar);
_CONST_(uchar);
_CONST_(ushort);
_CONST_(uint);
_CONST_(ulong);
_CONST_(llong);
_CONST_(ullong);

#ifndef __cplusplus
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef __INT8_TYPE__   int8_t;
typedef __UINT8_TYPE__  uint8_t;
typedef __INT16_TYPE__  int16_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT32_TYPE__  int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT64_TYPE__  int64_t;
typedef __UINT64_TYPE__ uint64_t;
typedef float           float32_t;
typedef double          float64_t;
#if defined(__x86_64__)
typedef __int128          int128_t;
typedef unsigned __int128 uint128_t;
typedef _Float16          float16_t;
typedef __float128        float128_t;
#endif

_CONST_(int8_t);
_CONST_(uint8_t);
_CONST_(int16_t);
_CONST_(uint16_t);
_CONST_(int32_t);
_CONST_(uint32_t);
_CONST_(int64_t);
_CONST_(uint64_t);
_CONST_(float32_t);
_CONST_(float64_t);
#if defined(__x86_64__)
_CONST_(int128_t);
_CONST_(uint128_t);
_CONST_(float16_t);
_CONST_(float128_t);
#endif

typedef __INTMAX_TYPE__  intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

_CONST_(intmax_t);
_CONST_(uintmax_t);

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;
typedef float32_t f32;
typedef float64_t f64;
#if defined(__x86_64__)
typedef int128_t   i128;
typedef uint128_t  u128;
typedef float16_t  f16;
typedef float128_t f128;
#endif

_CONST_(i8);
_CONST_(u8);
_CONST_(i16);
_CONST_(u16);
_CONST_(i32);
_CONST_(u32);
_CONST_(i64);
_CONST_(u64);
_CONST_(f32);
_CONST_(f64);
#if defined(__x86_64__)
_CONST_(i128);
_CONST_(u128);
_CONST_(f16);
_CONST_(f128);
#endif

typedef intmax_t  imax_t;
typedef uintmax_t umax_t;

_CONST_(imax_t);
_CONST_(umax_t);

typedef int errno_t;

_CONST_(errno_t);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 复数

typedef _Complex float  cfloat;
typedef _Complex double cdouble;
typedef _Complex double complex;

_CONST_(cfloat);
_CONST_(cdouble);
_CONST_(complex);

typedef _Complex __INT8_TYPE__   cint8_t;
typedef _Complex __UINT8_TYPE__  cuint8_t;
typedef _Complex __INT16_TYPE__  cint16_t;
typedef _Complex __UINT16_TYPE__ cuint16_t;
typedef _Complex __INT32_TYPE__  cint32_t;
typedef _Complex __UINT32_TYPE__ cuint32_t;
typedef _Complex __INT64_TYPE__  cint64_t;
typedef _Complex __UINT64_TYPE__ cuint64_t;
typedef _Complex float           cfloat32_t;
typedef _Complex double          cfloat64_t;
#if defined(__x86_64__)
typedef _Complex _Float16 cfloat16_t;
#  ifdef __clang__
typedef _Complex __float128 cfloat128_t;
#  endif
#endif

_CONST_(cint8_t);
_CONST_(cuint8_t);
_CONST_(cint16_t);
_CONST_(cuint16_t);
_CONST_(cint32_t);
_CONST_(cuint32_t);
_CONST_(cint64_t);
_CONST_(cuint64_t);
_CONST_(cfloat32_t);
_CONST_(cfloat64_t);
#if defined(__x86_64__)
_CONST_(cfloat16_t);
#  ifdef __clang__
_CONST_(cfloat128_t);
#  endif
#endif

typedef cint8_t    ci8;
typedef cuint8_t   cu8;
typedef cint16_t   ci16;
typedef cuint16_t  cu16;
typedef cint32_t   ci32;
typedef cuint32_t  cu32;
typedef cint64_t   ci64;
typedef cuint64_t  cu64;
typedef cfloat32_t cf32;
typedef cfloat64_t cf64;
#if defined(__x86_64__)
typedef cfloat16_t cf16;
#  ifdef __clang__
typedef cfloat128_t cf128;
#  endif
#endif

_CONST_(ci8);
_CONST_(cu8);
_CONST_(ci16);
_CONST_(cu16);
_CONST_(ci32);
_CONST_(cu32);
_CONST_(ci64);
_CONST_(cu64);
_CONST_(cf32);
_CONST_(cf64);
#if defined(__x86_64__)
_CONST_(cf16);
#  ifdef __clang__
_CONST_(cf128);
#  endif
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 编译器内置向量类型

#define __(type, len) typedef type vec##len##type __attribute__((vector_size(sizeof(type) * len)))

#define __8(type)                                                                                  \
  __(type, 2);                                                                                     \
  __(type, 4);                                                                                     \
  __(type, 8);
#define __16(type)                                                                                 \
  __8(type);                                                                                       \
  __(type, 16);
#define __32(type)                                                                                 \
  __16(type);                                                                                      \
  __(type, 32);
#define __64(type)                                                                                 \
  __32(type);                                                                                      \
  __(type, 64);
#define __128(type)                                                                                \
  __64(type);                                                                                      \
  __(type, 128);

__128(int8_t);
__128(uint8_t);
__64(int16_t);
__64(uint16_t);
__32(int32_t);
__32(uint32_t);
__16(int64_t);
__16(uint64_t);
__32(float32_t);
__16(float64_t);
#if defined(__x86_64__)
__8(int128_t);
__8(uint128_t);
__64(float16_t);
__8(float128_t);
#endif

__32(float);
__16(double);

#undef __16
#undef __32
#undef __64
#undef __128

#undef __

//~ 自定义

#define __(type, len) typedef type v##len##type __attribute__((vector_size(sizeof(type) * len)))

#define __8(type)                                                                                  \
  __(type, 2);                                                                                     \
  __(type, 4);                                                                                     \
  __(type, 8);
#define __16(type)                                                                                 \
  __8(type);                                                                                       \
  __(type, 16);
#define __32(type)                                                                                 \
  __16(type);                                                                                      \
  __(type, 32);
#define __64(type)                                                                                 \
  __32(type);                                                                                      \
  __(type, 64);
#define __128(type)                                                                                \
  __64(type);                                                                                      \
  __(type, 128);

__128(i8);
__128(u8);
__64(i16);
__64(u16);
__32(i32);
__32(u32);
__16(i64);
__16(u64);
__32(f32);
__16(f64);
#if defined(__x86_64__)
__8(i128);
__8(u128);
__64(f16);
__8(f128);
#endif

#undef __16
#undef __32
#undef __64
#undef __128

#undef __

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 标准库原子操作部分

// 看到底下 NO_STD || !defined(__cplusplus) 了没
// 都是为了和标准库能够共存搞的

#ifndef __cplusplus
typedef enum {
  // 最低限度的内存顺序语义。
  // 在这种模式下，编译器和处理器可以采取任意措施来优化性能，并不需要遵循任何顺序。
  memory_order_relaxed = __ATOMIC_RELAXED,
  // 用于消费者-生产者同步。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  memory_order_consume = __ATOMIC_CONSUME,
  // 用于加锁操作。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  memory_order_acquire = __ATOMIC_ACQUIRE,
  // 用于解锁操作。
  // 写入操作会等待先前的读取操作完成，并且不会影响后续的写入操作。
  memory_order_release = __ATOMIC_RELEASE,
  // 组合了 acquire 和 release 的语义。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  // 写入操作会等待先前的读取操作完成，并且不会影响后续的写入操作。
  memory_order_acq_rel = __ATOMIC_RELEASE,
  // 严格的顺序语义。
  // 读写操作会按照程序中的顺序执行，并且不会被重排序。
  memory_order_seq_cst = __ATOMIC_SEQ_CST,
} memory_order;
#endif

#ifdef __cplusplus
enum class memory_order : int {
  // 最低限度的内存顺序语义。
  // 在这种模式下，编译器和处理器可以采取任意措施来优化性能，并不需要遵循任何顺序。
  relaxed = __ATOMIC_RELAXED,
  // 用于消费者-生产者同步。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  consume = __ATOMIC_CONSUME,
  // 用于加锁操作。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  acquire = __ATOMIC_ACQUIRE,
  // 用于解锁操作。
  // 写入操作会等待先前的读取操作完成，并且不会影响后续的写入操作。
  release = __ATOMIC_RELEASE,
  // 组合了 acquire 和 release 的语义。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  // 写入操作会等待先前的读取操作完成，并且不会影响后续的写入操作。
  acq_rel = __ATOMIC_RELEASE,
  // 严格的顺序语义。
  // 读写操作会按照程序中的顺序执行，并且不会被重排序。
  seq_cst = __ATOMIC_SEQ_CST,
};

inline constexpr auto memory_order_relaxed = memory_order::relaxed;
inline constexpr auto memory_order_consume = memory_order::consume;
inline constexpr auto memory_order_acquire = memory_order::acquire;
inline constexpr auto memory_order_release = memory_order::release;
inline constexpr auto memory_order_acq_rel = memory_order::acq_rel;
inline constexpr auto memory_order_seq_cst = memory_order::seq_cst;
#endif

enum {
  // 最低限度的内存顺序语义。
  // 在这种模式下，编译器和处理器可以采取任意措施来优化性能，并不需要遵循任何顺序。
  atom_relaxed = __ATOMIC_RELAXED,
  // 用于消费者-生产者同步。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  atom_consume = __ATOMIC_CONSUME,
  // 用于加锁操作。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  atom_acquire = __ATOMIC_ACQUIRE,
  // 用于解锁操作。
  // 写入操作会等待先前的读取操作完成，并且不会影响后续的写入操作。
  atom_release = __ATOMIC_RELEASE,
  // 组合了 acquire 和 release 的语义。
  // 读取操作会等待先前的写入操作完成，并且不会影响后续的读取操作。
  // 写入操作会等待先前的读取操作完成，并且不会影响后续的写入操作。
  atom_acq_rel = __ATOMIC_RELEASE,
  // 严格的顺序语义。
  // 读写操作会按照程序中的顺序执行，并且不会被重排序。
  atom_seq_cst = __ATOMIC_SEQ_CST,
};

#if !(defined(__cplusplus) && defined(__GNUC__) && !defined(__clang__))
#  if NO_STD || !defined(__cplusplus)
// 消除一种类型为 T 的数据类型的依赖关系
#    define kill_dependency(x)                                                                     \
      ({                                                                                           \
        auto __tmp = (x);                                                                          \
        __tmp;                                                                                     \
      })
#  endif
#endif

#define ATOMIC_VAR_INIT(val) (val)

#if NO_STD || !defined(__cplusplus)
// 用VAL初始化由PTR指向的原子对象。
#  define atomic_init(ptr, val) __atom_store(ptr, val, __atom_relaxed)
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 原子类型

#if NO_STD || !defined(__cplusplus)
#  define atomic _Atomic
#endif

#if defined(__cplusplus) && defined(__GNUC__) && !defined(__clang__)

extern "C++" {
#  include <bits/atomic_base.h>
using std::__atomic_base;
}

typedef __atomic_base<bool>               atomic_bool;
typedef __atomic_base<char>               atomic_char;
typedef __atomic_base<signed char>        atomic_schar;
typedef __atomic_base<unsigned char>      atomic_uchar;
typedef __atomic_base<short>              atomic_short;
typedef __atomic_base<unsigned short>     atomic_ushort;
typedef __atomic_base<int>                atomic_int;
typedef __atomic_base<unsigned int>       atomic_uint;
typedef __atomic_base<long>               atomic_long;
typedef __atomic_base<unsigned long>      atomic_ulong;
typedef __atomic_base<long long>          atomic_llong;
typedef __atomic_base<unsigned long long> atomic_ullong;

typedef __atomic_base<int8_t>   atomic_int8_t;
typedef __atomic_base<uint8_t>  atomic_uint8_t;
typedef __atomic_base<int16_t>  atomic_int16_t;
typedef __atomic_base<uint16_t> atomic_uint16_t;
typedef __atomic_base<int32_t>  atomic_int32_t;
typedef __atomic_base<uint32_t> atomic_uint32_t;
typedef __atomic_base<int64_t>  atomic_int64_t;
typedef __atomic_base<uint64_t> atomic_uint64_t;
#  if defined(__x86_64__)
typedef __atomic_base<int128_t>  atomic_int128_t;
typedef __atomic_base<uint128_t> atomic_uint128_t;
#  endif

typedef __atomic_base<intmax_t>  atomic_intmax_t;
typedef __atomic_base<uintmax_t> atomic_uintmax_t;

typedef __atomic_base<char16_t> atomic_char16_t;
typedef __atomic_base<char32_t> atomic_char32_t;
typedef __atomic_base<wchar_t>  atomic_wchar_t;

typedef __atomic_base<intptr_t>  atomic_intptr_t;
typedef __atomic_base<uintptr_t> atomic_uintptr_t;
typedef __atomic_base<ssize_t>   atomic_ssize_t;
typedef __atomic_base<size_t>    atomic_size_t;
typedef __atomic_base<ptrdiff_t> atomic_ptrdiff_t;

typedef __atomic_base<bool> atomic_flag_t;

#else

typedef _Atomic bool               atomic_bool;
typedef _Atomic char               atomic_char;
typedef _Atomic signed char        atomic_schar;
typedef _Atomic unsigned char      atomic_uchar;
typedef _Atomic short              atomic_short;
typedef _Atomic unsigned short     atomic_ushort;
typedef _Atomic int                atomic_int;
typedef _Atomic unsigned int       atomic_uint;
typedef _Atomic long               atomic_long;
typedef _Atomic unsigned long      atomic_ulong;
typedef _Atomic long long          atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;

typedef _Atomic int8_t    atomic_int8_t;
typedef _Atomic uint8_t   atomic_uint8_t;
typedef _Atomic int16_t   atomic_int16_t;
typedef _Atomic uint16_t  atomic_uint16_t;
typedef _Atomic int32_t   atomic_int32_t;
typedef _Atomic uint32_t  atomic_uint32_t;
typedef _Atomic int64_t   atomic_int64_t;
typedef _Atomic uint64_t  atomic_uint64_t;
#  if defined(__x86_64__)
typedef _Atomic int128_t  atomic_int128_t;
typedef _Atomic uint128_t atomic_uint128_t;
#  endif

typedef _Atomic intmax_t  atomic_intmax_t;
typedef _Atomic uintmax_t atomic_uintmax_t;

typedef _Atomic char16_t atomic_char16_t;
typedef _Atomic char32_t atomic_char32_t;
typedef _Atomic wchar_t  atomic_wchar_t;

typedef _Atomic intptr_t  atomic_intptr_t;
typedef _Atomic uintptr_t atomic_uintptr_t;
typedef _Atomic ssize_t   atomic_ssize_t;
typedef _Atomic size_t    atomic_size_t;
typedef _Atomic ptrdiff_t atomic_ptrdiff_t;

typedef _Atomic bool atomic_flag_t;

#endif

#if NO_STD || !defined(__cplusplus)
#  define atomic_thread_fence(mo)  __atomic_thread_fence(mo)
#  define atomic_signal_fence(mo)  __atomic_signal_fence(mo)
#  define atomic_is_lock_free(obj) __atomic_is_lock_free(sizeof(*(obj)), (obj))
#endif

#if !(defined(__cplusplus) && defined(__GNUC__) && !defined(__clang__))
#  define ATOMIC_FLAG_INIT false
#endif

//~ 自定义的

typedef atomic_bool   atom_bool;
typedef atomic_char   atom_char;
typedef atomic_schar  atom_schar;
typedef atomic_uchar  atom_uchar;
typedef atomic_short  atom_short;
typedef atomic_ushort atom_ushort;
typedef atomic_int    atom_int;
typedef atomic_uint   atom_uint;
typedef atomic_long   atom_long;
typedef atomic_ulong  atom_ulong;
typedef atomic_llong  atom_llong;
typedef atomic_ullong atom_ullong;

typedef atomic_int8_t   atom_i8;
typedef atomic_uint8_t  atom_u8;
typedef atomic_int16_t  atom_i16;
typedef atomic_uint16_t atom_u16;
typedef atomic_int32_t  atom_i32;
typedef atomic_uint32_t atom_u32;
typedef atomic_int64_t  atom_i64;
typedef atomic_uint64_t atom_u64;
#if defined(__x86_64__)
typedef atomic_int128_t  atom_i128;
typedef atomic_uint128_t atom_u128;
#endif

typedef atomic_intmax_t  atom_imax_t;
typedef atomic_uintmax_t atom_umax_t;

typedef atomic_char16_t atom_char16_t;
typedef atomic_char32_t atom_char32_t;
typedef atomic_wchar_t  atom_wchar_t;

typedef atomic_intptr_t  atom_iptr_t;
typedef atomic_uintptr_t atom_uptr_t;
typedef atomic_ssize_t   atom_ssize_t;
typedef atomic_size_t    atom_size_t;
typedef atomic_ptrdiff_t atom_pdiff_t;

#define atom_thread_fence(mo)  __atomic_thread_fence(mo)
#define atom_signal_fence(mo)  __atomic_signal_fence(mo)
#define atom_is_lock_free(obj) __atomic_is_lock_free(sizeof(*(obj)), (obj))

typedef atomic_flag_t atom_flag_t;

#define ATOM_FLAG_INIT false

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 是否无需上锁

#define ATOMIC_BOOL_LOCK_FREE     __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE     __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE  __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE    __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE      __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE     __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE    __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE  __GCC_ATOMIC_POINTER_LOCK_FREE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 原子函数

/* 请注意，这些宏需要使用 auto 来去除 _Atomic 限定符（如果这些限定符
   适用于宏操作数，则也要去除 const 限定符）。

   还请注意，头文件使用 __atomic 的通用形式内置函数，这要求对值参数取地址，然
   后我们传递该值。这允许宏适用于任何类型，并且编译器足够聪明，可以将这些转换为
   无锁 _N 变体，并丢弃临时变量。  */

// 原子地读取 ptr 指向的变量
#define __atom_load(ptr, mo)                                                                       \
  ({                                                                                               \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __val;                                                                          \
    __atomic_load(__ptr, &__val, mo);                                                              \
    __val;                                                                                         \
  })
// 原子地将 ptr 指向的变量设为 val
#define __atom_store(ptr, val, mo)                                                                 \
  ({                                                                                               \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __val = (val);                                                                  \
    __atomic_store(__ptr, &__val, mo);                                                             \
  })

// 原子地将 ptr 指向的变量加上 val，并返回旧值
#define __atom_add(ptr, val, mo) __atomic_fetch_add(ptr, val, mo)
// 原子地将 ptr 指向的变量减去 val，并返回旧值
#define __atom_sub(ptr, val, mo) __atomic_fetch_sub(ptr, val, mo)
// 原子地将 ptr 指向的变量按位或上 val，并返回旧值
#define __atom_or(ptr, val, mo)  __atomic_fetch_or(ptr, val, mo)
// 原子地将 ptr 指向的变量按位与上 val，并返回旧值
#define __atom_and(ptr, val, mo) __atomic_fetch_and(ptr, val, mo)
// 原子地将 ptr 指向的变量按位异或上 val，并返回旧值
#define __atom_xor(ptr, val, mo) __atomic_fetch_xor(ptr, val, mo)

// 用于原子地设置 ptr 指向的变量的值为 1，并返回该变量之前的值
#define __atom_tas(ptr, mo) __atomic_test_and_set(ptr, mo)
// 原子地将 ptr 指向的变量的值设为 0
#define __atom_clr(ptr, mo) __atomic_clear(ptr, mo)

// 原子地将 ptr 指向的变量的值设为 val，并返回该变量之前的值
#define __atom_exch(ptr, val, mo)                                                                  \
  __extension__({                                                                                  \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __val = (val);                                                                  \
    typeof(*__ptr) __tmp;                                                                          \
    __atomic_exchange(__ptr, &__val, &__tmp, mo);                                                  \
    __tmp;                                                                                         \
  })

// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
#define __atom_cexch(ptr, exp, des, smo, fmo)                                                      \
  ({                                                                                               \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __des = (des);                                                                  \
    __atomic_compare_exchange(__ptr, exp, &__des, 0, smo, fmo);                                    \
  })
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
#define __atom_cexch_weak(ptr, exp, des, smo, fmo)                                                 \
  ({                                                                                               \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __des = (des);                                                                  \
    __atomic_compare_exchange(__ptr, exp, &__des, 1, smo, fmo);                                    \
  })

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 原子函数

#if NO_STD || !defined(__cplusplus)
// 原子地读取 ptr 指向的变量
#  define atomic_load_explicit(ptr, mo) __atom_load(ptr, mo)
// 原子地读取 ptr 指向的变量
// 保证严格的顺序语义。
#  define atomic_load(ptr)              __atom_load(ptr, atom_seq_cst)

// 原子地将 ptr 指向的变量设为 val
#  define atomic_store_explicit(ptr, val, mo) __atom_store(ptr, val, mo)
// 原子地将 ptr 指向的变量设为 val
// 保证严格的顺序语义。
#  define atomic_store(ptr, val)              __atom_store(ptr, val, atom_seq_cst)

// 原子地将 ptr 指向的变量的值设为 val，并返回该变量之前的值
#  define atomic_exchange_explicit(ptr, val, mo) __atom_exch(ptr, val, mo)
// 原子地将 ptr 指向的变量的值设为 val，并返回该变量之前的值
// 保证严格的顺序语义。
#  define atomic_exchange(ptr, val)              __atom_exch(ptr, val, atom_seq_cst)

// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
#  define atomic_compare_exchange_strong_explicit(ptr, val, des, smo, fmo)                         \
    __atom_cexch(ptr, val, des, smo, fmo)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#  define atomic_compare_exchange_strong(ptr, val, des)                                            \
    __atom_cexch(ptr, val, des, atom_seq_cst, atom_seq_cst)

// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
#  define atomic_compare_exchange_weak_explicit(ptr, val, des, smo, fmo)                           \
    __atom_cexch_weak(ptr, val, des, smo, fmo)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#  define atomic_compare_exchange_weak(ptr, val, des)                                              \
    __atom_cexch_weak(ptr, val, des, atom_seq_cst, atom_seq_cst)

// 原子地将 ptr 指向的变量加上 val，并返回旧值
#  define atomic_fetch_add_explicit(ptr, val, mo) __atom_add(ptr, val, mo)
// 原子地将 ptr 指向的变量加上 val，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_add(ptr, val)              __atom_add(ptr, val, atom_seq_cst)

// 原子地将 ptr 指向的变量减去 val，并返回旧值
#  define atomic_fetch_sub_explicit(ptr, val, mo) __atom_sub(ptr, val, mo)
// 原子地将 ptr 指向的变量减去 val，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_sub(ptr, val)              __atom_sub(ptr, val, atom_seq_cst)

// 原子地将 ptr 指向的变量按位或上 val，并返回旧值
#  define atomic_fetch_or_explicit(ptr, val, mo) __atom_or(ptr, val, mo)
// 原子地将 ptr 指向的变量按位或上 val，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_or(ptr, val)              __atom_or(ptr, val, atom_seq_cst)

// 原子地将 ptr 指向的变量按位与上 val，并返回旧值
#  define atomic_fetch_and_explicit(ptr, val, mo) __atom_and(ptr, val, mo)
// 原子地将 ptr 指向的变量按位与上 val，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_and(ptr, val)              __atom_and(ptr, val, atom_seq_cst)

// 原子地将 ptr 指向的变量按位异或上 val，并返回旧值
#  define atomic_fetch_xor_explicit(ptr, val, mo) __atom_xor(ptr, val, mo)
// 原子地将 ptr 指向的变量按位异或上 val，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_xor(ptr, val)              __atom_xor(ptr, val, atom_seq_cst)

// 用于原子地设置 ptr 指向的变量的值为 1，并返回该变量之前的值
#  define atomic_flag_test_and_set_explicit(ptr, mo) __atom_tas(ptr, mo)
// 用于原子地设置 ptr 指向的变量的值为 1，并返回该变量之前的值
// 保证严格的顺序语义。
#  define atomic_flag_test_and_set(ptr)              __atom_tas(ptr, atom_seq_cst)

// 原子地将 ptr 指向的变量的值设为 0
#  define atomic_flag_clear_explicit(ptr, MO) __atom_clr(ptr, mo)
// 原子地将 ptr 指向的变量的值设为 0
// 保证严格的顺序语义。
#  define atomic_flag_clear(ptr)              __atom_clr(ptr, atom_seq_cst)
#endif

//

// 原子地读取 ptr 指向的变量
// 保证严格的顺序语义。
#define atom_load(ptr)                 __atom_load(ptr, atom_seq_cst)
// 原子地将 ptr 指向的变量设为 val
// 保证严格的顺序语义。
#define atom_store(ptr, val)           __atom_store(ptr, val, atom_seq_cst)
// 原子地将 ptr 指向的变量的值设为 val，并返回该变量之前的值
// 保证严格的顺序语义。
#define atom_exch(ptr, val)            __atom_exch(ptr, val, atom_seq_cst)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#define atom_cexch(ptr, val, des)      __atom_cexch(ptr, val, des, atom_seq_cst, atom_seq_cst)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#define atom_cexch_weak(ptr, val, des) __atom_cexch_weak(ptr, val, des, atom_seq_cst, atom_seq_cst)
// 原子地将 ptr 指向的变量加上 val，并返回旧值
// 保证严格的顺序语义。
#define atom_add(ptr, val)             __atom_add(ptr, val, atom_seq_cst)
// 原子地将 ptr 指向的变量减去 val，并返回旧值
// 保证严格的顺序语义。
#define atom_sub(ptr, val)             __atom_sub(ptr, val, atom_seq_cst)
// 原子地将 ptr 指向的变量按位或上 val，并返回旧值
// 保证严格的顺序语义。
#define atom_or(ptr, val)              __atom_or(ptr, val, atom_seq_cst)
// 原子地将 ptr 指向的变量按位与上 val，并返回旧值
// 保证严格的顺序语义。
#define atom_and(ptr, val)             __atom_and(ptr, val, atom_seq_cst)
// 原子地将 ptr 指向的变量按位异或上 val，并返回旧值
// 保证严格的顺序语义。
#define atom_xor(ptr, val)             __atom_xor(ptr, val, atom_seq_cst)
// 用于原子地设置 ptr 指向的变量的值为 1，并返回该变量之前的值
// 保证严格的顺序语义。
#define atom_tas(ptr)                  __atom_tas(ptr, atom_seq_cst)
// 原子地将 ptr 指向的变量的值设为 0
// 保证严格的顺序语义。
#define atom_clr(ptr)                  __atom_clr(ptr, atom_seq_cst)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef i8 sbyte;
typedef u8 byte;

_CONST_(sbyte);
_CONST_(byte);

typedef const char *cstr;

_CONST_(cstr);

#undef _CONST_

#ifdef __cplusplus
}
#endif
