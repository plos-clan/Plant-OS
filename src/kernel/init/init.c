#include <kernel.h>

struct MOUSE_DEC mdec;
size_t           total_mem_size;
byte            *IVT;

void abort() {
  kloge("aborted");
  infinite_loop {
    asm_cli, asm_hlt;
  }
}

void *pci_addr_base;

#pragma clang optimize off

void sysinit() {
  u16 ax_value = 0x1234; // 16位寄存器值
  u8  al_value = 0x56;   // 8位寄存器值
  u16 result16;
  u8  result8;

  // 使用内联汇编传入16位寄存器
  asm volatile("movw %1, %%ax\n\t"
               "movw %%ax, %0\n\t"
               : "=r"(result16) // 输出操作数
               : "r"(ax_value)  // 输入操作数
               : "%ax"          // 被修改的寄存器
  );

  // 使用内联汇编传入8位寄存器
  asm volatile("movb %1, %%al\n\t"
               "movb %%al, %0\n\t"
               : "=r"(result8) // 输出操作数
               : "r"(al_value) // 输入操作数
               : "%al"         // 被修改的寄存器
  );

  total_mem_size = memtest(0x00400000, 0xbfffffff);
  init_page();

  IVT = page_alloc(0x500);
  memcpy(IVT, null, 0x500); // 这是正确的，忽略这个 warning
  // init_gdtidt();
  init_pic();

  asm_sti;
  irq_mask_clear(0);
  irq_mask_clear(1);
  asm_set_em, asm_set_ts, asm_set_ne;

  memory_init(page_alloc(128 * 1024 * 1024), 128 * 1024 * 1024);

  vbe_init();

  init_pit();
  init_tty();
  screen_clear();

  pci_addr_base = page_alloc(1 * 1024 * 1024);
  init_pci(pci_addr_base);

  sb16_init();
  sb16_regist();
  hda_init();
  hda_regist();
  vdisk_init();
  vfs_init();
  devfs_regist();
  fatfs_regist();
  tmpfs_regist();
  iso9660_regist();

  if (total_mem_size < 256 * 1024 * 1024) {
    fatal("You should have at least 256MiB memory in your pc to start Plant-OS.");
  } else {
    info("the memory test has been passed! Your PC has %dMiB memory",
         total_mem_size / (1024 * 1024));
  }
  init_keyboard();
}
