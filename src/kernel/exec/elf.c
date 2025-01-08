#include <kernel.h>

bool elf32_is_validate(const Elf32Header *hdr) {
  return hdr->ident.magic == ELF_MAGIC;
}

void print_section_names(const Elf32Header *hdr) {
  val shdr   = (const Elf32SectionHeader *)((usize)hdr + hdr->shoff);
  val strtab = (cstr)hdr + shdr[hdr->shstrndx].offset;
  klogd("Section Headers:");
  for (int i = 0; i < hdr->shnum; i++) {
    klogd("  [%2d] %s", i, strtab + shdr[i].name);
  }
}

u32 elf32_get_max_vaddr(const Elf32Header *hdr) {
  var   phdr = (const Elf32ProgramHeader *)((usize)hdr + hdr->phoff);
  usize max  = 0;
  for (usize i = 0; i < hdr->phnum; i++) {
    // 如果memsz大于filesz 说明这是bss段，我们以最大的为准
    usize size = max(phdr[i].filesz, phdr[i].memsz);
    max        = max(max, phdr[i].vaddr + size);
  }
  return max;
}

void load_segment(const Elf32ProgramHeader *prog, const void *elf) {
  if (prog->type != ELF_PROGRAM_TYPE_LOAD) return;
  size_t hi = PADDING_UP(prog->paddr + prog->memsz, PAGE_SIZE);
  size_t lo = PADDING_DOWN(prog->paddr, PAGE_SIZE);
  for (size_t i = lo; i < hi; i += PAGE_SIZE) {
    page_link(i);
  }
  memcpy((void *)prog->vaddr, elf + prog->offset, prog->filesz);
  if (prog->memsz > prog->filesz) { // 这个是bss段
    memset((void *)(prog->vaddr + prog->filesz), 0, prog->memsz - prog->filesz);
  }
}

usize load_elf(const Elf32Header *elf) {
  var prog = (const Elf32ProgramHeader *)((usize)elf + elf->phoff);
  for (usize i = 0; i < elf->phnum; i++) {
    load_segment(prog + i, elf);
  }
  print_section_names(elf);
  return elf->entry;
}

void elf32_load_data(const Elf32Header *elf, byte *ptr) {
  val p = (const byte *)elf;
  for (usize i = 0; i < elf->shnum; i++) {
    val section = (const Elf32SectionHeader *)(p + elf->shoff) + i;

    if (section->type != SHT_PROGBITS || !(section->flags & SHF_ALLOC)) continue;

    memcpy(ptr + section->addr, p + section->offset, section->size);
  }

  print_section_names(elf);
}
