#pragma once
#include <define.h>

#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR  0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR  0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

void irq_mask_clear(u8 irq); // 启用中断
void irq_mask_set(u8 irq);   // 禁用中断
void init_pic();             // 初始化 PIC
void send_eoi(int irq);      // 发送 EOI
