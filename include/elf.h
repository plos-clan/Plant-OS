#pragma once
#include <define.h>

// http://www.sco.com/developers/devspecs/gabi41.pdf
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

// --------------------------------------------------
//; Ident

typedef struct ElfIdent {
  u32 magic; // == ELF_MAGIC
  u8 class;
  u8 data;
  u8 version;
  u8 osabi;
  u8 abiversion;
  u8 pad[7];
} ElfIdent;

#define ELF_MAGIC MAGIC32(0x7f, 'E', 'L', 'F')

#define ELF_CLASS_32 1 // 32-bit objects
#define ELF_CLASS_64 2 // 64-bit objects

#define ELF_DATA_LE 1 // Little-endian
#define ELF_DATA_BE 2 // Big-endian

#define ELF_VERSION 1

#define ELF_OSABI_SYSV     0
#define ELF_OSABI_HPUX     1
#define ELF_OSABI_NETBSD   2
#define ELF_OSABI_LINUX    3
#define ELF_OSABI_GNUHURD  4
#define ELF_OSABI_SOLARIS  6
#define ELF_OSABI_AIX      7
#define ELF_OSABI_IRIX     8
#define ELF_OSABI_FREEBSD  9
#define ELF_OSABI_TRU64    10
#define ELF_OSABI_MODESTO  11
#define ELF_OSABI_OPENBSD  12
#define ELF_OSABI_OPENVMS  13
#define ELF_OSABI_NSK      14
#define ELF_OSABI_AROS     15
#define ELF_OSABI_FENIXOS  16
#define ELF_OSABI_CLOUDABI 17
#define ELF_OSABI_OPENVOS  18

// --------------------------------------------------
//; Header

typedef struct Elf32Header {
  ElfIdent ident;
  u16      type;
  u16      machine;
  u32      version;
  u32      entry;
  u32      phoff;
  u32      shoff;
  u32      flags;
  u16      ehsize;
  u16      phentsize;
  u16      phnum;
  u16      shentsize;
  u16      shnum;
  u16      shstrndx;
} Elf32Header;

#ifdef __x86_64__
typedef struct Elf64Header {
  ElfIdent ident;
  u16      type;
  u16      machine;
  u32      version;
  u64      entry;
  u64      phoff;
  u64      shoff;
  u32      flags;
  u16      ehsize;
  u16      phentsize;
  u16      phnum;
  u16      shentsize;
  u16      shnum;
  u16      shstrndx;
} Elf64Header;
#endif

#define ELF_TYPE_NONE   0      // Unknown.
#define ELF_TYPE_ELF    1      // Relocatable file.
#define ELF_TYPE_EXEC   2      // Executable file.
#define ELF_TYPE_DYN    3      // Shared object.
#define ELF_TYPE_CORE   4      // Core file.
#define ELF_TYPE_LOOS   0xfe00 // Reserved inclusive range. Operating system specific.
#define ELF_TYPE_HIOS   0xfeff // Reserved inclusive range. Operating system specific.
#define ELF_TYPE_LOPROC 0xff00 // Reserved inclusive range. Processor specific.
#define ELF_TYPE_HIPROC 0xffff // Reserved inclusive range. Processor specific.

#define ELF_MACHINE_IA32    3   // 0003: Intel 80386
#define ELF_MACHINE_ARM     40  // 0028: ARM
#define ELF_MACHINE_IA64    50  // 0032: Intel Itanium architecture
#define ELF_MACHINE_AMD64   62  // 003e: AMD x86-64 architecture
#define ELF_MACHINE_AARCH64 183 // 00b7: ARM AARCH64
#define ELF_MACHINE_RISCV   243 // 00f3: RISC-V

// --------------------------------------------------
//; Section Header

