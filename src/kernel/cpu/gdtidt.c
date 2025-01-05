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

// --------------------------------------------------
//; 通用处理逻辑

static inthandler_t handlers[IDT_LEN];

size_t syscall(size_t eax, size_t ebx, size_t ecx, size_t edx, size_t esi, size_t edi);

FASTCALL void inthandler(i32 id, regs32 *regs) {
  if (id != 0x07 && id != 0x2d) asm_set_ts;
  if (id >= 0x20 && id < 0x30) send_eoi(id - 0x20);

  if (acpi_inited) {
    timespec time;
    gettime_ns(&time); // 同时更新时间
    // ksrand((u32)time.tv_nsec);
  }

  if (id == 0x36) {
    regs->eax = syscall(regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
  } else if (handlers[id]) {
    handlers[id](id, regs);
  } else {
    klogw("Unknown interrupt %02x (%d)", id, id);
  }

  if (id != 0x07 && id != 0x2d) asm_set_ts;
}

inthandler_t inthandler_get(i32 id) {
  kassert(0 <= id && id < IDT_LEN);
  return handlers[id];
}

inthandler_t inthandler_set(i32 id, inthandler_t handler) {
  kassert(0 <= id && id < IDT_LEN);
  var old      = handlers[id];
  handlers[id] = handler;
  return old;
}

// --------------------------------------------------
//; 初始化GDT和IDT

extern void asm_inthandler() __attr(naked);

void init_gdtidt() {
  // 初始化 GDT
  var gdt = (SegmentDescriptor *)GDT_ADDR;
  for (int i = 0; i < GDT_LEN; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, U32_MAX, 0, AR_CODE32_ER);
  set_segmdesc(gdt + 2, U32_MAX, 0, AR_DATA32_RW);
  set_segmdesc(gdt + 3, U32_MAX, 0, AR_CODE32_ER | 3 << 5);
  set_segmdesc(gdt + 4, U32_MAX, 0, AR_DATA32_RW | 3 << 5);
  load_gdt(gdt, GDT_LEN); // 加载GDT表

  asm_wrmsr(IA32_SYSENTER_CS, RING0_CS, 0); // 为 sysenter 设置 msr

  // 初始化 IDT
  var idt = (GateDescriptor *)IDT_ADDR;
  for (size_t i = 0; i < IDT_LEN; i++) {
    int    ar      = i >= 0x30 ? AR_INTGATE32 | 3 << 5 : AR_INTGATE32;
    size_t handler = (size_t)&asm_inthandler + i * 7;
    set_gatedesc(idt + i, handler, RING0_CS, ar);
  }
  load_idt(idt, IDT_LEN); // 加载IDT表
}
