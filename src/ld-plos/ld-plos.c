#define PLOS_NO_SYSCALL_WARP 1
#include <elf.h>
#include <libc-base.h>
#include <sys.h>

#define SYMBOLTABLE_IMPLEMENTATION
#include <data-structure/unordered-map/st.h>

#include "ld-plos.h"

static st_t libs; // path -> data
static st_t syms; // symbol -> address

// 一个在没有标准库的环境下加载动态链接 elf 文件的程序

static int    argc;
static char **argv;
static char **envp;

#include "sysfn.h"

static i32 load_elf(cstr path, bool run);

// 加载完毕后运行应用程序
static __attr(noreturn) void run(usize entry) {
  asm("mov %0, %%edi\n\t" ::"r"(argc));
  asm("mov %0, %%esi\n\t" ::"r"(argv));
  asm("mov %0, %%edx\n\t" ::"r"(envp));
  asm volatile("jmp *%0" ::"r"(entry));
  __builtin_unreachable();
}

static void elf32_init(const Elf *file) {
  if (file->init != null) file->init();
  if (file->init_array != null) {
    for (usize i = 0; i < file->init_array_len; i++) {
      file->init_array[i]();
    }
  }
}

static void elf32_fini(const Elf *file) {
  if (file->fini_array != null) {
    for (isize i = file->fini_array_len - 1; i >= 0; i--) {
      file->fini_array[i]();
    }
  }
  if (file->fini != null) file->fini();
}

static int load_segment_load(const Elf *file, const Elf32ProgramHeader *prog) {
  usize hi   = PADDING_UP(prog->vaddr + prog->memsz, PAGE_SIZE);
  usize lo   = PADDING_DOWN(prog->vaddr, PAGE_SIZE);
  usize addr = syscall(SYSCALL_MMAP, lo, hi - lo);
  if (addr != lo) return -1;
  memcpy((void *)prog->vaddr, file->data + prog->offset, prog->filesz);
  return 0;
}

static int load_segment_dynamic(Elf *file, const Elf32ProgramHeader *prog) {
  val dyn = (const Elf32Dynamic *)prog->offset;
  for (usize i = 0; dyn[i].tag != ELF_DYNAMIC_NULL; i++) {
    val tag   = dyn[i].tag;
    val value = dyn[i].value;
    val ptr   = file->data + dyn[i].value;
    if (tag == ELF_DYNAMIC_NEEDED) load_elf(ptr, false);
    if (tag == ELF_DYNAMIC_HASH) file->hash32 = ptr;
    if (tag == ELF_DYNAMIC_GNU_HASH) file->hash32 = ptr;
    if (tag == ELF_DYNAMIC_STRTAB) file->strtab = ptr;
    if (tag == ELF_DYNAMIC_SYMTAB) file->sym32 = ptr;
    if (tag == ELF_DYNAMIC_INIT) file->init = ptr;
    if (tag == ELF_DYNAMIC_FINI) file->fini = ptr;
    if (tag == ELF_DYNAMIC_INIT_ARRAY) file->init_array = ptr;
    if (tag == ELF_DYNAMIC_INIT_ARRAYSZ) file->init_array_len = value / 4;
    if (tag == ELF_DYNAMIC_FINI_ARRAY) file->fini_array = ptr;
    if (tag == ELF_DYNAMIC_FINI_ARRAYSZ) file->fini_array_len = value / 4;
  }
  return 0;
}

static int load_segment(Elf *file, const Elf32ProgramHeader *prog) {
  if (prog->type == ELF_PROGRAM_TYPE_NULL) return 0;
  if (prog->type == ELF_PROGRAM_TYPE_LOAD) return load_segment_load(file, prog);
  if (prog->type == ELF_PROGRAM_TYPE_DYNAMIC) return load_segment_dynamic(file, prog);
  if (prog->type == ELF_PROGRAM_TYPE_NOTE) return 0;
  if (prog->type == ELF_PROGRAM_TYPE_TLS) return load_segment_load(file, prog);
  if (prog->type == ELF_PROGRAM_TYPE_PHDR) return 0;
  if (prog->type == ELF_PROGRAM_TYPE_GNU_STACK) return 0;
  return -1;
}

static int load_elf32(Elf *file, const Elf32Header *header) {
  var prog = (const Elf32ProgramHeader *)((usize)header + header->phoff);
  for (usize i = 0; i < header->phnum; i++) {
    if (load_segment(file, prog + i) < 0) return LDE_FILE_UNPARSABLE;
  }
  elf32_init(file);
  val entry = header->entry;
  syscall(SYSCALL_MUNMAP, file->data, file->size);
  syscall(SYSCALL_MUNMAP, file, sizeof(Elf));
  run(entry);
}

#ifdef __x86_64__

static int load_elf64(Elf *elf, const Elf64Header *header) {
  syscall(SYSCALL_MUNMAP, elf, elf->size);
  return 0;
}

#endif

static usize elf_file_map_addr = 0xb0000000;

static i32 load_elf(cstr path, bool run) {
  if (st_has(libs, path)) return 0;

  const isize size = syscall(SYSCALL_FILE_SIZE, path);
  if (size < 0) return LDE_FILE_NOT_FOUND;

  Elf *file = (void *)syscall(SYSCALL_MMAP, elf_file_map_addr, sizeof(Elf));
  if (file == null) return LDE_OUT_OF_MEMORY;
  elf_file_map_addr += PADDING_UP(sizeof(Elf), PAGE_SIZE);

  file->data = (void *)syscall(SYSCALL_MMAP, elf_file_map_addr, size);
  if (file->data == null) return LDE_OUT_OF_MEMORY;
  elf_file_map_addr += PADDING_UP(size, PAGE_SIZE);

  if (syscall(SYSCALL_LOAD_FILE, path, file->data, size) < 0) return LDE_FILE_UNREADABLE;

  if (st_insert(libs, path, file) < 0) return LDE_OUT_OF_MEMORY;

  val ident = file->ident;
  if (ident->magic != ELF_MAGIC) return LDE_FILE_UNPARSABLE;

  if (ident->class == ELF_CLASS_32) {
    val header = file->header32;
    if (header->machine != ELF_MACHINE_IA32) return LDE_ARCH_MISSMATCH;
    return load_elf32(file, header);
  }
  if (ident->class == ELF_CLASS_64) {
#ifdef __x86_64__
    val header = file->header64;
    if (header->machine != ELF_MACHINE_AMD64) return LDE_ARCH_MISSMATCH;
    return load_elf64(file, header);
#else
    // Can't load 64-bit ELF in 32-bit system.
    syscall(SYSCALL_MUNMAP, file->data, file->size);
    syscall(SYSCALL_MUNMAP, file, sizeof(Elf));
    return LDE_ARCH_MISSMATCH;
#endif
  }
  return LDE_FILE_UNPARSABLE;
}

static i32 __linker_main(int argc, char **argv, char **envp) {
  if (argc <= 0 || argv == null || envp == null || argv[0] == null) return LDE_INVALID_INPUT;
  return load_elf(argv[0], true);
}

// 链接器入口点
void __linker_start() {
  asm("mov %%edi, %0\n\t" : "=r"(argc));
  asm("mov %%esi, %0\n\t" : "=r"(argv));
  asm("mov %%edx, %0\n\t" : "=r"(envp));

  val errcode = __linker_main(argc, argv, envp);

  syscall(SYSCALL_EXIT, MASK(30) | MASK(29) | (errcode << 16));
}

// 应用程序异常处理
void __linker_handler() {
  //
}
