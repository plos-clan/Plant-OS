#pragma once
#include <pl2d/fb.h>

#ifdef __cplusplus
extern "C" {
#endif

// 你的程序应该提供的 API

void program_exit();
void screen_flush();

// plds 提供的 API

int  plds_init(void *buffer, u32 width, u32 height, pl2d_PixFmt fmt);
void plds_flush(); // 60fps 信号
void plds_deinit();
// 屏幕大小重设
int  plds_on_screen_resize(void *buffer, u32 width, u32 height, pl2d_PixFmt fmt);
// 鼠标移动
void plds_on_mouse_move(i32 x, i32 y);
// 鼠标按键按下
void plds_on_button_down(i32 button, i32 x, i32 y);
// 鼠标按键释放
void plds_on_button_up(i32 button, i32 x, i32 y);
// 键盘按键按下
void plds_on_key_down(i32 key);
// 键盘按键释放
void plds_on_key_up(i32 key);
// 鼠标滚轮，nrows 表示内容可见区域向下 N 行
void plds_on_scroll(i32 nrows);

#ifdef __cplusplus
}
#endif