// section type
#define ELF_SECTION_TYPE_NULL     0
#define ELF_SECTION_TYPE_PROGBITS 1
#define ELF_SECTION_TYPE_SYMTAB   2
#define ELF_SECTION_TYPE_STRTAB   3
#define ELF_SECTION_TYPE_RELA     4
#define ELF_SECTION_TYPE_HASH     5
#define ELF_SECTION_TYPE_DYNAMIC  6
#define ELF_SECTION_TYPE_NOTE     7
#define ELF_SECTION_TYPE_NOBITS   8
#define ELF_SECTION_TYPE_REL      9
#define ELF_SECTION_TYPE_SHLIB    10
#define ELF_SECTION_TYPE_DYNSYM   11
#define ELF_SECTION_TYPE_LOPROC   0x70000000
#define ELF_SECTION_TYPE_HIPROP   0x7fffffff
#define ELF_SECTION_TYPE_LOUSER   0x80000000
#define ELF_SECTION_TYPE_HIUSER   0xffffffff

// section attribute flags
#define ELF_SECTION_FLAG_WRITE     0x01
#define ELF_SECTION_FLAG_ALLOC     0x02
#define ELF_SECTION_FLAG_EXECINSTR 0x04
#define ELF_SECTION_FLAG_MASKPROC  0xf0000000

typedef struct Elf32SectionHeader {
  u32 name; // An offset to a string in the .shstrtab section that represents the name of this section.
  u32 type;
  u32 flags;
  u32 addr;
  u32 offset;
  u32 size;
  u32 link;
  u32 info;
  u32 addralign;
  u32 entsize;
} Elf32SectionHeader;

#ifdef __x86_64__
typedef struct Elf64SectionHeader {
  u32 name; // An offset to a string in the .shstrtab section that represents the name of this section.
  u32 type;
  u64 flags;
  u64 addr;
  u64 offset;
  u64 size;
  u32 link;
  u32 info;
  u64 addralign;
  u64 entsize;
} Elf64SectionHeader;
#endif

// --------------------------------------------------
//; Program Header

typedef struct Elf32ProgramHeader {
  u32 type;
  u32 offset;
  u32 vaddr;
  u32 paddr; // (ignored)
  u32 filesz;
  u32 memsz;
  u32 flags;
  u32 align;
} Elf32ProgramHeader;

#ifdef __x86_64__
typedef struct Elf64ProgramHeader {
  u32 type;
  u32 flags;
  u64 offset;
  u64 vaddr;
  u64 paddr; // (ignored)
  u64 filesz;
  u64 memsz;
  u64 align;
} Elf64ProgramHeader;
#endif

#define ELF_PROGRAM_TYPE_NULL      0 // Program header table entry unused.
#define ELF_PROGRAM_TYPE_LOAD      1 // Loadable segment.
#define ELF_PROGRAM_TYPE_DYNAMIC   2 // Dynamic linking information.
#define ELF_PROGRAM_TYPE_INTERP    3 // Interpreter information.
#define ELF_PROGRAM_TYPE_NOTE      4 // Auxiliary information.
#define ELF_PROGRAM_TYPE_SHLIB     5 // Reserved.
#define ELF_PROGRAM_TYPE_PHDR      6 // Segment containing program header table itself.
#define ELF_PROGRAM_TYPE_TLS       7 // Thread-Local Storage template.
#define ELF_PROGRAM_TYPE_LOOS      0x60000000
#define ELF_PROGRAM_TYPE_HIOS      0x6FFFFFFF
#define ELF_PROGRAM_TYPE_LOPROC    0x70000000
#define ELF_PROGRAM_TYPE_HIPROC    0x7fffffff
#define ELF_PROGRAM_TYPE_GNU_STACK 0x6474e551

#define ELF_PROGRAM_FLAG_EXEC  MASK(0)
#define ELF_PROGRAM_FLAG_WRITE MASK(1)
#define ELF_PROGRAM_FLAG_READ  MASK(2)

// --------------------------------------------------
//; Dynamic Section

typedef struct Elf32Dynamic {
  u32 tag;
  u32 value;
} Elf32Dynamic;

#ifdef __x86_64__
typedef struct Elf64Dynamic {
  u64 tag;
  u64 value;
} Elf64Dynamic;
#endif

