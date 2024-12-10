#pragma once
#include <fs.h>
#include <libc-base.h>
#include <sys.h>

typedef struct __PACKED__ task *task_t;

typedef struct {
  task_t owner;
  u32    value;
  task_t waiter;
} lock_t;
