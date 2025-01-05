#pragma once
#include <define.h>

// cr0 控制寄存器
// cr1 是保留寄存器
// cr2 只读，存放最近一次发生的页错误的线性地址
// cr3 存放页目录表的基地址
// cr4 控制寄存器

#define asm_get_cr0()                                                                              \
  ({                                                                                               \
    size_t cr0;                                                                                    \
    asm volatile("mov %%cr0, %0\n\t" : "=r"(cr0));                                                 \
    cr0;                                                                                           \
  })

#define asm_set_cr0(cr0) ({ asm volatile("mov %0, %%cr0\n\t" ::"r"((size_t)(cr0))); })

#define asm_get_cr2()                                                                              \
  ({                                                                                               \
    size_t cr2;                                                                                    \
    asm volatile("mov %%cr2, %0\n\t" : "=r"(cr2));                                                 \
    cr2;                                                                                           \
  })

#define asm_get_cr3()                                                                              \
  ({                                                                                               \
    size_t cr3;                                                                                    \
    asm volatile("mov %%cr3, %0\n\t" : "=r"(cr3));                                                 \
    cr3;                                                                                           \
  })

#define asm_set_cr3(cr3) ({ asm volatile("mov %0, %%cr3\n\t" ::"r"((size_t)(cr3))); })

#define asm_get_cr4()                                                                              \
  ({                                                                                               \
    size_t cr4;                                                                                    \
    asm volatile("mov %%cr4, %0\n\t" : "=r"(cr4));                                                 \
    cr4;                                                                                           \
  })

#define asm_set_cr4(cr4) ({ asm volatile("mov %0, %%cr4\n\t" ::"r"((size_t)(cr4))); })

#define CR0_PE MASK(0)  // Protection Enable 启用保护模式
#define CR0_MP MASK(1)  // Monitor Coprocessor
#define CR0_EM MASK(2)  // Emulation 启用模拟，表示没有 FPU
#define CR0_TS MASK(3)  // Task Switch 任务切换，延迟保存浮点环境
#define CR0_ET MASK(4)  // Extension Type 保留
#define CR0_NE MASK(5)  // Numeric Error 启用内部浮点错误报告
#define CR0_WP MASK(16) // Write Protect 写保护（禁止超级用户写入只读页）帮助写时复制
#define CR0_AM MASK(18) // Alignment Mask 对齐掩码
#define CR0_NW MASK(29) // Not Write-Through 不是直写
#define CR0_CD MASK(30) // Cache Disable 禁用内存缓冲
#define CR0_PG MASK(31) // Paging 启用分页

// PE: 保护模式
// MP: 协处理器
// EM: 仿真 x87
// TS: 任务切换
// ET: 扩展类型
// NE: 数学协处理器 (应该不会被使用)
// WP: 写保护
// AM: 对齐检查
// NW: 无写缓冲
// CD: 缓存禁止
// PG: 分页

#define CR4_VME        MASK(0)  // Virtual 8086 Mode Extensions
#define CR4_PVI        MASK(1)  // Protected-Mode Virtual Interrupts
#define CR4_TSD        MASK(2)  // Time Stamp Disable
#define CR4_DE         MASK(3)  // Debugging Extensions
#define CR4_PSE        MASK(4)  // Page Size Extensions
#define CR4_PAE        MASK(5)  // Physical Address Extension
#define CR4_MCE        MASK(6)  // Machine-Check Enable
#define CR4_PGE        MASK(7)  // Page Global Enable
#define CR4_PCE        MASK(8)  // Performance-Monitoring Counter Enable
#define CR4_OSFXSR     MASK(9)  // Operating System Support for FXSAVE and FXRSTOR instructions
#define CR4_OSXMMEXCPT MASK(10) // [CR4_OSXMMEXCPT]
#define CR4_UMIP       MASK(11) // User-Mode Instruction Prevention
#define CR4_LA57       MASK(12) // 5-Level Paging
#define CR4_VMXE       MASK(13) // VMX-Enable Bit
#define CR4_SMXE       MASK(14) // SMX-Enable Bit
#define CR4_FSGSBASE   MASK(16) // Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE
#define CR4_PCIDE      MASK(17) // PCID-Enable Bit
#define CR4_OSXSAVE    MASK(18) // XSAVE and Processor Extended States-Enable Bit
#define CR4_SMEP       MASK(20) // Supervisor Mode Execution Protection Enable
#define CR4_SMAP       MASK(21) // Supervisor Mode Access Prevention Enable
#define CR4_PKE        MASK(22) // Protection Key Enable
#define CR4_CET        MASK(23) // Control-flow Enforcement Technology
#define CR4_PKS        MASK(24) // Enable Protection Keys for Supervisor-Mode Pages
#define CR4_UINTR      MASK(26) // User Interrupts Enable

