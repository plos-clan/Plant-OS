#pragma once
#include <fcntl.h>
#include <stddef.h>

#ifdef ALL_IMPLEMENTATION
#  define FILE_IMPLEMENTATION
#endif

#ifdef FILE_IMPLEMENTATION
#  define extern static
#endif

extern void *read_from_file(const char *filename, size_t *size);
extern int   write_to_file(const char *filename, const void *data, size_t size);
extern void *map_file(const char *filename, size_t *size, int flags);
extern void  unmap_file(void *ptr, size_t size);

#ifdef FILE_IMPLEMENTATION
#  undef extern
#endif

#ifdef FILE_IMPLEMENTATION

#  include <stdint.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>

static void *read_from_file(const char *filename, size_t *size) {
  if (filename == NULL) return NULL;

  int fd = open(filename, O_RDONLY);
  if (fd < 0) return NULL;

  struct stat file_stat;
  if (fstat(fd, &file_stat) < 0) {
    close(fd);
    return NULL;
  }
  if (size) *size = file_stat.st_size;

  void *data = malloc(file_stat.st_size + 1);
  if (data == NULL) return NULL;

  ((uint8_t *)data)[file_stat.st_size] = 0;

  int rets = read(fd, data, file_stat.st_size);
  close(fd);
  if (rets != file_stat.st_size) {
    free(data);
    return NULL;
  }

  return data;
}

static int write_to_file(const char *filename, const void *data, size_t size) {
  if (filename == NULL || data == NULL) return -1;

  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) return -1;

  int nwrite = write(fd, data, size);
  if (nwrite != size) {
    close(fd);
    remove(filename);
    return -1;
  }

  close(fd);
  return 0;
}

static void *map_file(const char *filename, size_t *size, int flags) {
  if (filename == NULL) return NULL;

  int prot;
  switch (flags) {
  case O_RDONLY: prot = PROT_READ; break;
  case O_WRONLY: prot = PROT_WRITE; break;
  case O_RDWR: prot = PROT_READ | PROT_WRITE; break;
  default: return NULL;
  }

  int fd = open(filename, flags);
  if (fd < 0) return NULL;

  struct stat file_stat;
  if (fstat(fd, &file_stat) < 0) {
    close(fd);
    return NULL;
  }
  if (size) *size = file_stat.st_size;

  void *mapped_data = mmap(NULL, file_stat.st_size + 1, prot, MAP_SHARED, fd, 0);
  close(fd);
  if (mapped_data == MAP_FAILED) return NULL;

  return mapped_data;
}

static void unmap_file(void *ptr, size_t size) {
  munmap(ptr, size + 1);
}

#  undef FILE_IMPLEMENTATION
#endif
