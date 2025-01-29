#include "elf.h"
#include "file.h"
#include "uefi/uefi.h"

efi_status_t load_kernel(char_t *filename, uintn_t **kernel_entry) {
  efi_status_t status;
  void        *kernel_file_base = NULL;
  uintn_t      filesize;
  status = read_file(filename, &kernel_file_base, &filesize);

  ELF_HEADER_64     *kernel_file_header = kernel_file_base;
  PROGRAM_HEADER_64 *kernel_file_program_header =
      (void *)kernel_file_header + sizeof(ELF_HEADER_64);
  uintn_t low_addr  = 0xffffffffffffffff;
  uintn_t high_addr = 0;

  for (int i = 0; i < kernel_file_header->PHeadCount; i++) {
    if (kernel_file_program_header[i].Type == PT_LOAD) {
      if (kernel_file_program_header[i].VAddress < low_addr) {
        low_addr = kernel_file_program_header[i].VAddress;
      }
      if (kernel_file_program_header[i].VAddress + kernel_file_program_header[i].SizeInMemory >
          high_addr) {
        high_addr =
            kernel_file_program_header[i].VAddress + kernel_file_program_header[i].SizeInMemory;
      }
    }
  }

  uintn_t kernel_size       = high_addr - low_addr;
  uintn_t kernel_page_count = (kernel_size + 4095) / 4096;
  void   *kernel_base;
  status = BS->AllocatePages(AllocateAnyPages, EfiLoaderCode, kernel_page_count,
                             (efi_physical_address_t *)&kernel_base);
  for (int i = 0; i < kernel_file_header->PHeadCount; i++) {
    if (kernel_file_program_header[i].Type == PT_LOAD) {
      memcpy((void *)(kernel_base + kernel_file_program_header[i].VAddress - low_addr),
             (void *)(kernel_file_base + kernel_file_program_header[i].Offset),
             kernel_file_program_header[i].SizeInMemory);
    }
  }
  *kernel_entry = (uint64_t *)(kernel_base + kernel_file_header->Entry - low_addr);

  return EFI_SUCCESS;
}