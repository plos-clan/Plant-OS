#pragma once
#include "asm.h"
#include "cpuid.h"

#define asm_mfence ({ asm volatile("mfence\n\t" ::: "memory"); })

#define asm_cflush(addr) ({ asm volatile("clflush (%0)\n\t" ::"r"((size_t)(addr))); })

#define asm_wbinvd ({ asm volatile("wbinvd\n\t" ::: "memory"); })

finline void sync_memory(void *addr, size_t size) {
  const size_t cflush_size = cpuid_cflush_size;
  const size_t address     = (size_t)addr & ~(cflush_size - 1);
  if (cpuid_has_cflush) {
    for (size_t i = 0; i < size; i += cflush_size) {
      asm_cflush(address + i);
    }
  } else {
    asm_wbinvd;
  }
  asm_mfence;
}
