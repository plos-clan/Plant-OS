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
#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_HASH     5
#define SHT_DYNAMIC  6
#define SHT_NOTE     7
#define SHT_NOBITS   8
#define SHT_REL      9
#define SHT_SHLIB    10
#define SHT_DYNSYM   11
#define SHT_LOPROC   0x70000000
#define SHT_HIPROP   0x7fffffff
#define SHT_LOUSER   0x80000000
#define SHT_HIUSER   0xffffffff

// section attribute flags
#define SHF_WRITE     0x01
#define SHF_ALLOC     0x02
#define SHF_EXECINSTR 0x04
#define SHF_MASKPROC  0xf0000000

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
  u32 paddr;
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
  u64 paddr;
  u64 filesz;
  u64 memsz;
  u64 align;
} Elf64ProgramHeader;
#endif

#define ELF_PROGRAM_TYPE_NULL    0 // Program header table entry unused.
#define ELF_PROGRAM_TYPE_LOAD    1 // Loadable segment.
#define ELF_PROGRAM_TYPE_DYNAMIC 2 // Dynamic linking information.
#define ELF_PROGRAM_TYPE_INTERP  3 // Interpreter information.
#define ELF_PROGRAM_TYPE_NOTE    4 // Auxiliary information.
#define ELF_PROGRAM_TYPE_SHLIB   5 // Reserved.
#define ELF_PROGRAM_TYPE_PHDR    6 // Segment containing program header table itself.
#define ELF_PROGRAM_TYPE_TLS     7 // Thread-Local Storage template.
#define ELF_PROGRAM_TYPE_LOOS    0x60000000
#define ELF_PROGRAM_TYPE_HIOS    0x6FFFFFFF
#define ELF_PROGRAM_TYPE_LOPROC  0x70000000
#define ELF_PROGRAM_TYPE_HIPROC  0x7fffffff

#define ELF_PROGRAM_FLAG_EXEC  MASK(0)
#define ELF_PROGRAM_FLAG_WRITE MASK(1)
#define ELF_PROGRAM_FLAG_READ  MASK(2)
