#pragma once

typedef enum {
  MODE_A = 'A',
  MODE_B = 'B',
  MODE_C = 'C',
  MODE_D = 'D',
  MODE_E = 'E',
  MODE_F = 'F',
  MODE_G = 'G',
  MODE_H = 'H',
  MODE_f = 'f',
  MODE_J = 'J',
  MODE_K = 'K',
  MODE_S = 'S',
  MODE_T = 'T',
  MODE_m = 'm'
} vt100_mode_t;

void move_cursor_by_idx(int idx);
void print(const char *str);
void clear();