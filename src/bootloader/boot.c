#include "uefi/uefi.h"

#include "elf.h"
#include "graphic.h"

int main() {
  efi_status_t status = EFI_SUCCESS;
  efi_gop_t   *gop    = NULL;
  status              = graphic_init(&gop);
  status              = draw_logo(gop);
  uintn_t *kernel_entry_t;
  status           = load_kernel("kernel.bin", &kernel_entry_t);
  uintn_t ret_code = ((uintn_t(*)())kernel_entry_t)();

  while (1) {}
  return 0;
}