#include <kernel.h>

void set_segmdesc(SegmentDescriptor *sd, u32 limit, u32 base, u32 ar) {
  if (limit > 0xfffff) {
    ar    |= 0x8000; // G_bit = 1
    limit /= 0x1000;
  }
  sd->limit_low    = limit & 0xffff;
  sd->base_low     = base & 0xffff;
  sd->base_mid     = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high    = (base >> 24) & 0xff;
}

void set_gatedesc(GateDescriptor *gd, size_t offset, u32 selector, u32 ar) {
  gd->offset_low   = offset & 0xffff;
  gd->selector     = selector;
  gd->dw_count     = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high  = (offset >> 16) & 0xffff;
}

extern void asm_inthandler() __attr(naked);
extern void inthandler(i32 id, regs32 *regs) __attr(fastcall);

static void (*const asm_handlers[IDT_LEN])() = {
    [0x00] = asm_error0,  [0x01] = asm_error1,  [0x03] = asm_error3,  [0x04] = asm_error4,
    [0x05] = asm_error5,  [0x06] = asm_error6,  [0x07] = asm_error7,  [0x08] = asm_error8,
    [0x09] = asm_error9,  [0x0a] = asm_error10, [0x0b] = asm_error11, [0x0c] = asm_error12,
    [0x0d] = asm_error13, [0x0e] = asm_error14, [0x10] = asm_error16, [0x11] = asm_error17,
    [0x12] = asm_error18,
};

void inthandler2c(i32 id, regs32 *regs);
void ide_irq(i32 id, regs32 *regs);

static inthandler_t handlers[IDT_LEN] = {
    [0x2c]      = inthandler2c, // 鼠标中断
    [0x20 + 14] = ide_irq,      // IDE中断
    [0x20 + 15] = ide_irq,      // IDE中断
};

size_t syscall(size_t eax, size_t ebx, size_t ecx, size_t edx, size_t esi, size_t edi);

__attr(fastcall) void inthandler(i32 id, regs32 *regs) {
  if (id >= 0x20 && id < 0x30) send_eoi(id - 0x20);
  if (id == 0x36) {
    regs->eax = syscall(regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
  } else if (handlers[id]) {
    handlers[id](id, regs);
  } else {
    klogd("Unknown interrupt %02x (%d)", id, id);
  }
}

inthandler_t inthandler_get(i32 id) {
  return handlers[id];
}

inthandler_t inthandler_set(i32 id, inthandler_t handler) {
  var old      = handlers[id];
  handlers[id] = handler;
  return old;
}

void init_gdtidt() {
  // 初始化 GDT
  var gdt = (SegmentDescriptor *)GDT_ADDR;
  for (int i = 0; i < GDT_LEN; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_CODE32_ER);
  set_segmdesc(gdt + 3, 0xffffffff, 0x00000000, AR_DATA32_RW | 3 << 5);
  set_segmdesc(gdt + 4, 0xffffffff, 0x00000000, AR_CODE32_ER | 3 << 5);
  set_segmdesc(gdt + 5, 0xffffffff, 0x70000000, AR_DATA32_RW | 3 << 5);
  set_segmdesc(gdt + 1000, 0xffffffff, 0, AR_CODE32_ER); // CODE32
  set_segmdesc(gdt + 1001, 0xfffff, 0, AR_CODE16_ER);    // CODE16
  set_segmdesc(gdt + 1002, 0xfffff, 0, AR_DATA16_RW);    // DATA16
  load_gdt(gdt, GDT_LEN);                                // 加载GDT表

  // 初始化 IDT
  var idt = (GateDescriptor *)IDT_ADDR;
  for (size_t i = 0; i < IDT_LEN; i++) {
    int    ar      = i >= 0x30 ? AR_INTGATE32 | 3 << 5 : AR_INTGATE32;
    size_t handler = (size_t)&asm_inthandler + i * 7;
    set_gatedesc(idt + i, (size_t)asm_handlers[i] ?: handler, 2 * 8, ar);
  }
  load_idt(idt, IDT_LEN); // 加载IDT表
}
