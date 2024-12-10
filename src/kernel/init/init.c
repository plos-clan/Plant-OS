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

void init_serial();

#pragma clang optimize off

void sysinit() {
  total_mem_size = memtest(0x00400000, 0xbfffffff);
  init_page();

  IVT = page_alloc(0x500);
  klogi("蹲一个 UB, 马上要把 0 地址的 IVT 备份了啊");
  memcpy(IVT, null, 0x500); // 这是正确的，忽略这个 warning
  // init_gdtidt();
  init_pic();
  init_serial();
  init_pit();

  asm_sti;
  irq_enable(0);
  irq_enable(1);
  asm_set_em, asm_set_ts, asm_set_ne;

  memory_init(page_alloc(128 * 1024 * 1024), 128 * 1024 * 1024);

  vbe_init();

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
