#pragma once
#include <define.h>
#include <kernel/mtask.h>

extern volatile u64 system_tick;

void init_pit(); // 初始化 PIT

void sleep(u64 time_s);   // 睡眠 time_s 秒
void usleep(u64 time_us); // 睡眠 time_us 微秒
