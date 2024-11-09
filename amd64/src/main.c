#include <efi.h>

#include <elf.h>

uint16_t __attribute__((section(".reloc"))) __relocTable[5] = {0, 0, 0xa, 0, 0};

extern char _image_base;

static void relocate_kernel() {
  uintptr_t   image_base;
  Elf64_Rela *rela;

  __asm__ volatile("leaq _image_base(%%rip), %0" : "=r"(image_base));
  __asm__ volatile("leaq _rela(%%rip), %0" : "=r"(rela));

  for (uint32_t i = 0; ELF64_R_TYPE(rela[i].r_info) != R_X86_64_NONE; ++i) {
    if (ELF64_R_TYPE(rela[i].r_info) != R_X86_64_RELATIVE) continue;

    *(uintptr_t *)(image_base + rela[i].r_offset) += image_base;
  }
}

EFIAPI
efi_status_t _start(efi_handle_t      image_handle __attribute__((unused)),
                    efi_system_table *system_table __attribute__((unused))) {
  relocate_kernel();

  return 0;
}