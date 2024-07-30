#include <kernel.h>

#define MAX(a, b) a > b ? a : b
bool elf32Validate(Elf32_Ehdr *hdr) {
  return hdr->e_ident[EI_MAG0] == ELFMAG0 && hdr->e_ident[EI_MAG1] == ELFMAG1 &&
         hdr->e_ident[EI_MAG2] == ELFMAG2 && hdr->e_ident[EI_MAG3] == ELFMAG3;
}
u32 elf32_get_max_vaddr(Elf32_Ehdr *hdr) {
  Elf32_Phdr *phdr = (Elf32_Phdr *)((u32)hdr + hdr->e_phoff);
  u32         max  = 0;
  for (int i = 0; i < hdr->e_phnum; i++) {
    u32 size = MAX(phdr->p_filesz,
                   phdr->p_memsz); // 如果memsz大于filesz 说明这是bss段，我们以最大的为准
    max      = MAX(max, phdr->p_vaddr + size);
    phdr++;
  }
  return max;
}
static unsigned div_round_up(unsigned num, unsigned size) {
  return (num + size - 1) / size;
}
void load_segment(Elf32_Phdr *phdr, void *elf) {
  u32 p = div_round_up(phdr->p_memsz, 0x1000);
  int d = phdr->p_paddr;
  if (d & 0x00000fff) {
    unsigned e  = d + phdr->p_memsz;
    d           = d & 0xfffff000;
    e          &= 0xfffff000;
    p           = (e - d) / 0x1000 + 1;
  }
  for (unsigned i = 0; i < p; i++) {
    page_link(d + i * 0x1000);
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

    if (shdr->sh_type != SHT_PROGBITS || !(shdr->sh_flags & SHF_ALLOC)) { continue; }

    for (int i = 0; i < shdr->sh_size; i++) {
      ptr[shdr->sh_addr + i] = p[shdr->sh_offset + i];
    }
  }
}