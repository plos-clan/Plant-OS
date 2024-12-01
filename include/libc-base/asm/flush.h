#pragma once
#include "asm.h"
#include "cpuid.h"

finline void sync_memory(void *addr, size_t size) {
  const size_t cflush_size = cpuid_cflush_size;
  const size_t address     = (size_t)addr & ~(cflush_size - 1);
  if (cpuid_has_cflush) {
    for (size_t i = 0; i < size; i += cflush_size) {
      asm_cflush(address + i);
    }
  }
  asm_mfence;
}
