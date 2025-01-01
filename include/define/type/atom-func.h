// This code is released under the MIT License

#pragma once

#pragma GCC system_header

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 原子函数

//- 参考 gcc   /usr/lib/gcc/x86_64-linux-gnu/14/include/stdatomic.h
//- 参考 clang /usr/lib/llvm-19/lib/clang/19/include/stdatomic.h
//-   注意 clang 兼容 gcc 标准头文件，所以我们可以直接参照 gcc 的实现

#if 0 //+ 这是 clang 的定义，但貌似无法使用

#  define __atom_store(ptr, value, mo) __c11_atomic_store(ptr, value, mo)
#  define __atom_load(ptr, mo)         __c11_atomic_load(ptr, mo)

#  define __atom_add(ptr, value, mo) __c11_atomic_fetch_add(ptr, value, mo)
#  define __atom_sub(ptr, value, mo) __c11_atomic_fetch_sub(ptr, value, mo)
#  define __atom_or(ptr, value, mo)  __c11_atomic_fetch_or(ptr, value, mo)
#  define __atom_and(ptr, value, mo) __c11_atomic_fetch_and(ptr, value, mo)
#  define __atom_xor(ptr, value, mo) __c11_atomic_fetch_xor(ptr, value, mo)

#  define __atom_tas(ptr, mo) __c11_atomic_exchange(ptr, true, mo)
#  define __atom_clr(ptr, mo) __c11_atomic_store(ptr, false, mo)

#  define __atom_exch(ptr, value, mo) __c11_atomic_exchange(ptr, value, mo)

#  define __atom_cexch(ptr, expected, value, smo, fmo)                                             \
    __c11_atomic_compare_exchange_strong(ptr, expected, value, smo, fmo)
#  define __atom_cexch_weak(ptr, expected, value, smo, fmo)                                        \
    __c11_atomic_compare_exchange_weak(ptr, expected, value, smo, fmo)

#endif

//+ 我们遵循 gcc 的定义 (注意不要直接使用这些双下划线开头的函数)

/* Note that these macros require __auto_type to remove
   _Atomic qualifiers (and const qualifiers, if those are valid on
   macro operands).

   Also note that the header file uses the generic form of __atomic
   builtins, which requires the address to be taken of the value
   parameter, and then we pass that value on.  This allows the macros
   to work for any type, and the compiler is smart enough to convert
   these to lock-free _N variants if possible, and throw away the
   temps.  */

//* 以上是 gcc stdatomic.h 原文，以下是翻译。
//* 注意我们使用 auto 来代替 __auto_type (这应该不会导致错误)

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
// 原子地将 ptr 指向的变量设为 value
#define __atom_store(ptr, value, mo)                                                               \
  ({                                                                                               \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __val = (value);                                                                \
    __atomic_store(__ptr, &__val, mo);                                                             \
  })

// 原子地将 ptr 指向的变量加上 value，并返回旧值
#define __atom_add(ptr, value, mo) __atomic_fetch_add(ptr, value, mo)
// 原子地将 ptr 指向的变量减去 value，并返回旧值
#define __atom_sub(ptr, value, mo) __atomic_fetch_sub(ptr, value, mo)
// 原子地将 ptr 指向的变量按位或上 value，并返回旧值
#define __atom_or(ptr, value, mo)  __atomic_fetch_or(ptr, value, mo)
// 原子地将 ptr 指向的变量按位与上 value，并返回旧值
#define __atom_and(ptr, value, mo) __atomic_fetch_and(ptr, value, mo)
// 原子地将 ptr 指向的变量按位异或上 value，并返回旧值
#define __atom_xor(ptr, value, mo) __atomic_fetch_xor(ptr, value, mo)

// 用于原子地设置 ptr 指向的变量的值为 1，并返回该变量之前的值
#define __atom_tas(ptr, mo) __atomic_test_and_set(ptr, mo)
// 原子地将 ptr 指向的变量的值设为 0
#define __atom_clr(ptr, mo) __atomic_clear(ptr, mo)

