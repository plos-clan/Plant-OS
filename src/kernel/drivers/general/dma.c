#include <kernel.h>

#define LOW_BYTE(x)  ((x) & 0x00ff)
#define HIGH_BYTE(x) (((x) & 0xff00) >> 8)

// 每个DMA通道的快速访问寄存器和端口。
static const byte MASK_REG[8]  = {0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4};
static const byte MODE_REG[8]  = {0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6};
static const byte CLEAR_REG[8] = {0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8};

static const byte PAGE_PORT[8]  = {0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A};
static const byte ADDR_PORT[8]  = {0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC};
static const byte COUNT_PORT[8] = {0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE};

void dma_start(byte mode, byte channel, void *address, size_t size) {
  mode |= (channel % 4); // What the fuck ???
                         // 这什么奇葩标准

  if (channel > 4 && size % 2 != 0) fatal("16 位模式下大小必须为 2 byte 的整数倍");

  size_t addr   = (size_t)address;
  byte   page   = addr >> 16;
  u16    offset = (channel > 4 ? addr / 2 : addr);
  size          = (channel > 4 ? size / 2 : size) - 1;

  // 我们不想别的事情来打扰
  asm_cli;

  // 设置DMA通道，以便我们可以正确传输数据，这很简单，只要我们用I/O操作告诉DMA控制器就行了
  // 我们将使用这个通道（DMA_channel）
  asm_out8(MASK_REG[channel], 0x04 | (channel % 4));

  // 我们先得解除DMA对这个通道的屏蔽，不然用不了
  asm_out8(CLEAR_REG[channel], 0x00);

  // 向DMA发送指定的模式
  asm_out8(MODE_REG[channel], mode);

  // 发送数据所在的物理页
  asm_out8(PAGE_PORT[channel], page);

  // 发送偏移量地址，先发送高八位，再发送低八位（因为一次性最多只能发送一个byte）
  asm_out8(ADDR_PORT[channel], LOW_BYTE(offset));
  asm_out8(ADDR_PORT[channel], HIGH_BYTE(offset));

  // 发送数据的长度 跟之前一样，先发送低八位，再发送高八位
  asm_out8(COUNT_PORT[channel], LOW_BYTE(size));
  asm_out8(COUNT_PORT[channel], HIGH_BYTE(size));

  // 现在我们该做的东西已经全部做完了，所以启用DMA_channel
  asm_out8(MASK_REG[channel], (channel % 4));

  // 重新让CPU能够接收到中断
  asm_sti;
}
