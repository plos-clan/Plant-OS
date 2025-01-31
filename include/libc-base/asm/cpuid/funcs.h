#pragma once
#include "ids.h"
#include "macros.h"
#include <define.h>

finline void cpuid_do_cache() {
  if (cpuids_cached) return;
  cpuid(0, cpuids.ID0.eax, cpuids.ID0.ebx, cpuids.ID0.ecx, cpuids.ID0.edx);
  cpuid(1, cpuids.ID1.eax, cpuids.ID1.ebx, cpuids.ID1.ecx, cpuids.ID1.edx);
  cpuid(2, cpuids.ID2.eax, cpuids.ID2.ebx, cpuids.ID2.ecx, cpuids.ID2.edx);
  cpuid(3, cpuids.ID3.eax, cpuids.ID3.ebx, cpuids.ID3.ecx, cpuids.ID3.edx);
  cpuid(4, cpuids.ID4.eax, cpuids.ID4.ebx, cpuids.ID4.ecx, cpuids.ID4.edx);
  cpuid(15, cpuids.ID15.eax, cpuids.ID15.ebx, cpuids.ID15.ecx, cpuids.ID15.edx);
  cpuid(16, cpuids.ID16.eax, cpuids.ID16.ebx, cpuids.ID16.ecx, cpuids.ID16.edx);
  cpuid(0x40000000, cpuids.IDV.eax, cpuids.IDV.ebx, cpuids.IDV.ecx, cpuids.IDV.edx);
  cpuid(0x80000000, cpuids.IDX0.eax, cpuids.IDX0.ebx, cpuids.IDX0.ecx, cpuids.IDX0.edx);
  cpuid(0x80000001, cpuids.IDX1.eax, cpuids.IDX1.ebx, cpuids.IDX1.ecx, cpuids.IDX1.edx);
  cpuids_cached = true;
}
