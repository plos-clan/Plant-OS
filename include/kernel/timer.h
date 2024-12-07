#pragma once
#include <data-structure.h>
#include <define.h>
#include <kernel/mtask.h>
#define MAX_TIMER 500

extern volatile u64 system_tick;

void init_pit();
