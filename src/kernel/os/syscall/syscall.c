#include <kernel.h>

void *syscall_getheap() {
  return (void *)current_task()->alloc_addr;
}

u32 syscall_heapsize() {
  return *(current_task()->alloc_size);
}

void *sycall_handlers[MAX_SYSCALLS] = {
    [SYSCALL_EXIT] = syscall_exit,       [SYSCALL_PUTC] = putchar,
    [SYSCALL_FORK] = task_fork,          [SYSCALL_PRINT] = print,
    [SYSCALL_GETHEAP] = syscall_getheap, [SYSCALL_HEAPSZ] = syscall_heapsize,
    [SYSCALL_MMAP] = syscall_mmap,       [SYSCALL_MUNMAP] = syscall_munmap,
    [SYSCALL_READ] = syscall_read,       [SYSCALL_WRITE] = syscall_write,
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
};

typedef ssize_t (*syscall_t)(ssize_t, ssize_t, ssize_t, ssize_t, ssize_t);

ssize_t syscall() {
  volatile ssize_t eax, ebx, ecx, edx, esi, edi;
  asm("mov %%eax, %0\n\t" : "=r"(eax));
  asm("mov %%ebx, %0\n\t" : "=r"(ebx));
  asm("mov %%ecx, %0\n\t" : "=r"(ecx));
  asm("mov %%edx, %0\n\t" : "=r"(edx));
  asm("mov %%esi, %0\n\t" : "=r"(esi));
  asm("mov %%edi, %0\n\t" : "=r"(edi));
  logi("eax: %d, ebx: %d, ecx: %d, edx: %d, esi: %d, edi: %d", eax, ebx, ecx, edx, esi, edi);
  if (0 <= eax && eax < MAX_SYSCALLS && sycall_handlers[eax] != null) {
    eax = ((syscall_t)sycall_handlers[eax])(ebx, ecx, edx, esi, edi);
  } else {
    eax = -1;
  }
  logi("ret: %d", eax);
  return eax;
}

// void asm_inthandler36() {
//   asm volatile("push %ds\n\t");
//   asm volatile("push %es\n\t");
//   asm volatile("push %fs\n\t");
//   asm volatile("push %gs\n\t");
//   asm volatile("push %ebx\n\t");
//   asm volatile("push %ecx\n\t");
//   asm volatile("push %edx\n\t");
//   asm volatile("push %esi\n\t");
//   asm volatile("push %edi\n\t");
//   asm volatile("push %ebp\n\t");
//   asm volatile("push %esp\n\t");
//   asm volatile("call syscall\n\t");
//   asm volatile("pop %esp\n\t");
//   asm volatile("pop %ebp\n\t");
//   asm volatile("pop %edi\n\t");
//   asm volatile("pop %esi\n\t");
//   asm volatile("pop %edx\n\t");
//   asm volatile("pop %ecx\n\t");
//   asm volatile("pop %ebx\n\t");
//   asm volatile("pop %gs\n\t");
//   asm volatile("pop %fs\n\t");
//   asm volatile("pop %es\n\t");
//   asm volatile("pop %ds\n\t");
//   asm volatile("iret\n\t");
// }
