#pragma once
#include "../elf.h"

bool  elf32_is_validate(const Elf32Header *hdr);
void  elf32_load_data(const Elf32Header *elf, byte *ptr);
u32   elf32_get_max_vaddr(const Elf32Header *hdr);
usize load_elf(const Elf32Header *elf);
