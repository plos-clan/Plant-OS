#include <kernel.h>

/* 定义用于访问一个整数的上位和下位字节。 */
#define LOW_BYTE(x) (x & 0x00FF)
#define HI_BYTE(x)  ((x & 0xFF00) >> 8)

/* 每个DMA通道的快速访问寄存器和端口。 */
byte MaskReg[8]  = {0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4};
byte ModeReg[8]  = {0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6};
byte ClearReg[8] = {0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8};

byte PagePort[8]  = {0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A};
byte AddrPort[8]  = {0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC};
byte CountPort[8] = {0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE};

void _dma_xfer(byte DMA_channel, byte page, uint offset, uint length, byte mode);

void dma_xfer(byte channel, unsigned long address, uint length, byte read) {
  byte page = 0, mode = 0;
  uint offset = 0;

  mode = (read ? 0x48 : 0x44) + channel;

  page   = address >> 16;
  offset = address & 0xFFFF;
  length--;

  _dma_xfer(channel, page, offset, length, mode);
}

void _dma_xfer(byte DMA_channel, byte page, uint offset, uint length, byte mode) {
  /* 我们不想别的事情来打扰 */
  asm("cli");

  /* 设置DMA通道，以便我们可以正确传输数据，这很简单，只要我们用I/O操作告诉DMA控制器就行了
   */
  /* 我们将使用这个通道（DMA_channel）*/
  asm_out8(MaskReg[DMA_channel], 0x04 | DMA_channel);

  /* 我们先得解除DMA对这个通道的屏蔽，不然用不了 */
  asm_out8(ClearReg[DMA_channel], 0x00);

  /* 向DMA发送指定的模式 */
  asm_out8(ModeReg[DMA_channel], mode);

  /* 发送偏移量地址，先发送高八位，再发送低八位（因为一次性最多只能发送一个byte）
   */
  asm_out8(AddrPort[DMA_channel], LOW_BYTE(offset));
  asm_out8(AddrPort[DMA_channel], HI_BYTE(offset));

  /* 发送数据所在的物理页 */
  asm_out8(PagePort[DMA_channel], page);

  /* 发送数据的长度 跟之前一样，先发送低八位，再发送高八位*/
  asm_out8(CountPort[DMA_channel], LOW_BYTE(length));
  asm_out8(CountPort[DMA_channel], HI_BYTE(length));

  /* 现在我们该做的东西已经全部做完了，所以启用DMA_channel */
  asm_out8(MaskReg[DMA_channel], DMA_channel);

  /* 重新让CPU能够接收到中断 */
  asm("sti");
}