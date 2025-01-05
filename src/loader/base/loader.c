#include <loader.h>

struct TASK MainTask;

bool elf32_is_validate(Elf32_Ehdr *hdr) {
  return hdr->e_ident[EI_MAG0] == ELFMAG0 && hdr->e_ident[EI_MAG1] == ELFMAG1 &&
         hdr->e_ident[EI_MAG2] == ELFMAG2 && hdr->e_ident[EI_MAG3] == ELFMAG3;
}

void load_segment(Elf32_Phdr *phdr, void *elf) {
  klogf("%08x %08x %d\n", phdr->p_vaddr, phdr->p_offset, phdr->p_filesz);
  memcpy((void *)phdr->p_vaddr, elf + phdr->p_offset, phdr->p_filesz);
  if (phdr->p_memsz > phdr->p_filesz) { // 这个是bss段
    memset((void *)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
  }
}

u32 load_elf(Elf32_Ehdr *hdr) {
  Elf32_Phdr *phdr = (Elf32_Phdr *)((u32)hdr + hdr->e_phoff);
  for (int i = 0; i < hdr->e_phnum; i++) {
    load_segment(phdr, (void *)hdr);
    phdr++;
  }
  return hdr->e_entry;
}

void read_pci_class(u8 bus, u8 device, u8 function, u8 *class_code, u8 *subclass_code) {
  // 读取设备类别和子类别寄存器的值
  u32 class_register = read_pci(bus, device, function, 0x08);
  // 解析设备类别和子类别
  *class_code    = (class_register >> 24) & 0xFF;
  *subclass_code = (class_register >> 16) & 0xFF;
}

static bool is_ide_device(u8 bus, u8 device, u8 function) {
  u8 class_code, subclass_code;
  // 读取设备类别和子类别
  read_pci_class(bus, device, function, &class_code, &subclass_code);
  // 检查设备类别和子类别是否符合IDE设备的类别码
  return class_code == 0x01 && subclass_code == 0x01;
}

int get_vdisk_type(int drive);

void DOSLDR_MAIN() {
  cpuid_do_cache(); // 缓存 CPUID 信息

  if (cpuids.fpu) asm_clr_ts, asm_clr_em;
  if (cpuids.sse) asm_set_mp, asm_set_osfxsr, asm_set_osxmmexcpt;
  if (cpuids.avx) {
    asm_set_osxsave;
    const u64 xcr0 = asm_get_xcr0();
    asm_set_xcr0(xcr0 | 0x6);
  }

  if (cpuids.fpu) asm volatile("fninit\n\t" ::: "memory");
  const u32 value = 0x1f80;
  if (cpuids.sse) asm volatile("ldmxcsr (%0)" ::"r"(&value));
  if (cpuids.avx) asm volatile("vzeroupper\n\t");

  init_pic();
  asm_sti;
  u32 memtotal = 128 * 1024 * 1024;
  memman_init((void *)0x00600000, memtotal - 0x00600000);
  screen_clear();
  vdisk_init();
  init_vfs();
  floppy_init();
  Register_fat_fileSys();
  vdisk vd;
  vd.flag  = 1;
  vd.read  = NULL;
  vd.write = NULL;
  vd.size  = 1;
  regist_vdisk(vd);
  ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
  ahci_init();
  char default_drive;
  u32  default_drive_number;
  if (memcmp((void *)"FAT12   ", (void *)0x7c00 + BS_FileSysType, 8) == 0 ||
      memcmp((void *)"FAT16   ", (void *)0x7c00 + BS_FileSysType, 8) == 0) {
    if (*(u8 *)(0x7c00 + BS_DrvNum) >= 0x80) {
      default_drive_number = *(u8 *)(0x7c00 + BS_DrvNum) - 0x80 + 0x02;
    } else {
      default_drive_number = *(u8 *)(0x7c00 + BS_DrvNum);
    }
  } else if (memcmp((void *)"FAT32   ", (void *)0x7c00 + BPB_Fat32ExtByts + BS_FileSysType, 8) ==
             0) {
    if (*(u8 *)(0x7c00 + BPB_Fat32ExtByts + BS_DrvNum) >= 0x80) {
      default_drive_number = *(u8 *)(0x7c00 + BPB_Fat32ExtByts + BS_DrvNum) - 0x80 + 0x02;
    } else {
      default_drive_number = *(u8 *)(0x7c00 + BPB_Fat32ExtByts + BS_DrvNum);
    }
  } else {
    if (*(u8 *)(0x7c00) >= 0x80) {
      default_drive_number = *(u8 *)(0x7c00) - 0x80 + 0x02;
    } else {
      default_drive_number = *(u8 *)(0x7c00);
    }
  }
  default_drive = default_drive_number + 0x41;
  if (get_vdisk_type(default_drive) != 1) { default_drive++; }
  NowTask()->drive        = default_drive;
  NowTask()->drive_number = default_drive_number;
  vfs_mount_disk(NowTask()->drive, NowTask()->drive);
  vfs_change_disk(NowTask()->drive);
  klogf("Plant-OS loader (Refactored)\n");
  klogf("MIT License\n");
  klogf("Copyright (c) 2024 plos-clan\n");
  klogf("memtotal=%dMB\n", memtotal / 1024 / 1024);
  char path[15] = " :\\kernel.bin";
  path[0]       = NowTask()->drive;
  klogf("Load file:%s\n", path);
  int sz = vfs_filesize(path);
  if (sz == -1) {
    klogf("DOSLDR can't find kernel.bin in Drive %c", path[0]);
    infinite_loop;
  }
  // printf("fp = %08x\n%d\n",fp, fp->size);
  char *s = page_alloc(sz);
  klogf("Will load in %08x size = %08x\n", s, sz);
  vfs_readfile(path, s);
  klogf("Loading...\n");
  u32 entry = load_elf((Elf32_Ehdr *)s);

  asm volatile("push %0\n\t"
               "ret\n\t" ::"r"(entry));
}

struct TASK *NowTask() {
  return &MainTask;
}
