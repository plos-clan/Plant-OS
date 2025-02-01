#include <kernel.h>

size_t total_mem_size;
byte  *IVT;

void abort() {
  kloge("aborted");
  infinite_loop {
    asm_cli, asm_hlt;
  }
}

void serial_init();

void sysinit() {
  klogi("kernel is starting");

  cpuid_do_cache(); // 缓存 CPUID 信息

  klogd("%s", cpuids.x2apic ? "x2apic" : "apic");

  //+ ========== fpu / sse / avx ==========

  if (cpuids.fpu) klogi("Your cpu supports fpu.");
  if (cpuids.sse) klogi("Your cpu supports sse.");
  if (cpuids.avx) klogi("Your cpu supports avx.");

  kassert(!!cpuids.avx == !!cpuids.xsave, "That's impossible.");
  kassert(!!cpuids.avx == !!cpuids.osxsave, "Maybe that's an error of QEMU.");

  if (cpuids.fpu) asm_clr_ts, asm_clr_em;
  if (cpuids.sse) asm_set_mp, asm_set_osfxsr, asm_set_osxmmexcpt;
  if (cpuids.avx) {
    asm_set_osxsave;
    const u64 xcr0 = asm_get_xcr0();
    asm_set_xcr0(xcr0 | 0x6);
  }

  if (cpuids.fpu) asm volatile("fnclex\n\tfninit\n\t" ::: "memory");
  const u32 value = 0x1f80;
  if (cpuids.sse) asm volatile("ldmxcsr (%0)" ::"r"(&value));
  if (cpuids.avx) asm volatile("vzeroupper\n\t");

  klogd("fpu/sse/avx inited.");

  //+ ========== vm ==========

  detect_hypervisor();

  switch (hypervisor_id) {
  case HYPERVISOR_KVM: klogi("You are running on KVM."); break;
  case HYPERVISOR_KVM_HV: klogi("You are running on KVM (Hyper-V emulation)."); break;
  case HYPERVISOR_QEMU: klogi("You are running on QEMU."); break;
  case HYPERVISOR_VMWARE: klogi("You are running on VMware."); break;
  case HYPERVISOR_VBOX: klogi("You are running on VBox."); break;
  case HYPERVISOR_HYPERV: klogi("You are running on Hyper-V."); break;
  default: break;
  }

  if (vmware_backdoor_available) klogi("VMware Backdoor available.");

  //+ ========== VT-x / AMD-V ==========

  if (cpuids.vmx) klogi("Your cpu supports VT-x.");
  if (cpuids.svm) klogi("Your cpu supports AMD-V.");

  //+ ========== memory / paging ==========

  total_mem_size = memtest(0x00400000, 0xbfffffff);
  init_paging();

  IVT = page_alloc(0x500);
#if USE_UBSCAN
  klogi("蹲一个 UB, 马上要把 0 地址的 IVT 备份了啊");
#endif
  memcpy(IVT, null, 0x500); // 这是正确的，忽略这个 warning

  memory_init(page_alloc(SIZE_2M), SIZE_2M);

  //+ ========== ==========

  init_pic();
  serial_init();
  init_pit();

  asm_sti;

  vbe_init();

  init_tty();
  screen_clear();

  pci_init();

  virtio_init();
  virtio_gpu_init();
  init_acpi();
  sb16_init();
  hda_init();
  hda_regist();
  vdisk_init();
  vfs_init();
  devfs_regist();
  fatfs_regist();
  tmpfs_regist();
  iso9660_regist();

  resman_descriptors_init(); // 初始化文件描述符表

  if (total_mem_size < 256 * 1024 * 1024) {
    fatal("You should have at least 256MiB memory in your pc to start Plant-OS.");
  } else {
    info("the memory test has been passed! Your PC has %dMiB memory",
         total_mem_size / (1024 * 1024));
  }

  keyboard_init();
  mouse_init();

  vmtools_init();
}
