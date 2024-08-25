#pragma once
#include <define.h>

void dma_send(byte channel, u64 address, uint length, byte read, bool is_16bit);

finline void dma8_send(byte channel, u64 address, uint length, byte read) {
  dma_send(channel, address, length, read, false);
}

finline void dma16_send(byte channel, u64 address, uint length, byte read) {
  dma_send(channel, address, length, read, true);
}
