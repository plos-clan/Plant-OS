#pragma once
#include "asm.h"
#include "cpuid.h"

#define compiler_barrier ({ asm volatile("" ::: "memory"); })

#define memory_barrier ({ asm volatile("mfence" ::: "memory"); })

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

// 刷新虚拟地址 vaddr 的 块表 TLB
finline void flush_tlb(usize vaddr) {
  asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

#define flush_tlb(vaddr) flush_tlb((usize)(vaddr))
