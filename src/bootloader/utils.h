#ifndef _UTILS_H_
#define _UTILS_H_

#include "uefi/uefi.h"

#define CHECKSTATUS(status)                                                                        \
  ({                                                                                               \
    if (EFIERR(status)) { return status; }                                                         \
  })

void dump_memory(const void *mem, uintn_t size);
// uintn_t get_physical_memory_size();

#endif