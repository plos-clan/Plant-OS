#include <kernel.h>

#define PORT 0x3f8 // COM1

int init_serial() {
  asm_out8(PORT + 1, 0x00);                // 禁止COM的中断发生
  asm_out8(PORT + 3, 0x80);                // 启用DLAB（设置波特率除数）。
  asm_out8(PORT + 0, 0x01);                // 设置除数为1，(低位) 115200波特
  asm_out8(PORT + 1, 0x00);                //            (高位)
  asm_out8(PORT + 3, 0x03);                // 每次 8 位 | 无奇偶校验 | 一个停止位
  asm_out8(PORT + 2, 0xC7);                // 启用FIFO，有14字节的阈值
  asm_out8(PORT + 4, 0x0B);                // 启用IRQ，设置RTS/DSR
  asm_out8(PORT + 4, 0x1E);                // 设置为环回模式，测试串口
  asm_out8(PORT + 0, 0xAE);                // 测试串口（发送字节0xAE并检查串口是否返回相同的字节）
  if (asm_in8(PORT + 0) != 0xAE) return 1; // 检查串口是否有问题（即：与发送的字节不一样）
  asm_out8(PORT + 4, 0x0F);                // 如果串口没有故障，将其设置为正常运行模式。
  return 0;                                // (非环回，启用 IRQ，启用OUT#1和OUT#2位)
}

static bool serial_received() {
  return asm_in8(PORT + 5) & 1;
}

static bool is_transmit_empty() {
  return asm_in8(PORT + 5) & 0x20;
}

byte serial_read() {
  while (serial_received() == 0) {}
  return asm_in8(PORT);
}

int serial_read_nonblock() {
  if (serial_received() == 0) return -1;
  return asm_in8(PORT);
}

void serial_write(char a) {
  waituntil(is_transmit_empty());
  asm_out8(PORT, a);
}
