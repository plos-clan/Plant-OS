#include "dosldr/screen.h"
#include <dosldr.h>

static int x, y;
static int cons_x, cons_y;

void Move_Cursor(i16 x, i16 y);

void clear() {
  int i;
  int j;
  for (i = 0; i < 160; i += 2) {
    for (j = 0; j < 25; j++) {
      *(char *)(0xb8000 + j * 160 + i) = ' ';
    }
  }
  x      = 0;
  y      = 0;
  cons_x = 0;
  cons_y = 0;
  Move_Cursor(cons_x, cons_y);
}

int getlength(const char *str) {
  for (int i = 0;; ++i) {
    if (str[i] == '\0') return i;
  }
}

void putstr(const char *str, int length) {
  int i;
  for (i = 0; i < length; i++) {
    if (y == 24 && x >= 160) {
      /*暂时什么也不做*/
      //TODO:屏幕滚动！！！！
      screen_ne();
    }
    if (str[i] == 0x0d) { continue; }
    putchar(str[i]);
  }
}

void printchar(char ch) {
  char ch1[] = {ch, '\0'};
  print(ch1);
}

void outputchar_b800(int x, int y, char ch) {
  x                                *= 2;
  *(char *)(0xb8000 + y * 160 + x)  = ch;
}

void putchar(char ch) {

  if (x == 160) {
    y++;
    cons_y++;
    x      = 0;
    cons_x = 0;
    Move_Cursor(cons_x, cons_y);
  }

  if (ch == '\n') {
    if (y == 24) {
      /*暂时不做什么*/
      //TODO:屏幕滚动！！！！
      screen_ne();

      return;
    }
    y++;
    //*(char *)(0xb8000 + cons_y * 160 + cons_x) = ' ';
    cons_y++;
    x = 0;

    cons_x = 0;
    Move_Cursor(cons_x, cons_y);
    return;
  } else if (ch == '\0') {
    return;
  } else if (ch == '\b') {
    if (x == 0) { return; }
    *(char *)(0xb8000 + y * 160 + x - 2)  = ' '; /*显示位置为第23行*/
    x                                    -= 2;
    cons_x                               -= 2;
    //*(char *)(0xb8000 + cons_y * 160 + cons_x + 4) = ' ';
    cons_x += 1;
    Move_Cursor(cons_x, cons_y);
    return;
  }
  //*(char *)(0xb8000 + cons_y * 160 + cons_x) = ' ';
  cons_x += 1;
  Move_Cursor(cons_x, cons_y);
  *(char *)(0xb8000 + y * 160 + x)  = ch; /*显示位置为第23行*/
  x                                += 2;
}
void Move_Cursor(i16 x, i16 y) {
  int res = y * 80 + x;
  move_cursor_by_idx(res);
}
void print(const char *str) {
  putstr(str, getlength(str));
}
void screen_ne() /*向下滚动一行*/
{
  char *p = (char *)(0xb8000 + 0 * 160 + 0);
  int   i, j;
  /*清空第一行*/
  for (i = 0; i < 160; i += 2) {
    *(char *)(0xb8000 + 0 * 160 + i) = 0;
  }
  //clear();
  for (i = 0; i < 25; i++) {
    for (j = 0; j < 80; j++) {
      //*(char *)(0xb8000 + i * 160 + j) = *(char *)(0xb8000 + i+1 * 160 + j);
      char ch = *(char *)(0xb8000 + i * 160 + j * 2);
      outputchar_b800(j, i - 1, ch);
    }
  }

  for (j = 0; j < 160; j += 2) {
    *(char *)(0xb8000 + 24 * 160 + j) = 0;
  }

  cons_x = 0;
  x      = 0;
  Move_Cursor(cons_x, cons_y);
}
int get_x() {
  return x;
}
int get_y() {
  return y;
}
void gotoxy(int x1, int y1) {
  cons_x = x1;
  cons_y = y1;
  x      = x1 * 2;
  y      = y1;
  Move_Cursor(cons_x, cons_y);
}