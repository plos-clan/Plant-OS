#pragma once
#include <define.h>
int  klogf(const char *format, ...);
void screen_clear();
void print(const char *str);
void screen_ne();
void move_cursor_by_idx(int idx);
void putchar(char ch);
