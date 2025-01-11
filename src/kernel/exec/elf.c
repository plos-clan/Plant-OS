#include <kernel.h>

bool elf32_is_validate(const Elf32Header *hdr) {
  return hdr->ident.magic == ELF_MAGIC;
}

void print_section_names(const Elf32Header *elf) {
  val shdr   = (const Elf32SectionHeader *)((usize)elf + elf->shoff);
  val strtab = (cstr)elf + shdr[elf->shstrndx].offset;
  klogd("Section Headers:");
  for (int i = 0; i < elf->shnum; i++) {
    klogd("  [%2d] %s", i, strtab + shdr[i].name);
  }
}

void load_segment(const Elf32ProgramHeader *prog, const void *elf) {
  if (prog->type != ELF_PROGRAM_TYPE_LOAD) return;
  if (prog->memsz < prog->filesz) return;
  val hi = PADDING_UP(prog->vaddr + prog->memsz, PAGE_SIZE);
  val lo = PADDING_DOWN(prog->vaddr, PAGE_SIZE);
  for (size_t i = lo; i < hi; i += PAGE_SIZE) {
    page_link(i);
  }
  memcpy((void *)prog->vaddr, elf + prog->offset, prog->filesz);
  memset((void *)prog->vaddr + prog->filesz, 0, prog->memsz - prog->filesz);
}

usize load_elf(const Elf32Header *elf) {
  var prog = (const Elf32ProgramHeader *)((usize)elf + elf->phoff);
  for (usize i = 0; i < elf->phnum; i++) {
    load_segment(prog + i, elf);
  }
  print_section_names(elf);
  return elf->entry;
}
