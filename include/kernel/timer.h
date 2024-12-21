#pragma once
#include <define.h>
#include <kernel/mtask.h>

extern volatile u64 system_tick;

void init_pit();

void sleep(u64 time_s);
void usleep(u64 time_us);
