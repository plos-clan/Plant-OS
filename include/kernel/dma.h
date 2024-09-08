#pragma once
#include <define.h>

void dma_start(byte mode, byte channel, void *address, uint length, bool is_16bit);

finline void dma_send(byte channel, void *address, uint length, bool is_16bit) {
  dma_start(0x48, channel, address, length, is_16bit);
}

finline void dma_recv(byte channel, void *address, uint length, bool is_16bit) {
  dma_start(0x44, channel, address, length, is_16bit);
}

finline void dma8_start(byte mode, byte channel, void *address, uint length) {
  dma_start(mode, channel, address, length, false);
}

finline void dma8_send(byte channel, void *address, uint length) {
  dma_send(channel, address, length, false);
}

finline void dma8_recv(byte channel, void *address, uint length) {
  dma_recv(channel, address, length, false);
}

finline void dma16_start(byte mode, byte channel, void *address, uint length) {
  dma_start(mode, channel, address, length, true);
}

finline void dma16_send(byte channel, void *address, uint length) {
  dma_send(channel, address, length, true);
}

finline void dma16_recv(byte channel, void *address, uint length) {
  dma_recv(channel, address, length, true);
}
