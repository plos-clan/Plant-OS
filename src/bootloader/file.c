#include "file.h"

#include "uefi/uefi.h"

efi_status_t read_file(char *filename, void **file_buffer, uintn_t *file_size) {
  FILE *fp;
  fp = fopen(filename, "r");
  struct stat filestate;
  fstat(fp, &filestate);
  *file_size   = filestate.st_size;
  *file_buffer = malloc(*file_size);
  fread(*file_buffer, 1, *file_size, fp);
  return EFI_SUCCESS;
}