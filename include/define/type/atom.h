// This code is released under the MIT License

#pragma once

#pragma GCC system_header

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~ 标准库原子操作部分

//- 参考 gcc   /usr/lib/gcc/x86_64-linux-gnu/14/include/stdatomic.h
//- 参考 clang /usr/lib/llvm-19/lib/clang/19/include/stdatomic.h
//-   注意 clang 兼容 gcc 标准头文件，所以我们可以直接参照 gcc 的实现

// 看到底下 NO_STD || !defined(__cplusplus) 了没
// 都是为了和标准库能够共存搞的

#ifndef __cplusplus
typedef enum memory_order { //+ 但在 gcc 中这个 enum 没有名称
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

//~ 自定义简写部分

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

#if NO_STD
#  define ATOMIC_VAR_INIT(value) (value)
#endif

#if NO_STD || !defined(__cplusplus)
// 用VAL初始化由PTR指向的原子对象。
#  define atomic_init(ptr, value) __atom_store(ptr, value, __atom_relaxed)
#endif
