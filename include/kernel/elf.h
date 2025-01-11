#pragma once
#include "../elf.h"

bool  elf32_is_validate(const Elf32Header *hdr);
usize load_elf(const Elf32Header *elf);