// 原子地将 ptr 指向的变量的值设为 value，并返回该变量之前的值
#define __atom_exch(ptr, value, mo)                                                                \
  __extension__({                                                                                  \
    auto           __ptr = (ptr);                                                                  \
    typeof(*__ptr) __val = (value);                                                                \
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
//~ 标准库原子函数

#if NO_STD || !defined(__cplusplus)
// 原子地读取 ptr 指向的变量
#  define atomic_load_explicit(ptr, mo) __atom_load(ptr, mo)
// 原子地读取 ptr 指向的变量
// 保证严格的顺序语义。
#  define atomic_load(ptr)              __atom_load(ptr, atom_seq_cst)

// 原子地将 ptr 指向的变量设为 value
#  define atomic_store_explicit(ptr, value, mo) __atom_store(ptr, value, mo)
// 原子地将 ptr 指向的变量设为 value
// 保证严格的顺序语义。
#  define atomic_store(ptr, value)              __atom_store(ptr, value, atom_seq_cst)

// 原子地将 ptr 指向的变量的值设为 value，并返回该变量之前的值
#  define atomic_exchange_explicit(ptr, value, mo) __atom_exch(ptr, value, mo)
// 原子地将 ptr 指向的变量的值设为 value，并返回该变量之前的值
// 保证严格的顺序语义。
#  define atomic_exchange(ptr, value)              __atom_exch(ptr, value, atom_seq_cst)

// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
#  define atomic_compare_exchange_strong_explicit(ptr, value, des, smo, fmo)                       \
    __atom_cexch(ptr, value, des, smo, fmo)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#  define atomic_compare_exchange_strong(ptr, value, des)                                          \
    __atom_cexch(ptr, value, des, atom_seq_cst, atom_seq_cst)

// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
#  define atomic_compare_exchange_weak_explicit(ptr, value, des, smo, fmo)                         \
    __atom_cexch_weak(ptr, value, des, smo, fmo)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#  define atomic_compare_exchange_weak(ptr, value, des)                                            \
    __atom_cexch_weak(ptr, value, des, atom_seq_cst, atom_seq_cst)

// 原子地将 ptr 指向的变量加上 value，并返回旧值
#  define atomic_fetch_add_explicit(ptr, value, mo) __atom_add(ptr, value, mo)
// 原子地将 ptr 指向的变量加上 value，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_add(ptr, value)              __atom_add(ptr, value, atom_seq_cst)

// 原子地将 ptr 指向的变量减去 value，并返回旧值
#  define atomic_fetch_sub_explicit(ptr, value, mo) __atom_sub(ptr, value, mo)
// 原子地将 ptr 指向的变量减去 value，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_sub(ptr, value)              __atom_sub(ptr, value, atom_seq_cst)

// 原子地将 ptr 指向的变量按位或上 value，并返回旧值
#  define atomic_fetch_or_explicit(ptr, value, mo) __atom_or(ptr, value, mo)
// 原子地将 ptr 指向的变量按位或上 value，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_or(ptr, value)              __atom_or(ptr, value, atom_seq_cst)

// 原子地将 ptr 指向的变量按位与上 value，并返回旧值
#  define atomic_fetch_and_explicit(ptr, value, mo) __atom_and(ptr, value, mo)
// 原子地将 ptr 指向的变量按位与上 value，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_and(ptr, value)              __atom_and(ptr, value, atom_seq_cst)

// 原子地将 ptr 指向的变量按位异或上 value，并返回旧值
#  define atomic_fetch_xor_explicit(ptr, value, mo) __atom_xor(ptr, value, mo)
// 原子地将 ptr 指向的变量按位异或上 value，并返回旧值
// 保证严格的顺序语义。
#  define atomic_fetch_xor(ptr, value)              __atom_xor(ptr, value, atom_seq_cst)

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 原子函数简写

// 原子地读取 ptr 指向的变量
// 保证严格的顺序语义。
#define atom_load(ptr)              __atom_load(ptr, atom_seq_cst)
// 原子地将 ptr 指向的变量设为 value
// 保证严格的顺序语义。
#define atom_store(ptr, value)      __atom_store(ptr, value, atom_seq_cst)
// 原子地将 ptr 指向的变量的值设为 value，并返回该变量之前的值
// 保证严格的顺序语义。
#define atom_exch(ptr, value)       __atom_exch(ptr, value, atom_seq_cst)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#define atom_cexch(ptr, value, des) __atom_cexch(ptr, value, des, atom_seq_cst, atom_seq_cst)
// 原子地比较 ptr 指向的变量的值是否等于 *exp，如果相等则将其设为 *des，并返回
// true，否则将 *exp 设为 ptr 指向的变量的值并返回 false.
// 保证严格的顺序语义。
#define atom_cexch_weak(ptr, value, des)                                                           \
  __atom_cexch_weak(ptr, value, des, atom_seq_cst, atom_seq_cst)
// 原子地将 ptr 指向的变量加上 value，并返回旧值
// 保证严格的顺序语义。
#define atom_add(ptr, value) __atom_add(ptr, value, atom_seq_cst)
// 原子地将 ptr 指向的变量减去 value，并返回旧值
// 保证严格的顺序语义。
#define atom_sub(ptr, value) __atom_sub(ptr, value, atom_seq_cst)
// 原子地将 ptr 指向的变量按位或上 value，并返回旧值
// 保证严格的顺序语义。
#define atom_or(ptr, value)  __atom_or(ptr, value, atom_seq_cst)
// 原子地将 ptr 指向的变量按位与上 value，并返回旧值
// 保证严格的顺序语义。
#define atom_and(ptr, value) __atom_and(ptr, value, atom_seq_cst)
// 原子地将 ptr 指向的变量按位异或上 value，并返回旧值
// 保证严格的顺序语义。
#define atom_xor(ptr, value) __atom_xor(ptr, value, atom_seq_cst)
// 用于原子地设置 ptr 指向的变量的值为 1，并返回该变量之前的值
// 保证严格的顺序语义。
#define atom_tas(ptr)        __atom_tas(ptr, atom_seq_cst)
// 原子地将 ptr 指向的变量的值设为 0
// 保证严格的顺序语义。
#define atom_clr(ptr)        __atom_clr(ptr, atom_seq_cst)
