#pragma once
#include <define.h>

// 遵守 sysenter/sysexit 的要求
// 原则上不应该修改，如要修改需同时修改汇编中的定义
#define RING0_CS (1 * 8)
#define RING0_DS (2 * 8)
#define RING3_CS (3 * 8)
#define RING3_DS (4 * 8)

typedef struct {
  u32 edi, esi, ebp, _, ebx, edx, ecx, eax;
  u32 eip;
} stack_frame;

#define SA_RPL_MASK      0xFFFC
#define SA_TI_MASK       0xFFFB
#define SA_TIL           4 // 设置此项，将从LDT中寻找
#define SA_RPL0          0
#define SA_RPL1          1
#define SA_RPL2          2
#define SA_RPL3          3
#define GET_SEL(cs, rpl) ((cs & SA_RPL_MASK & SA_TI_MASK) | (rpl))

typedef struct __PACKED__ TSS32 {
  u32  backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
  u32  eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  u32  es, cs, ss, ds, fs, gs;
  u32  ldtr;
  u16  trap, iomap;
  byte io_map[8192];
} TSS32;

#define IDT_ADDR 0x0026f800 // IDT 地址

#define IDT_LEN      256
#define GDT_ADDR     0x00270000 // GDT 地址
#define GDT_LEN      8192
#define ADR_BOTPAK   0x100000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_DATA16_RW 0x0092
#define AR_CODE32_ER 0x409a
#define AR_CODE16_ER 0x009a
#define AR_INTGATE32 0x008e
#define AR_TSS32     0x0089

typedef struct SegmentDescriptor {
  u16  limit_low;
  u16  base_low;
  byte base_mid;
  byte access_right;
  byte limit_high;
  byte base_high;
} SegmentDescriptor;

typedef struct GateDescriptor {
  u16  offset_low;
  u16  selector;
  byte dw_count;
  byte access_right;
  u16  offset_high;
} GateDescriptor;

void set_segmdesc(SegmentDescriptor *sd, u32 limit, u32 base, u32 ar);
void set_gatedesc(GateDescriptor *gd, size_t offset, u32 selector, u32 ar);

typedef struct regs16 {
  u16 di, si, bp, _, bx, dx, cx, ax;
  u16 gs, fs, es, ds;
} regs16;

// 可参考 https://www.felixcloutier.com/x86/iret:iretd:iretq

/**
 *\brief 32 位保护模式下中断处理程序的寄存器
 *
 */
typedef struct regs32 {
  u32 edi, esi, ebp, _, ebx, edx, ecx, eax;
  u32 gs, fs, es, ds;
  u32 id, err; // err: 当中断具有错误码时为错误码，否则为中断号
  u32 eip, cs, flags, esp, ss;
} regs32;

// 目前 plos 并没有 64 位的实现

typedef struct regs64 {
  u64 r15, r14, r13, r12, r11, r10, r9, r8;
  u64 rdi, rsi, rbp, _, rbx, rdx, rcx, rax;
  u64 gs, fs, es, ds;
  u64 id, err; // err: 当中断具有错误码时为错误码，否则为中断号
  u64 rip, cs, rflags, rsp, ss;
} regs64;

void v86_int(byte intnum, regs16 *regs); // 调用虚拟86模式中断
void init_paging();                      // 初始化分页
void init_gdtidt();                      // 初始化全局描述符表和中断描述符表
void init_error_inthandler();            // 初始化错误中断处理程序
void fpu_enable();                       // 启用浮点运算单元
void fpu_disable();                      // 禁用浮点运算单元
void fpu_do_save(void *extra_regs);
void fpu_save(void *extra_regs);
void fpu_init();

#if __x86_64__
typedef void(inthandler_f)(i32 id, regs64 *regs);
typedef void (*inthandler_t)(i32 id, regs64 *regs);
#else
//. 注意 fastcall 调用约定
typedef void(inthandler_f)(i32 id, regs32 *regs) FASTCALL;
//. 注意 fastcall 调用约定
typedef void (*inthandler_t)(i32 id, regs32 *regs) FASTCALL;
#endif

/**
 *\brief 获取中断处理函数
 *
 *\param id       中断号
 *\return 处理函数
 */
inthandler_t inthandler_get(i32 id);

/**
 *\brief 设置中断处理函数
 *
 *\param id       中断号
 *\param handler  处理函数
 *\return 之前的处理函数
 */
inthandler_t inthandler_set(i32 id, inthandler_t handler);

//! 仅用于多任务切换到用户态
//!   不能直接调用！请：
//:     asm volatile("mov %0, %%esp\n\t" ::"r"(iframe));
//:     asm volatile("jmp asm_inthandler_quit\n\t");
//:     __builtin_unreachable();
void asm_inthandler_quit();
void asm_syscall_quit();
