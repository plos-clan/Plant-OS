#include <kernel.h>

bool elf32Validate(Elf32_Ehdr *hdr) {
  return *(u32 *)hdr->e_ident == ELF_MAGIC;
}

u32 elf32_get_max_vaddr(Elf32_Ehdr *hdr) {
  Elf32_Phdr *phdr = (Elf32_Phdr *)((u32)hdr + hdr->e_phoff);
  u32         max  = 0;
  for (int i = 0; i < hdr->e_phnum; i++) {
    // 如果memsz大于filesz 说明这是bss段，我们以最大的为准
    u32 size = max(phdr->p_filesz, phdr->p_memsz);
    max      = max(max, phdr->p_vaddr + size);
    phdr++;
  }
  return max;
}

void load_segment(Elf32_Phdr *phdr, void *elf) {
  // int d = phdr->p_paddr;
  // if (d & 0x00000fff) {
  //   u32 e  = d + phdr->p_memsz;
  //   d      = d & 0xfffff000;
  //   e     &= 0xfffff000;
  //   p      = (e - d) / 0x1000 + 1;
  // }
  size_t hi = PADDING_UP(phdr->p_paddr + phdr->p_memsz, PAGE_SIZE);
  size_t lo = PADDING_DOWN(phdr->p_paddr, PAGE_SIZE);
  for (size_t i = lo; i < hi; i += PAGE_SIZE) {
    page_link(i);
  }
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

void elf32LoadData(Elf32_Ehdr *elfhdr, u8 *ptr) {
  u8 *p = (u8 *)elfhdr;
  for (int i = 0; i < elfhdr->e_shnum; i++) {
    Elf32_Shdr *shdr = (Elf32_Shdr *)(p + elfhdr->e_shoff + sizeof(Elf32_Shdr) * i);

    if (shdr->sh_type != SHT_PROGBITS || !(shdr->sh_flags & SHF_ALLOC)) continue;

    for (int i = 0; i < shdr->sh_size; i++) {
      ptr[shdr->sh_addr + i] = p[shdr->sh_offset + i];
    }
  }
}
