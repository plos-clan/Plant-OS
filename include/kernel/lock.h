#pragma once
#include <define.h>
#include <type.h>

typedef struct mtask mtask;
typedef struct {
  mtask *owner;
  u32    value;
  mtask *waiter;
} lock_t;

#define LOCK_UNLOCKED 0
#define LOCK_LOCKED   1

void lock_init(lock_t *l);
