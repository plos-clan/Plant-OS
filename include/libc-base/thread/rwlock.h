#pragma once
#include <define.h>

#define PLOS_HAS_RWLOCK 1

typedef atomic volatile isize rwlock_t;

#define RWLOCK_INIT 0

#define rwlock_rdlock(lock)

#define rwlock_wrlock(lock)                                                                        \
  ({                                                                                               \
    rwlock_t *_lock_ = &(lock);                                                                    \
    while (!atom_cexch(_lock_, 0, 1)) {                                                            \
      _val_ = atom_add(_lock_, 1);                                                                 \
    }                                                                                              \
  })

// 从读锁转换为写锁
#define rwlock_rd2wr(lock)

// 从写锁转换为读锁
#define rwlock_wr2rd(lock)

#define rwlock_unlock(lock)                                                                        \
  ({                                                                                               \
    rwlock_t *_lock_  = &(lock);                                                                   \
    rwlock_t  _value_ = -1;                                                                        \
    if (!atom_cexch(_lock_, &_value_, 0)) {                                                        \
      if (atom_sub(_lock_, 1) == 0) stom_store(_lock_, 0);                                         \
    }                                                                                              \
  })
