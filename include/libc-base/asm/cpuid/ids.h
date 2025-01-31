#pragma once
#include <define.h>

// View https://en.wikipedia.org/wiki/CPUID for more information.

struct CPUIDS_IDN {
  u32 eax, ebx, ecx, edx;
};

union CPUIDS {
  struct {
    struct CPUIDS_IDN ID0;
    struct CPUIDS_IDN ID1;
    struct CPUIDS_IDN ID2;
    struct CPUIDS_IDN ID3;
    struct CPUIDS_IDN ID4;
    struct CPUIDS_IDN ID15;
    struct CPUIDS_IDN ID16;
    struct CPUIDS_IDN IDV;
    struct CPUIDS_IDN IDX0;
    struct CPUIDS_IDN IDX1;
  };
  struct {
    u32 ID0_EAX, ID0_EBX, ID0_ECX, ID0_EDX;
    u32 ID1_EAX, ID1_EBX, ID1_ECX, ID1_EDX;
    u32 ID2_EAX, ID2_EBX, ID2_ECX, ID2_EDX;
    u32 ID3_EAX, ID3_EBX, ID3_ECX, ID3_EDX;
    u32 ID4_EAX, ID4_EBX, ID4_ECX, ID4_EDX;
    u32 ID15_EAX, ID15_EBX, ID15_ECX, ID15_EDX;
    u32 ID16_EAX, ID16_EBX, ID16_ECX, ID16_EDX;
    u32 IDV_EAX, IDV_EBX, IDV_ECX, IDV_EDX;
    u32 IDX0_EAX, IDX0_EBX, IDX0_ECX, IDX0_EDX;
    u32 IDX1_EAX, IDX1_EBX, IDX1_ECX, IDX1_EDX;
  };
  struct {
    //& The CPUID =====> ID0 (0x00000000)
    struct { // CPUID_ID0_EAX
      u32 max_cpuid_level;
    };
    union { // CPUID_ID0_EBX ECX EDX
      char manufacturer[12];
      byte manufacturer_byte[12];
    };
    //& The CPUID =====> ID1 (0x00000001)
    struct { // CPUID_ID1_EAX
      u32 stepping          : 4;
      u32 model             : 4;
      u32 family            : 4;
      u32 processor_type    : 2;
      u32 ID0_EAX_RESERVED1 : 2;
      u32 extended_model    : 4;
      u32 extended_family   : 8;
      u32 ID0_EAX_RESERVED2 : 4;
    };
    struct { // CPUID_ID1_EBX
      u32 brand_index         : 8;
      u32 clflush_line_size   : 8;
      u32 max_addressable_ids : 8;
      u32 initial_apic_id     : 8;
    };
    struct { // CPUID_ID1_ECX
      u32 sse3              : 1;
      u32 pclmulqdq         : 1;
      u32 dtes64            : 1;
      u32 monitor           : 1;
      u32 ds_cpl            : 1;
      u32 vmx               : 1; // VT-x 支持
      u32 smx               : 1;
      u32 est               : 1;
      u32 tm2               : 1;
      u32 ssse3             : 1;
      u32 cnxt_id           : 1;
      u32 sdbg              : 1;
      u32 fma               : 1;
      u32 cx16              : 1;
      u32 xtpr              : 1;
      u32 pdcm              : 1;
      u32 ID1_ECX_RESERVED1 : 1;
      u32 pcid              : 1;
      u32 dca               : 1;
      u32 sse4_1            : 1;
      u32 sse4_2            : 1;
      u32 x2apic            : 1;
      u32 movbe             : 1;
      u32 popcnt            : 1;
      u32 tsc_deadline      : 1;
      u32 aes               : 1;
      u32 xsave             : 1;
      u32 osxsave           : 1;
      u32 avx               : 1;
      u32 f16c              : 1;
      u32 rdrand            : 1;
      u32 hypervisor        : 1;
    };
    struct { // CPUID_ID1_EDX
      u32 fpu               : 1;
      u32 vme               : 1;
      u32 de                : 1;
      u32 pse               : 1;
      u32 tsc               : 1;
      u32 msr               : 1;
      u32 pae               : 1;
      u32 mce               : 1;
      u32 cx8               : 1;
      u32 apic              : 1;
      u32 ID1_EDX_RESERVED1 : 1;
      u32 sep               : 1;
      u32 mtrr              : 1;
      u32 pge               : 1;
      u32 mca               : 1;
      u32 cmov              : 1;
      u32 pat               : 1;
      u32 pse36             : 1;
      u32 psn               : 1;
      u32 clfsh             : 1;
      u32 ID1_EDX_RESERVED2 : 1;
      u32 ds                : 1;
      u32 acpi              : 1;
      u32 mmx               : 1;
      u32 fxsr              : 1;
      u32 sse               : 1;
      u32 sse2              : 1;
      u32 ss                : 1;
      u32 htt               : 1;
      u32 tm                : 1;
      u32 ia64              : 1;
      u32 pbe               : 1;
    };
    //& The CPUID =====> ID2 (0x00000002)
    struct { // CPUID_ID2_EAX EBX ECX EDX
      byte CPUID_ID2_CONSTANT_02h;
      byte cache_and_tlb_descriptor[15];
    };
    //& The CPUID =====> ID3 (0x00000003)
    struct { // CPUID_ID3_EAX EBX ECX EDX
      u32 CPUID_ID3_RESERVED[4];
    };
    //& The CPUID =====> ID4 (0x00000004)
    struct { // CPUID_ID4_EAX
      u32 cache_type        : 5;
      u32 cache_level       : 3;
      u32 self_initializing : 1;
      u32 fully_associative : 1;
      u32 reserved1         : 4;
      u32 wtf               : 12; // TODO: WTF?
      u32 reserved2         : 6;
    };
    struct { // CPUID_ID4_EBX
      u32 line_size     : 12;
      u32 partitioning  : 10;
      u32 associativity : 10;
    };
    struct { // CPUID_ID4_ECX
      u32 ID4_ECX_ : 32;
    };
    struct { // CPUID_ID4_EDX
      u32 ID4_EDX_ : 32;
    };
    //& The CPUID =====> ID15 (0x0000000F)
    struct { // CPUID_ID15_EAX EBX ECX EDX
      u32 TSC_freq_by_CCC_freq_denominator;
      u32 TSC_freq_by_CCC_freq_numerator;
      u32 CCC_freq;
      u32 ID15_EDX_RESERVED;
    };
    //& The CPUID =====> ID16 (0x00000010)
    struct { // CPUID_ID16_EAX EBX ECX EDX
      u32 processor_base_freq : 16;
      u32 ID16_EAX_RESERVED1  : 16;
      u32 processor_max_freq  : 16;
      u32 ID16_EBX_RESERVED1  : 16;
      u32 processor_bus_freq  : 16;
      u32 ID16_ECX_RESERVED1  : 16;
      u32 ID16_EDX_RESERVED;
    };
    //& The CPUID =====> IDV (0x40000000)
    struct {
      u32  IDV_EAX_RESERVED;
      byte hypervisor_id[12];
    };
    //& The CPUID =====> IDX0 (0x80000000)
    struct { //$ CPUID_IDX0_EAX
      u32 max_cpuid_level_ex;
    };
    union { //$ CPUID_IDX0_EBX ECX EDX
      char amd_manufacturer[12];
      byte amd_manufacturer_byte[12];
    };
    //& The CPUID =====> IDX1 (0x80000001)
    //$                  CPUID_IDX1_EAX EBX
    u32 IDX1_EAX_RESERVED;
    u32 IDX1_EBX_RESERVED;
    //$                  CPUID_IDX1_ECX
    struct {
      u32 lahf_lm            : 1;
      u32 cmp_legacy         : 1;
      u32 svm                : 1;
      u32 extapic            : 1;
      u32 cr8_legacy         : 1;
      u32 abm                : 1;
      u32 sse4a              : 1;
      u32 misalignsse        : 1;
      u32 _3dnowprefetch     : 1;
      u32 osvw               : 1;
      u32 ibs                : 1;
      u32 xop                : 1;
      u32 skinit             : 1;
      u32 wdt                : 1;
      u32 IDX1_ECX_RESERVED1 : 1;
      u32 lwp                : 1;
      u32 fma4               : 1;
      u32 tce                : 1;
      u32 IDX1_ECX_RESERVED2 : 1;
      u32 nodeid_msr         : 1;
      u32 IDX1_ECX_RESERVED3 : 1;
      u32 tbm                : 1;
      u32 topoext            : 1;
      u32 perfctr_core       : 1;
      u32 perfctr_nb         : 1;
      u32 StreamPerfMon      : 1;
      u32 dbx                : 1;
      u32 perftsc            : 1;
      u32 pcx_l2i            : 1;
      u32 monitorx           : 1;
      u32 addr_mask_ext      : 1;
      u32 IDX1_ECX_RESERVED4 : 1;
    };
    //$                  CPUID_IDX1_EDX
    struct {
      u32 IDX1_EDX_DUPLICATED1 : 10; // These fields are duplicated from ID1
      u32 syscall_k6           : 1;  // SYSCALL/SYSRET (K6 only)
      u32 syscall              : 1;  // SYSCALL/SYSRET
      u32 IDX1_EDX_DUPLICATED2 : 6;  // These fields are duplicated from ID1
      u32 IDX1_EDX_RESERVED1   : 1;
      u32 ecc                  : 1;
      u32 nx                   : 1;
      u32 IDX1_EDX_RESERVED2   : 1;
      u32 extended_mmx         : 1;
      u32 IDX1_EDX_DUPLICATED3 : 2; // These fields are duplicated from ID1
      u32 fxsr_opt             : 1;
      u32 pdpe1gb              : 1;
      u32 rdtscp               : 1;
      u32 IDX1_EDX_RESERVED3   : 1;
      u32 lm                   : 1;
      u32 _3dnowext            : 1;
      u32 _3dnow               : 1;
    };
  };
};

extern union CPUIDS cpuids;

extern bool cpuids_cached;
