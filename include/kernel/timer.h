#pragma once
#include <data-structure.h>
#include <define.h>
#include <kernel/mtask.h>
#include <type.h>
#define MAX_TIMER 500

struct TIMER {
  struct TIMER *next;
  u32           timeout, flags;
  cir_queue_t   queue;
  u8            data;
  mtask        *waiter;
};

struct TIMERCTL {
  volatile u32  count, next;
  struct TIMER *t0;
  struct TIMER  timers0[MAX_TIMER];
};

extern struct TIMERCTL timerctl;
extern uint64_t        global_time;

struct TIMER *timer_alloc();
void          init_pit();
