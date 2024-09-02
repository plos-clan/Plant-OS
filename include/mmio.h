#pragma once
#include <define.h>

finline void mmio_out32(uintptr_t addr, uint32_t data) { *(volatile u32 *)addr = data; }
finline uint32_t mmio_in32(uintptr_t addr) { return *(volatile u32 *)addr; }
finline void mmio_out16(uintptr_t addr, uint16_t data) { *(volatile u32 *)addr = data; }
finline uint16_t mmio_in16(uintptr_t addr) { return *(volatile u16 *)addr; }
finline void mmio_out8(uintptr_t addr, uint8_t data) { *(volatile u8 *)addr = data; }
finline uint8_t mmio_in8(uintptr_t addr) { return *(volatile u8 *)addr; }