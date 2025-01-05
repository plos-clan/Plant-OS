#include <kernel.h>

size_t total_mem_size;
byte  *IVT;

void abort() {
  kloge("aborted");
  infinite_loop {
    asm_cli, asm_hlt;
  }
}

void *pci_addr_base;

void init_serial();
void virtio_init();
void virtio_gpu_init();

#define KERNEL_HEAP_SIZE (128 * 1024 * 1024)

void sysinit() {
  klogi("kernel is starting");

  cpuid_do_cache(); // 缓存 CPUID 信息

  klogd("%s", cpuids.x2apic ? "x2apic" : "apic");

  if (cpuids.fpu) asm_clr_ts, asm_clr_em;
  if (cpuids.sse) asm_set_osfxsr, asm_set_osxmmexcpt;

  total_mem_size = memtest(0x00400000, 0xbfffffff);
  init_paging();

  IVT = page_alloc(0x500);
#if USE_UBSCAN
  klogi("蹲一个 UB, 马上要把 0 地址的 IVT 备份了啊");
#endif
  memcpy(IVT, null, 0x500); // 这是正确的，忽略这个 warning
  init_pic();
  init_serial();
  init_pit();

  asm_sti;

  memory_init(page_alloc(KERNEL_HEAP_SIZE), KERNEL_HEAP_SIZE);

  vbe_init();

  init_tty();
  screen_clear();

  pci_addr_base = page_alloc(1 * 1024 * 1024);
  init_pci(pci_addr_base);

  virtio_init();
  virtio_gpu_init();
  init_acpi();
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