// [CR4_OSXMMEXCPT]: Operating System Support for Unmasked SIMD Floating-Point Exceptions

#define asm_get_xcr0()                                                                             \
  ({                                                                                               \
    u32 eax, edx;                                                                                  \
    asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));                                        \
    u64 xcr0 = ((u64)edx << 32) | eax;                                                             \
    xcr0;                                                                                          \
  })

#define asm_set_xcr0(xcr0)                                                                         \
  ({                                                                                               \
    u64 _xcr0_ = (xcr0);                                                                           \
    asm volatile("xsetbv" : : "a"((u32)_xcr0_), "d"((u32)(_xcr0_ >> 32)), "c"(0));                 \
  })

// set: 设置标志  clr: 清除标志

// --------------------------------------------------
//; cr0

#define asm_set_pe asm_set_cr0(asm_get_cr0() | CR0_PE)
#define asm_clr_pe asm_set_cr0(asm_get_cr0() & ~CR0_PE)

#define asm_set_mp asm_set_cr0(asm_get_cr0() | CR0_MP)
#define asm_clr_mp asm_set_cr0(asm_get_cr0() & ~CR0_MP)

#define asm_set_em asm_set_cr0(asm_get_cr0() | CR0_EM)
#define asm_clr_em asm_set_cr0(asm_get_cr0() & ~CR0_EM)

#define asm_set_ts asm_set_cr0(asm_get_cr0() | CR0_TS)
#define asm_clr_ts asm_set_cr0(asm_get_cr0() & ~CR0_TS)

#define asm_set_et asm_set_cr0(asm_get_cr0() | CR0_ET)
#define asm_clr_et asm_set_cr0(asm_get_cr0() & ~CR0_ET)

#define asm_set_ne asm_set_cr0(asm_get_cr0() | CR0_NE)
#define asm_clr_ne asm_set_cr0(asm_get_cr0() & ~CR0_NE)

#define asm_set_wp asm_set_cr0(asm_get_cr0() | CR0_WP)
#define asm_clr_wp asm_set_cr0(asm_get_cr0() & ~CR0_WP)

#define asm_set_am asm_set_cr0(asm_get_cr0() | CR0_AM)
#define asm_clr_am asm_set_cr0(asm_get_cr0() & ~CR0_AM)

#define asm_set_nw asm_set_cr0(asm_get_cr0() | CR0_NW)
#define asm_clr_nw asm_set_cr0(asm_get_cr0() & ~CR0_NW)

#define asm_set_cd asm_set_cr0(asm_get_cr0() | CR0_CD)
#define asm_clr_cd asm_set_cr0(asm_get_cr0() & ~CR0_CD)

#define asm_set_pg asm_set_cr0(asm_get_cr0() | CR0_PG)
#define asm_clr_pg asm_set_cr0(asm_get_cr0() & ~CR0_PG)

// --------------------------------------------------
//; cr4

#define asm_set_vme asm_set_cr4(asm_get_cr4() | CR4_VME)
#define asm_clr_vme asm_set_cr4(asm_get_cr4() & ~CR4_VME)

#define asm_set_pvi asm_set_cr4(asm_get_cr4() | CR4_PVI)
#define asm_clr_pvi asm_set_cr4(asm_get_cr4() & ~CR4_PVI)

#define asm_set_tsd asm_set_cr4(asm_get_cr4() | CR4_TSD)
#define asm_clr_tsd asm_set_cr4(asm_get_cr4() & ~CR4_TSD)

#define asm_set_de asm_set_cr4(asm_get_cr4() | CR4_DE)
#define asm_clr_de asm_set_cr4(asm_get_cr4() & ~CR4_DE)

