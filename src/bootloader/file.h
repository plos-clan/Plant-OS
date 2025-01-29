#ifndef _FILE_H_
#define _FILE_H_

#include "uefi/uefi.h"

efi_status_t read_file(char *filename, void **file_buffer, uintn_t *file_size);

#endif