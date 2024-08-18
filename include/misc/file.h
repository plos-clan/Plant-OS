#pragma once
#include <libc-base.h>

extern void *read_from_file(const char *filename, size_t *size);
extern int   write_to_file(const char *filename, const void *data, size_t size);
extern void *map_file(const char *filename, size_t *size, int flags);
extern void  unmap_file(void *ptr, size_t size);
