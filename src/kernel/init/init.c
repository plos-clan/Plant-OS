#include <kernel.h>

struct MOUSE_DEC mdec;
size_t           memsize;
byte            *IVT;

void abort() {
  infinite_loop {
    asm_cli, asm_hlt;
  }
}

void *pci_addr_base;

#pragma clang optimize off

void sysinit() {
  do_init_seg_register();
  memsize = memtest(0x00400000, 0xbfffffff);
  init_page();
  IVT = page_alloc(0x10000);
  memcpy(IVT, null, 0x10000); // 这是正确的，忽略这个 warning
  // init_gdtidt();
  init_pic();

  asm_sti;
  irq_mask_clear(0);
  irq_mask_clear(1);
  asm_set_em, asm_set_ts, asm_set_ne;

  memory_init(page_alloc(128 * 1024 * 1024), 128 * 1024 * 1024);

  // 可以 setmode

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

  if (memsize < 256 * 1024 * 1024) {
    fatal("You should have at least 256MB memory in your pc to start Plant-OS.");
  } else {
    info("the memory test has been passed! Your PC has %dMB memory", memsize / (1024 * 1024));
  }
  init_keyboard();
}
