#pragma once
#include <define.h>
#include <type.h>
int  printf(const char *format, ...);
void clear();
void print(const char *str);
void screen_ne();
void move_cursor_by_idx(int idx);
void putchar(char ch);