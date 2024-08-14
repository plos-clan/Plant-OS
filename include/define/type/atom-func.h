// This code is released under the MIT License

#pragma once

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
