#pragma once
#include <define.h>

#define PLOS_HAS_SPIN 1

typedef volatile bool spin_t;

#define SPIN_INIT false

#define spin_trylock(_spin_) (!atom_tas(&(_spin_)))
#define spin_lock(_spin_)                                                                          \
  ({                                                                                               \
    while (atom_tas(&(_spin_))) {}                                                                 \
  })
#define spin_unlock(_spin_) atom_clr(&(_spin_))

#define spin_lockall(...)                                                                          \
  ({                                                                                               \
    const spin_t _spins_[] = {__VA_ARGS__};                                                        \
    while (true) {                                                                                 \
      usize _i_ = 0;                                                                               \
      for (; _i_ < COUNT(_spins_); _i_++) {                                                        \
        if (!spin_trylock(_spins_[_i_])) break;                                                    \
      }                                                                                            \
      if (_i_ == COUNT(_spins_)) break;                                                            \
      for (; _i_ > 0; _i_--) {                                                                     \
        spin_unlock(_spins_[_i_ - 1]);                                                             \
      }                                                                                            \
    }                                                                                              \
  })

#define spin_unlockall(...)                                                                        \
  ({                                                                                               \
    const spin_t _spins_[] = {__VA_ARGS__};                                                        \
    for (usize _i_ = COUNT(_spins_); _i_ > 0; _i_--) {                                             \
      spin_unlock(_spins_[_i_ - 1]);                                                               \
    }                                                                                              \
  })
