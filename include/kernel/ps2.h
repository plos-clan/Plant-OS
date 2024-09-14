#pragma once
#include <define.h>

#define PORT_KEYDAT     0x0060
#define PORT_KEYSTA     0x0064
#define PORT_KEYCMD     0x0064
#define MOUSE_ROLL_NONE 0
#define MOUSE_ROLL_UP   1
#define MOUSE_ROLL_DOWN 2

struct MOUSE_DEC {
  u8   buf[4], phase;
  int  x, y, btn;
  int  sleep;
  char roll;
};

void                    wait_KBC_sendready();
void                    mouse_sleep(struct MOUSE_DEC *mdec);
void                    init_keyboard();
int                     getch();
extern struct MOUSE_DEC mdec;
void                    mouse_ready(struct MOUSE_DEC *mdec);