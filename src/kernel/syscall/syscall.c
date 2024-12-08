#include <kernel.h>

void *syscall_getheap() {
  return (void *)current_task()->alloc_addr;
}

u32 syscall_heapsize() {
  return *(current_task()->alloc_size);
}

static void *syscall_vbe_setmode(int width, int height, int bpp) {
  if (width <= 0 || height <= 0 || bpp <= 0) return null;
  return vbe_match_and_set_mode(width, height, bpp);
}

static int syscall_vbe_flip() {
  return vbe_flip();
}

static int syscall_vbe_flush(const void *buf) {
  return vbe_flush(buf);
}

static int syscall_vbe_clear(byte r, byte g, byte b) {
  return vbe_clear(r, g, b);
}

static ssize_t syscall_file_size(cstr path) {
  vfs_node_t file = vfs_open(path);
  if (file == null) return -1;
  size_t size = file->size;
  vfs_close(file);
  return size;
}

static ssize_t syscall_load_file(cstr path, void *buf, size_t size) {
  vfs_node_t file = vfs_open(path);
  if (file == null) return -1;
  size_t ret = vfs_read(file, buf, 0, size);
  vfs_close(file);
  return ret;
}

static ssize_t syscall_save_file(cstr path, const void *buf, size_t size) {
  vfs_node_t file = vfs_open(path);
  if (file == null) return -1;
  size_t ret = vfs_write(file, buf, 0, size);
  vfs_close(file);
  return ret;
}

void *sycall_handlers[MAX_SYSCALLS] = {
    [SYSCALL_EXIT]    = &syscall_exit,
    [SYSCALL_PUTC]    = &putchar,
    [SYSCALL_FORK]    = &task_fork,
    [SYSCALL_PRINT]   = &print,
    [SYSCALL_GETHEAP] = &syscall_getheap,
    [SYSCALL_HEAPSZ]  = &syscall_heapsize,
    [SYSCALL_MMAP]    = &syscall_mmap,
    [SYSCALL_MUNMAP]  = &syscall_munmap,
    [SYSCALL_READ]    = &syscall_read,
    [SYSCALL_WRITE]   = &syscall_write,
    // [SYSCALL_OPEN] = syscall_open,
    // [SYSCALL_CLOSE] = syscall_close,
    // [SYSCALL_WAITPID] = syscall_waitpid,
    // [SYSCALL_EXECVE] = syscall_execve,
    // [SYSCALL_CHDIR] = syscall_chdir,
    // [SYSCALL_GETCWD] = syscall_getcwd,
    // [SYSCALL_DUP] = syscall_dup,
    // [SYSCALL_PIPE] = syscall_pipe,
    // [SYSCALL_MKDIR] = syscall_mkdir,
    // [SYSCALL_RMDIR] = syscall_rmdir,
    // [SYSCALL_LINK] = syscall_link,
    // [SYSCALL_UNLINK] = syscall_unlink,
    // [SYSCALL_READDIR] = syscall_readdir,
    // [SYSCALL_STAT] = syscall_stat,
    // [SYSCALL_FSTAT] = syscall_fstat,
    // [SYSCALL_CHMOD] = syscall_chmod,
    // [SYSCALL_FCHMOD] = syscall_fchmod,
    // [SYSCALL_CHOWN] = syscall_chown,
    // [SYSCALL_FCHOWN] = syscall_fchown,
    // [SYSCALL_MOUNT] = syscall_mount,
    // [SYSCALL_UMOUNT] = syscall_umount,
    // [SYSCALL_SYMLINK] = syscall_symlink,
    // [SYSCALL_READLINK] = syscall_readlink,
    // [SYSCALL_IOCTL] = syscall_ioctl,
    // [SYSCALL_FCNTL] = syscall_fcntl,
    // [SYSCALL_SOCKET] = syscall_socket,
    // [SYSCALL_BIND] = syscall_bind,
    // [SYSCALL_LISTEN] = syscall_listen,
    // [SYSCALL_ACCEPT] = syscall_accept,
    // [SYSCALL_CONNECT] = syscall_connect,
    // [SYSCALL_SEND] = syscall_send,
    // [SYSCALL_RECV] = syscall_recv,
    // [SYSCALL_SENDTO] = syscall_sendto,
    // [SYSCALL_RECVFROM] = syscall_recvfrom,
    // [SYSCALL_SHUTDOWN] = syscall_shutdown,
    // [SYSCALL_GETSOCKNAME] = syscall_getsockname,
    // [SYSCALL_GETPEERNAME] = syscall_getpeername,
    // [SYSCALL_SETSOCKOPT] = syscall_setsockopt,
    // [SYSCALL_GETSOCKOPT] = syscall_getsockopt,
    [SYSCALL_VBE_SETMODE] = &syscall_vbe_setmode,
    [SYSCALL_VBE_FLIP]    = &syscall_vbe_flip,
    [SYSCALL_VBE_FLUSH]   = &syscall_vbe_flush,
    [SYSCALL_VBE_CLEAR]   = &syscall_vbe_clear,
    [SYSCALL_FILE_SIZE]   = &syscall_file_size,
    [SYSCALL_LOAD_FILE]   = &syscall_load_file,
    [SYSCALL_SAVE_FILE]   = &syscall_save_file,
};

typedef ssize_t (*syscall_t)(ssize_t, ssize_t, ssize_t, ssize_t, ssize_t);

ssize_t syscall(ssize_t eax, ssize_t ebx, ssize_t ecx, ssize_t edx, ssize_t esi, ssize_t edi) {
  klogi("eax: %d, ebx: %d, ecx: %d, edx: %d, esi: %d, edi: %d", eax, ebx, ecx, edx, esi, edi);
  if (0 <= eax && eax < MAX_SYSCALLS && sycall_handlers[eax] != null) {
    eax = ((syscall_t)sycall_handlers[eax])(ebx, ecx, edx, esi, edi);
  } else {
    eax = -1;
  }
  klogi("ret: %d", eax);
  return eax;
}
