#pragma once
#include <data-structure.h>
#include <define.h>
#include <kernel/mtask.h>
#define MAX_TIMER 500

struct TIMERCTL {
  volatile u32 count, next;
};

extern struct TIMERCTL timerctl;
extern uint64_t        global_time;

void init_pit();
