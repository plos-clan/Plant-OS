#pragma once
#include <define.h>

typedef bool spin_t;

#define spin_trylock(_spin_) (!atom_tas(&(_spin_)))
#define spin_lock(_spin_)                                                                          \
  ({                                                                                               \
    while (atom_tas(&(_spin_))) {}                                                                 \
  })
#define spin_unlock(_spin_) atom_clr(&(_spin_))
