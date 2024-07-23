#pragma once
#include <define.h>
#include <type.h>
typedef struct mtask mtask;
typedef struct {
  mtask   *owner;
  unsigned value;
  mtask   *waiter;
} lock_t;
#define LOCK_UNLOCKED 0
#define LOCK_LOCKED   1