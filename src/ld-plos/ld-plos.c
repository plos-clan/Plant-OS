#include <elf.h>
#include <libc-base.h>
#include <sys.h>

#include "ld-plos.h"

// 一个在没有标准库的环境下加载动态链接 elf 文件的程序

static int    argc;
static char **argv;
static char **envp;

// 加载完毕后运行应用程序
static __attr(noreturn) void run(usize entry) {
  asm("mov %0, %%edi\n\t" ::"r"(argc));
  asm("mov %0, %%esi\n\t" ::"r"(argv));
  asm("mov %0, %%edx\n\t" ::"r"(envp));
  asm volatile("jmp *%0" ::"r"(entry));
  __builtin_unreachable();
}

#ifdef __x86_64__

static int load_elf32(const Elf32Header *elf) {
  return 0;
}

static int load_elf64(const Elf64Header *elf) {
  return 0;
}

#else

static int load_segment(const Elf32ProgramHeader *prog, const void *elf) {
  if (prog->type != ELF_PROGRAM_TYPE_LOAD) return 0;
  usize hi   = PADDING_UP(prog->vaddr + prog->memsz, PAGE_SIZE);
  usize lo   = PADDING_DOWN(prog->vaddr, PAGE_SIZE);
  usize addr = syscall(SYSCALL_MMAP, lo, hi - lo);
  if (addr != lo) syscall(SYSCALL_EXIT, 666);
  memcpy((void *)prog->vaddr, elf + prog->offset, prog->filesz);
  return 0;
}

static int load_elf32(const Elf32Header *elf) {
  var prog = (const Elf32ProgramHeader *)((usize)elf + elf->phoff);
  for (usize i = 0; i < elf->phnum; i++) {
    if (load_segment(prog + i, elf) < 0) return LDE_FILE_UNPARSABLE;
  }
  run(elf->entry);
}

static int load_elf64(const void *elf) {
  // Can't load 64-bit ELF in 32-bit system.
  return LDE_ARCH_MISSMATCH;
}

#endif

static int __linker_main(int argc, char **argv, char **envp) {
  if (argc <= 0 || argv == null || envp == null || argv[0] == null) return LDE_INVALID_INPUT;

  isize size = syscall(SYSCALL_FILE_SIZE, argv[0]);
  if (size < 0) return LDE_FILE_NOT_FOUND;
  void *file = (void *)syscall(SYSCALL_MMAP, null, size);
  if (file == null) return LDE_OUT_OF_MEMORY;
  if (syscall(SYSCALL_LOAD_FILE, argv[0], file, size) < 0) return LDE_FILE_UNREADABLE;

  const Elf32Header *elf   = (Elf32Header *)file;
  const ElfIdent    *ident = (ElfIdent *)file;
  if (ident->magic != ELF_MAGIC) return 1;

  if (ident->class == ELF_CLASS_32) {
    if (elf->machine != ELF_MACHINE_IA32) return LDE_ARCH_MISSMATCH;
    return load_elf32(file);
  }
  if (ident->class == ELF_CLASS_64) {
    if (elf->machine != ELF_MACHINE_AMD64) return LDE_ARCH_MISSMATCH;
    return load_elf64(file);
  }
  return LDE_FILE_UNPARSABLE;
}

void __linker_start() {
  asm("mov %%edi, %0\n\t" : "=r"(argc));
  asm("mov %%esi, %0\n\t" : "=r"(argv));
  asm("mov %%edx, %0\n\t" : "=r"(envp));

  int errcode = __linker_main(argc, argv, envp);

  syscall(SYSCALL_EXIT, MASK(30) | MASK(29) | (errcode << 16));
}
