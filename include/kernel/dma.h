#pragma once
#include <define.h>

/**
 *\brief 向 DMA 控制器发送命令
 *
 *\param mode     DMA 模式
 *\param channel  DMA 通道
 *\param address  发送的数据起始地址
 *\param size     发送数据的大小
 */
void dma_start(byte mode, byte channel, void *address, size_t size);

/**
 *\brief 使用 DMA 发送数据
 *
 *\param channel  DMA 通道
 *\param address  发送的数据起始地址
 *\param size     发送数据的大小
 */
finline void dma_send(byte channel, void *address, size_t size) {
  dma_start(0x48, channel, address, size);
}

/**
 *\brief 使用 DMA 接收数据
 *
 *\param channel  DMA 通道
 *\param address  接收数据的起始地址
 *\param size     接收数据的大小
 */
finline void dma_recv(byte channel, void *address, size_t size) {
  dma_start(0x44, channel, address, size);
}
