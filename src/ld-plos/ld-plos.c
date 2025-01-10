#include <elf.h>
#include <libc-base.h>
#include <sys.h>

// 一个在没有标准库的环境下加载动态链接 elf 文件的程序

#define syscall(...) __syscall(__VA_ARGS__)

#ifdef __x86_64__

static void load_elf32(const Elf32Header *elf) {}
static void load_elf64(const Elf64Header *elf) {}

#else

void print_num(usize num) {
  static char buffer[32];
  char       *buf = buffer + 32;
  *--buf          = '\0';
  *--buf          = '\n';
  while (num) {
    int digit  = num % 16;
    *--buf     = (digit < 10) ? digit + '0' : digit - 10 + 'a';
    num       /= 16;
  }
  syscall(SYSCALL_PRINT, buf);
}

int load_segment(const Elf32ProgramHeader *prog, const void *elf) {
  print_num(prog->type);
  print_num(prog->offset);
  print_num(prog->vaddr);
  print_num(prog->filesz);
  print_num(prog->memsz);
  print_num(prog->flags);
  if (prog->type != ELF_PROGRAM_TYPE_LOAD) return 0;
  usize hi   = PADDING_UP(prog->vaddr + prog->memsz, PAGE_SIZE);
  usize lo   = PADDING_DOWN(prog->vaddr, PAGE_SIZE);
  usize addr = syscall(SYSCALL_MMAP, lo, hi - lo);
  if (addr != lo) syscall(SYSCALL_EXIT, 666);
  memcpy((void *)prog->vaddr, elf + prog->offset, prog->filesz);
  return 0;
}

static void load_elf32(const Elf32Header *elf) {
  var prog = (const Elf32ProgramHeader *)((usize)elf + elf->phoff);
  for (usize i = 0; i < elf->phnum; i++) {
    if (load_segment(prog + i, elf) < 0) return;
  }
  asm volatile("jmp *%0" : : "r"(elf->entry));
}
static void load_elf64(const void *elf) {
  // Can't load 64-bit ELF in 32-bit system.
}

#endif

void __linker_main(int argc, char **argv, char **envp) {
  if (argc <= 0 || argv == null || envp == null) return;
  if (argv[0] == null) return;

  usize size = syscall(SYSCALL_FILE_SIZE, argv[0]);
  void *file = (void *)syscall(SYSCALL_MMAP, null, size);
  syscall(SYSCALL_LOAD_FILE, argv[0], file, size);

  const ElfIdent *ident = (ElfIdent *)file;
  if (ident->magic != ELF_MAGIC) return;
  if (ident->class == ELF_CLASS_32) {
    load_elf32(file);
  } else if (ident->class == ELF_CLASS_64) {
    load_elf64(file);
  }
}

void __linker_start() {
  int volatile argc;
  char **volatile argv;
  char **volatile envp;
  asm("mov %%edi, %0\n\t" : "=r"(argc));
  asm("mov %%esi, %0\n\t" : "=r"(argv));
  asm("mov %%edx, %0\n\t" : "=r"(envp));

  __linker_main(argc, argv, envp);

  syscall(SYSCALL_EXIT, INT_MAX);
}
