#pragma once
#include <define.h>
#include <type.h>
typedef struct {
  u32 eax, ebx, ecx, edx, esi, edi, ebp;
  u32 eip;
} stack_frame;
typedef struct intr_frame_t {
  unsigned edi;
  unsigned esi;
  unsigned ebp;
  // 虽然 pushad 把 esp 也压入，但 esp 是不断变化的，所以会被 popad 忽略
  unsigned esp_dummy;

  unsigned ebx;
  unsigned edx;
  unsigned ecx;
  unsigned eax;

  unsigned gs;
  unsigned fs;
  unsigned es;
  unsigned ds;

  unsigned eip;
  unsigned cs;
  unsigned eflags;
  unsigned esp;
  unsigned ss;
} intr_frame_t;
typedef struct fpu_t {
  u16 control;
  u16 RESERVED1;
  u16 status;
  u16 RESERVED2;
  u16 tag;
  u16 RESERVED3;
  u32 fip0;
  u32 fop0;
  u32 fdp0;
  u32 fdp1;
  u8  regs[80];
} __PACKED__ fpu_t;

void io_hlt();
void io_stihlt();
int  io_in8(int port);
int  io_in16(int port);
int  io_in32(int port);
void io_out8(int port, int data);
void io_out16(int port, int data);
void io_out32(int port, int data);
int  io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int  load_cr0(void);
void store_cr0(int cr0);
u32  get_cr0();
void set_cr0(u32 cr0);
#define SA_RPL_MASK      0xFFFC
#define SA_TI_MASK       0xFFFB
#define SA_TIL           4 // 设置此项，将从LDT中寻找
#define SA_RPL0          0
#define SA_RPL1          1
#define SA_RPL2          2
#define SA_RPL3          3
#define GET_SEL(cs, rpl) ((cs & SA_RPL_MASK & SA_TI_MASK) | (rpl))
finline void set_cr3(u32 pde) {
  asm volatile("movl %%eax, %%cr3\n" ::"a"(pde));
}

struct TSS32 {
  int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
  int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  int es, cs, ss, ds, fs, gs;
  int ldtr, iomap;
};