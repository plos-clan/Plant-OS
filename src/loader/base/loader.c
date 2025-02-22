#include "../../apps/include/zstd.h"
#include <loader.h>

struct TASK MainTask;

bool elf32_is_validate(const Elf32Header *hdr) {
  return *(u32 *)hdr->ident == ELF_MAGIC;
}

void load_segment(Elf32ProgramHeader *phdr, void *elf) {
  klogf("%08x %08x %d\n", phdr->p_vaddr, phdr->p_offset, phdr->p_filesz);
  memcpy((void *)phdr->p_vaddr, elf + phdr->p_offset, phdr->p_filesz);
  if (phdr->p_memsz > phdr->p_filesz) { // 这个是bss段
    memset((void *)(phdr->p_vaddr + phdr->p_filesz), 0, phdr->p_memsz - phdr->p_filesz);
  }
}

u32 load_elf(Elf32Header *hdr) {
  Elf32ProgramHeader *phdr = (Elf32ProgramHeader *)((u32)hdr + hdr->e_phoff);
  for (int i = 0; i < hdr->e_phnum; i++) {
    load_segment(phdr, (void *)hdr);
    phdr++;
  }
  return hdr->entry;
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

  if (cpuids.fpu) asm volatile("fnclex\n\tfninit\n\t" ::: "memory");
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
  if (memeq("FAT12   ", (void *)0x7c00 + BS_FileSysType, 8) ||
      memeq("FAT16   ", (void *)0x7c00 + BS_FileSysType, 8)) {
    if (*(u8 *)(0x7c00 + BS_DrvNum) >= 0x80) {
      default_drive_number = *(u8 *)(0x7c00 + BS_DrvNum) - 0x80 + 0x02;
    } else {
      default_drive_number = *(u8 *)(0x7c00 + BS_DrvNum);
    }
  } else if (memeq("FAT32   ", (void *)0x7c00 + BPB_Fat32ExtByts + BS_FileSysType, 8)) {
    if (*(u8 *)(0x7c00 + BPB_Fat32ExtByts + BS_DrvNum) >= 0x80) {
      default_drive_number = *(u8 *)(0x7c00 + BPB_Fat32ExtByts + BS_DrvNum) - 0x80 + 0x02;
    } else {
      default_drive_number = *(u8 *)(0x7c00 + BPB_Fat32ExtByts + BS_DrvNum);
    }
  } else {
    if (mem_get8(0x7c00) >= 0x80) {
      default_drive_number = mem_get8(0x7c00) - 0x80 + 0x02;
    } else {
      default_drive_number = mem_get8(0x7c00);
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
  char path[15] = " :\\kernel.zst";
  path[0]       = NowTask()->drive;
  klogf("Load file:%s\n", path);
  int sz = vfs_filesize(path);
  if (sz == -1) {
    klogf("DOSLDR can't find kernel.zst in Drive %c", path[0]);
    infinite_loop;
  }
  char *s = page_alloc(sz);
  klogf("Loading...\n");
  vfs_readfile(path, s);
  klogf("Decompressing...\n");
  u32 size = ZSTD_getFrameContentSize(s, sz);
  if (size == ZSTD_CONTENTSIZE_ERROR) {
    klogf("Decompress error\n");
    infinite_loop;
  }
  void *ptr = page_alloc(size);
  if (ptr == null) {
    klogf("Out of memory\n");
    infinite_loop;
  }
  u32 rets = ZSTD_decompress(ptr, size, s, sz);
  if (ZSTD_isError(rets)) {
    klogf("Decompress error\n");
    infinite_loop;
  }
  klogf("Booting...\n");
  u32 entry = load_elf(ptr);

  asm volatile("push %0\n\t"
               "cli\n\t"
               "ret\n\t" ::"r"(entry));
}

struct TASK *NowTask() {
  return &MainTask;
}
