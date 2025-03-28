#include <kernel.h>

static void *syscall_getheap() {
  return (void *)ADDR_TASK_HEAP;
}

static u32 syscall_heapsize() {
  return current_task->heapsize;
}

static int syscall_vbe_getmode(void **vram, int *width, int *height) {
  if (!check_memory_permission(vram, sizeof(void *), true)) task_abort();
  if (!check_memory_permission(width, sizeof(int), true)) task_abort();
  if (!check_memory_permission(height, sizeof(int), true)) task_abort();
  *vram   = vbe_frontbuffer;
  *width  = screen_w;
  *height = screen_h;
  for (usize i = 0; i < screen_w * screen_h * 4; i += PAGE_SIZE) {
    page_link_addr_pde((usize)vbe_frontbuffer + i, current_task->cr3, (usize)vbe_frontbuffer + i);
  }
  return 0;
}

static void *syscall_vbe_setmode(int width, int height, int bpp) {
  if (width <= 0 || height <= 0 || bpp <= 0) return null;
  return vbe_match_and_set_mode(width, height, bpp);
}

static int syscall_vbe_flip() {
  return vbe_flip();
}

static int syscall_vbe_flush(const void *buf) {
  if (!check_memory_permission(buf, screen_w * screen_h * 4)) task_abort();
  return vbe_flush(buf);
}

static int syscall_vbe_clear(byte r, byte g, byte b) {
  return vbe_clear(r, g, b);
}

static isize syscall_file_size(cstr path) {
  if (!check_string_permission(path)) task_abort();
  vfs_node_t file = vfs_open(path);
  if (file == null) return -1;
  val size = file->size;
  vfs_close(file);
  return size;
}

static isize syscall_load_file(cstr path, void *buf, usize size) {
  if (!check_string_permission(path)) task_abort();
  if (!check_memory_permission(buf, size, true)) task_abort();
  val file = vfs_open(path);
  if (file == null) return -1;
  val ret = vfs_read(file, buf, 0, size);
  vfs_close(file);
  return ret;
}

static isize syscall_save_file(cstr path, const void *buf, usize size) {
  if (!check_string_permission(path)) task_abort();
  if (!check_memory_permission(buf, size)) task_abort();
  val file = vfs_open(path);
  if (file == null) return -1;
  val ret = vfs_write(file, buf, 0, size);
  vfs_close(file);
  return ret;
}

void syscall_print(cstr s) {
  if (!check_string_permission(s)) task_abort();
  print(s);
}

static void *const sycall_handlers[MAX_SYSCALLS] = {
    [SYSCALL_EXIT]    = &syscall_exit,
    [SYSCALL_PUTC]    = &putchar,
    [SYSCALL_FORK]    = &task_fork,
    [SYSCALL_PRINT]   = &syscall_print,
    [SYSCALL_GETHEAP] = &syscall_getheap,
    [SYSCALL_HEAPSZ]  = &syscall_heapsize,
    [SYSCALL_MMAP]    = &syscall_mmap,
    [SYSCALL_MUNMAP]  = &syscall_munmap,
    [SYSCALL_READ]    = &syscall_read,
    [SYSCALL_WRITE]   = &syscall_write,
    [SYSCALL_OPEN]    = &syscall_open,
    [SYSCALL_CLOSE]   = &syscall_close,
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
    //% 临时 syscall
    [SYSCALL_VBE_SETMODE] = &syscall_vbe_setmode,
    [SYSCALL_VBE_FLIP]    = &syscall_vbe_flip,
    [SYSCALL_VBE_FLUSH]   = &syscall_vbe_flush,
    [SYSCALL_VBE_CLEAR]   = &syscall_vbe_clear,
    [SYSCALL_VBE_GETMODE] = &syscall_vbe_getmode,
    [SYSCALL_FILE_SIZE]   = &syscall_file_size,
    [SYSCALL_LOAD_FILE]   = &syscall_load_file,
    [SYSCALL_SAVE_FILE]   = &syscall_save_file,
};

isize syscall(isize eax, isize ebx, isize ecx, isize edx, isize esi, isize edi) {
  klogi("eax: %d, ebx: %d, ecx: %d, edx: %d, esi: %d, edi: %d", eax, ebx, ecx, edx, esi, edi);
  if (0 <= eax && eax < MAX_SYSCALLS && sycall_handlers[eax] != null) {
    eax = ((syscall_t)sycall_handlers[eax])(ebx, ecx, edx, esi, edi);
  } else {
    eax = -1;
  }
  klogi("ret: %d", eax);
  return eax;
}
