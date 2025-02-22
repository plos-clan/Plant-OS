#include <kernel.h>

//* ----------------------------------------------------------------------------------------------------
//; 内存权限检查
//* ----------------------------------------------------------------------------------------------------

//+ 将此功能与页表管理分离以便修改

bool check_address_permission3(const void *addr, bool wr, usize cr3) {
  val pde = pdeof(addr, cr3);
  if (!pde->present) return false;
  if (wr && !pde->wrable) return false;
  if (!pde->user) return false;
  val pte = pteof(addr, paddr(pde));
  if (!pte->present) return false;
  if (wr && !pte->wrable) return false;
  if (!pte->user) return false;
  return true;
}

bool check_address_permission2(const void *addr, bool wr) {
  return check_address_permission3(addr, wr, asm_get_cr3());
}

bool check_address_permission1(const void *addr) {
  return check_address_permission3(addr, false, asm_get_cr3());
}

// --------------------------------------------------

bool check_memory_permission4(const void *_addr, size_t size, bool wr, usize cr3) {
  const void *addr     = PADDING_DOWN(_addr, PAGE_SIZE);
  const void *end_addr = _addr + size;
  for (; addr < end_addr; addr += PAGE_SIZE) {
    if (!check_address_permission(addr, wr, cr3)) return false;
  }
  return true;
}

bool check_memory_permission3(const void *_addr, size_t size, bool wr) {
  return check_memory_permission4(_addr, size, wr, asm_get_cr3());
}

bool check_memory_permission2(const void *_addr, size_t size) {
  return check_memory_permission4(_addr, size, false, asm_get_cr3());
}

// --------------------------------------------------

bool check_string_permission2(cstr addr, usize cr3) {
  if (!check_address_permission(addr, false)) return false;
  for (addr++;; addr++) {
    if (!((size_t)addr & 0xfff) && !check_address_permission(addr, false, cr3)) return false;
    if (*addr == '\0') break;
  }
  return true;
}

bool check_string_permission1(cstr addr) {
  return check_string_permission2(addr, asm_get_cr3());
}

// --------------------------------------------------

bool check_string_array_permission2(cstr *addr, usize cr3) {
  for (;; addr++) {
    if (!check_address_permission(addr, false, cr3)) return false;
    if (!check_address_permission((void *)((usize)(addr + 1) - 1), false, cr3)) return false;
    if (!*addr) break;
    if (!check_string_permission(*addr, cr3)) return false;
  }
  return true;
}

bool check_string_array_permission1(cstr *addr) {
  return check_string_array_permission2(addr, asm_get_cr3());
}
