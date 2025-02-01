#pragma once
#include <define.h>

#define PORT_KEYDAT     0x0060
#define PORT_KEYSTA     0x0064
#define PORT_KEYCMD     0x0064
#define MOUSE_ROLL_NONE 0
#define MOUSE_ROLL_UP   1
#define MOUSE_ROLL_DOWN 2

struct MOUSE_DEC {
  byte buf[4];
  byte phase;
  int  x, y, btn;
  int  sleep;
  char roll;
};

void ps2_wait();
void mouse_sleep(struct MOUSE_DEC *mdec);
void keyboard_init();
void mouse_init();
int  getch();

extern struct MOUSE_DEC mdec;

void mouse_ready(struct MOUSE_DEC *mdec);

typedef struct PointerEvent {
  usize device_id;  // 设备 ID 不可用默认为 0
  usize pointer_id; // 指针 ID 不可用默认为 0
  u32   buttons;    // 指针的按钮按下状态，每一位代表一个按钮
  u32   x;          // 指针指向的 x 位置
  u32   y;          // 指针指向的 y 位置
  float xratio;     // 指针指向的 x 位置的比例
  float yratio;     // 指针指向的 y 位置的比例
  u32   width;      // 指针接触范围的宽度
  u32   height;     // 指针接触范围的高度
  f32   pressure;   // 压力 (0 - 1) 不可用默认为 0.5
  f32   twist;      // 指针设备的朝向 (0 - 2 * PI) 不可用默认为 0
} PointerEvent;