#define asm_set_pse asm_set_cr4(asm_get_cr4() | CR4_PSE)
#define asm_clr_pse asm_set_cr4(asm_get_cr4() & ~CR4_PSE)

#define asm_set_pae asm_set_cr4(asm_get_cr4() | CR4_PAE)
#define asm_clr_pae asm_set_cr4(asm_get_cr4() & ~CR4_PAE)

#define asm_set_mce asm_set_cr4(asm_get_cr4() | CR4_MCE)
#define asm_clr_mce asm_set_cr4(asm_get_cr4() & ~CR4_MCE)

#define asm_set_pge asm_set_cr4(asm_get_cr4() | CR4_PGE)
#define asm_clr_pge asm_set_cr4(asm_get_cr4() & ~CR4_PGE)

#define asm_set_pce asm_set_cr4(asm_get_cr4() | CR4_PCE)
#define asm_clr_pce asm_set_cr4(asm_get_cr4() & ~CR4_PCE)

#define asm_set_osfxsr asm_set_cr4(asm_get_cr4() | CR4_OSFXSR)
#define asm_clr_osfxsr asm_set_cr4(asm_get_cr4() & ~CR4_OSFXSR)

#define asm_set_osxmmexcpt asm_set_cr4(asm_get_cr4() | CR4_OSXMMEXCPT)
#define asm_clr_osxmmexcpt asm_set_cr4(asm_get_cr4() & ~CR4_OSXMMEXCPT)

#define asm_set_umip asm_set_cr4(asm_get_cr4() | CR4_UMIP)
#define asm_clr_umip asm_set_cr4(asm_get_cr4() & ~CR4_UMIP)

#define asm_set_la57 asm_set_cr4(asm_get_cr4() | CR4_LA57)
#define asm_clr_la57 asm_set_cr4(asm_get_cr4() & ~CR4_LA57)

#define asm_set_vmx asm_set_cr4(asm_get_cr4() | CR4_VMXE)
#define asm_clr_vmx asm_set_cr4(asm_get_cr4() & ~CR4_VMXE)

#define asm_set_smx asm_set_cr4(asm_get_cr4() | CR4_SMXE)
#define asm_clr_smx asm_set_cr4(asm_get_cr4() & ~CR4_SMXE)

#define asm_set_fsgsbase asm_set_cr4(asm_get_cr4() | CR4_FSGSBASE)
#define asm_clr_fsgsbase asm_set_cr4(asm_get_cr4() & ~CR4_FSGSBASE)

#define asm_set_pcide asm_set_cr4(asm_get_cr4() | CR4_PCIDE)
#define asm_clr_pcide asm_set_cr4(asm_get_cr4() & ~CR4_PCIDE)

#define asm_set_osxsave asm_set_cr4(asm_get_cr4() | CR4_OSXSAVE)
#define asm_clr_osxsave asm_set_cr4(asm_get_cr4() & ~CR4_OSXSAVE)

#define asm_set_smep asm_set_cr4(asm_get_cr4() | CR4_SMEP)
#define asm_clr_smep asm_set_cr4(asm_get_cr4() & ~CR4_SMEP)

#define asm_set_smap asm_set_cr4(asm_get_cr4() | CR4_SMAP)
#define asm_clr_smap asm_set_cr4(asm_get_cr4() & ~CR4_SMAP)

#define asm_set_pke asm_set_cr4(asm_get_cr4() | CR4_PKE)
#define asm_clr_pke asm_set_cr4(asm_get_cr4() & ~CR4_PKE)

#define asm_set_cet asm_set_cr4(asm_get_cr4() | CR4_CET)
#define asm_clr_cet asm_set_cr4(asm_get_cr4() & ~CR4_CET)

#define asm_set_pks asm_set_cr4(asm_get_cr4() | CR4_PKS)
#define asm_clr_pks asm_set_cr4(asm_get_cr4() & ~CR4_PKS)

#define asm_set_uintr asm_set_cr4(asm_get_cr4() | CR4_UINTR)
#define asm_clr_uintr asm_set_cr4(asm_get_cr4() & ~CR4_UINTR)
