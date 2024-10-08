#pragma once
#include <define.h>

typedef struct {
  u32 eax, ebx, ecx, edx, esi, edi, ebp;
  u32 eip;
} stack_frame;

typedef struct intr_frame_t {
  u32 edi;
  u32 esi;
  u32 ebp;
  // 虽然 pushad 把 esp 也压入，但 esp 是不断变化的，所以会被 popad 忽略
  u32 esp_dummy;

  u32 ebx;
  u32 edx;
  u32 ecx;
  u32 eax;

  u32 gs;
  u32 fs;
  u32 es;
  u32 ds;

  u32 eip;
  u32 cs;
  u32 eflags;
  u32 esp;
  u32 ss;
} intr_frame_t;

typedef struct __PACKED__ fpu_regs {
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
} fpu_regs_t;

void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void load_tr(int tr);

#define SA_RPL_MASK      0xFFFC
#define SA_TI_MASK       0xFFFB
#define SA_TIL           4 // 设置此项，将从LDT中寻找
#define SA_RPL0          0
#define SA_RPL1          1
#define SA_RPL2          2
#define SA_RPL3          3
#define GET_SEL(cs, rpl) ((cs & SA_RPL_MASK & SA_TI_MASK) | (rpl))

typedef struct TSS32 {
  i32 backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
  i32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  i32 es, cs, ss, ds, fs, gs;
  i32 ldtr, iomap;
} TSS32;

#define ADR_IDT      0x0026f800
#define LIMIT_IDT    0x000007ff
#define ADR_GDT      0x00270000
#define LIMIT_GDT    0x0000ffff
#define ADR_BOTPAK   0x100000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_DATA16_RW 0x0092
#define AR_CODE32_ER 0x409a
#define AR_CODE16_ER 0x009a
#define AR_INTGATE32 0x008e
#define AR_TSS32     0x0089

struct SEGMENT_DESCRIPTOR {
  i16  limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
  i16  offset_low, selector;
  char dw_count, access_right;
  i16  offset_high;
};

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, u32 limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, size_t offset, int selector, int ar);

void asm_error0();
void asm_error1();
void asm_error3();
void asm_error4();
void asm_error5();
void asm_error6();
void asm_error7();
void asm_error8();
void asm_error9();
void asm_error10();
void asm_error11();
void asm_error12();
void asm_error13();
void asm_error14();
void asm_error16();
void asm_error17();
void asm_error18();

void asm_inthandler36() __attr(naked);
void asm_inthandler72() __attr(naked);

void asm_gui_api();
void asm_net_api();

void asm_inthandler2c();
void asm_inthandler20();
void asm_inthandler21();

void asm_ide_irq();

typedef struct {
  u16 di, si, bp, sp, bx, dx, cx, ax;
  u16 gs, fs, es, ds, eflags;
} regs16;

void int32(u8 intnum, regs16 *regs);
void asm16_int(u8 intnum, regs16 *regs);
void do_init_seg_register();
void init_page();
void init_gdtidt();
void fpu_disable();
bool interrupt_disable();
void set_interrupt_state(bool state);
void register_intr_handler(int num, int addr);
