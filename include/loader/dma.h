#pragma once
#include <define.h>

/**
 *\brief 向 DMA 控制器发送命令
 *
 *\param mode     DMA 模式
 *\param channel  DMA 通道
 *\param address  发送的数据起始地址
 *\param size     发送数据的大小
 *\param is_16bit 是否为 16 位模式
 */
void dma_start(byte mode, byte channel, void *address, uint size, bool is_16bit);

finline void dma_send(byte channel, void *address, uint size, bool is_16bit) {
  dma_start(0x48, channel, address, size, is_16bit);
}

finline void dma_recv(byte channel, void *address, uint size, bool is_16bit) {
  dma_start(0x44, channel, address, size, is_16bit);
}

finline void dma8_start(byte mode, byte channel, void *address, uint size) {
  dma_start(mode, channel, address, size, false);
}

finline void dma8_send(byte channel, void *address, uint size) {
  dma_send(channel, address, size, false);
}

finline void dma8_recv(byte channel, void *address, uint size) {
  dma_recv(channel, address, size, false);
}

finline void dma16_start(byte mode, byte channel, void *address, uint size) {
  dma_start(mode, channel, address, size, true);
}

finline void dma16_send(byte channel, void *address, uint size) {
  dma_send(channel, address, size, true);
}

finline void dma16_recv(byte channel, void *address, uint size) {
  dma_recv(channel, address, size, true);
}