#define ELF_DYNAMIC_NULL            0 // 结束标记
#define ELF_DYNAMIC_NEEDED          1 // 依赖库
#define ELF_DYNAMIC_PLTRELSZ        2 // 重定位表大小
#define ELF_DYNAMIC_PLTGOT          3 // 重定位表地址
#define ELF_DYNAMIC_HASH            4 // 符号哈希表地址
#define ELF_DYNAMIC_STRTAB          5 // 字符串表地址
#define ELF_DYNAMIC_SYMTAB          6 // 符号表地址
#define ELF_DYNAMIC_RELA            7
#define ELF_DYNAMIC_RELASZ          8
#define ELF_DYNAMIC_RELAENT         9
#define ELF_DYNAMIC_STRSZ           10
#define ELF_DYNAMIC_SYMENT          11
#define ELF_DYNAMIC_INIT            12 // 初始化函数地址
#define ELF_DYNAMIC_FINI            13 // 结束函数地址
#define ELF_DYNAMIC_SONAME          14
#define ELF_DYNAMIC_RPATH           15
#define ELF_DYNAMIC_SYMBOLIC        16
#define ELF_DYNAMIC_REL             17
#define ELF_DYNAMIC_RELSZ           18
#define ELF_DYNAMIC_RELENT          19
#define ELF_DYNAMIC_PLTREL          20
#define ELF_DYNAMIC_DEBUG           21
#define ELF_DYNAMIC_TEXTREL         22
#define ELF_DYNAMIC_JMPREL          23
#define ELF_DYNAMIC_BIND_NOW        24
#define ELF_DYNAMIC_INIT_ARRAY      25
#define ELF_DYNAMIC_FINI_ARRAY      26
#define ELF_DYNAMIC_INIT_ARRAYSZ    27
#define ELF_DYNAMIC_FINI_ARRAYSZ    28
#define ELF_DYNAMIC_RUNPATH         29
#define ELF_DYNAMIC_FLAGS           30
#define ELF_DYNAMIC_ENCODING        32
#define ELF_DYNAMIC_PREINIT_ARRAY   32
#define ELF_DYNAMIC_PREINIT_ARRAYSZ 33
#define ELF_DYNAMIC_MAXPOSTAGS      34
#define ELF_DYNAMIC_GNU_HASH        0x6ffffef5

// --------------------------------------------------
//; Symbol Table

typedef struct Elf32Symbol {
  u32 name;
  u32 value;
  u32 size;
  u8  info;
  u8  other;
  u16 shndx;
} Elf32Symbol;

#ifdef __x86_64__
typedef struct Elf64Symbol {
  u32 name;
  u8  info;
  u8  other;
  u16 shndx;
  u64 value;
  u64 size;
} Elf64Symbol;
#endif

// --------------------------------------------------
//; Hash Table

typedef struct Elf32Hash {
  u32 nbucket;
  u32 nchain;
  u32 bucket;
  u32 chain;
} Elf32Hash;

typedef struct Elf64Hash {
  u32 nbucket;
  u32 nchain;
  u32 bucket;
  u32 chain;
} Elf64Hash;

finline u32 elf_hash(cstr name) {
  u32 h = 0;
  for (usize i = 0; name[i] != '\0'; i++) {
    h     <<= 4;
    h      += name[i];
    u32 g   = h & 0xf0000000;
    h      ^= g;
    h      ^= g >> 24;
  }
  return h;
}

typedef struct Elf32GnuHash {
  u32 nbuckets;
  u32 symndx;
  u32 maskwords;
  u32 shift2;
  u32 bloom[1];
} Elf32GnuHash;

// --------------------------------------------------
//; elf 加载时数据

typedef void (*ElfInitFunc)();
typedef void (*ElfFiniFunc)();

typedef struct Elf {
  cstr  path; // 文件路径 (不属于结构体所有)
  usize size; // 文件大小
  union {
    const void        *data;
    const ElfIdent    *ident;
    const Elf32Header *header32;
#ifdef __x86_64__
    const Elf64Header *header64;
#endif
  };
  i32 errcode;

  cstr strtab;

  union {
    const Elf32Symbol *sym32;
#ifdef __x86_64__
    const Elf64Symbol *sym64;
#endif
  };

  union {
    const Elf32Hash *hash32;
#ifdef __x86_64__
    const Elf32Hash *hash64;
#endif
  };

  ElfInitFunc        init;
  ElfFiniFunc        fini;
  const ElfInitFunc *init_array;
  usize              init_array_len;
  const ElfFiniFunc *fini_array;
  usize              fini_array_len;
} Elf;
