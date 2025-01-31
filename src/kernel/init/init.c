#include <kernel.h>

size_t total_mem_size;
byte  *IVT;

void abort() {
  kloge("aborted");
  infinite_loop {
    asm_cli, asm_hlt;
  }
}

#define vmware_send(cmd, arg)                                                                      \
  ({                                                                                               \
    __SIZE_TYPE__ eax = 0x564D5868;                                                                \
    __SIZE_TYPE__ ebx = (arg);                                                                     \
    __SIZE_TYPE__ ecx = (cmd);                                                                     \
    __SIZE_TYPE__ edx = 0x5658;                                                                    \
    asm volatile("in %%dx, %%eax\n\t" : "+a"(eax), "+b"(ebx), "+c"(ecx), "+d"(edx));               \
    ebx;                                                                                           \
  })

#define vm_is_vmware()                                                                             \
  ({                                                                                               \
    __SIZE_TYPE__ eax = 0x564D5868;                                                                \
    __SIZE_TYPE__ ebx = 0;                                                                         \
    __SIZE_TYPE__ ecx = 0x0A;                                                                      \
    __SIZE_TYPE__ edx = 0x5658;                                                                    \
    asm volatile("in %%dx, %%eax\n\t" : "+a"(eax), "+b"(ebx), "+c"(ecx), "+d"(edx));               \
    ebx == 0x564D5868;                                                                             \
  })

void serial_init();

bool vmware_backdoor_available = false;

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

  if (memeq(cpuids.hypervisor_id, "KVMKVMKVM", 9)) {
    klogi("You are running on KVM."); //
  }

  if (memeq(cpuids.hypervisor_id, "Linux KVM Hv", 12)) {
    klogi("You are running on KVM (Hyper-V emulation)."); //
  }

  if (memeq(cpuids.hypervisor_id, "TCGTCGTCGTCG", 12)) {
    klogi("You are running on QEMU."); //
  }

  if (memeq(cpuids.hypervisor_id, "VMwareVMware", 12)) {
    klogi("You are running on VMware."); //
  }

  if (memeq(cpuids.hypervisor_id, "VBoxVBoxVBox", 12)) {
    klogi("You are running on VBox."); //
  }

  if (memeq(cpuids.hypervisor_id, "Microsoft Hv", 9)) {
    klogi("You are running on Hyper-V."); //
  }

  if (vm_is_vmware()) {
    klogi("VMware Backdoor available."); //
  }

  // 取消鼠标捕获
  vmware_send(41, 0x45414552);
  vmware_send(40, 0);
  vmware_send(39, 1);
  vmware_send(41, 0x53424152);

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

  init_pci(page_alloc(1 * 1024 * 1024));

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
}
