#include <misc.h>

#if NO_STD

// TODO 使用正式系统调用替换这里的临时实现
dlexport void *read_from_file(const char *filename, size_t *size) {
  if (filename == NULL) return NULL;

  ssize_t file_size = __syscall(SYSCALL_FILE_SIZE, filename);
  if (file_size < 0) return NULL;

  if (size) *size = file_size;

  void *data = malloc(file_size + 1);
  if (data == NULL) return NULL;

  ((uint8_t *)data)[file_size] = 0;

  ssize_t read_size = __syscall(SYSCALL_LOAD_FILE, filename, data, file_size);
  if (read_size != file_size) {
    free(data);
    return NULL;
  }

  return data;
}

// TODO 使用正式系统调用替换这里的临时实现
dlexport int write_to_file(const char *filename, const void *data, size_t size) {
  if (filename == NULL || data == NULL) return -1;

  ssize_t nwrite = __syscall(SYSCALL_SAVE_FILE, filename, data, size);
  if (nwrite != size) return -1;

  return 0;
}

dlexport void *map_file(const char *filename, size_t *size, int flags) {
  TODO("实现");
}

dlexport void unmap_file(void *ptr, size_t size) {
  TODO("实现");
}

#else

#  include <fcntl.h>
#  include <stdio.h>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>

dlexport void *read_from_file(const char *filename, size_t *size) {
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

dlexport int write_to_file(const char *filename, const void *data, size_t size) {
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

dlexport void *map_file(const char *filename, size_t *size, int flags) {
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

dlexport void unmap_file(void *ptr, size_t size) {
  munmap(ptr, size + 1);
}

#endif
