#pragma once
#include <define.h>

// elf documentation: http://www.sco.com/developers/devspecs/gabi41.pdf

// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

// ELF HEADER

// identification indexes
#define EI_CLASS   4 // file class
#define EI_DATA    5 // data encoding
#define EI_VERSION 6 // file version
#define EI_PAD     7 // start of padding bytes
#define EI_NIDENT  16

#define ELF_MAGIC MAGIC32(0x7f, 'E', 'L', 'F')

// file class
#define ELFCLASSNONE 0 // invalid class
#define ELF_CLASS_32 1 // 32-bit objects
#define ELF_CLASS_64 2 // 64-bit objects

// data encoding
#define ELFDATANONE 0 // invalid data encoding
#define ELFDATA2LSB 1 // little endian
#define ELFDATA2MSB 2 // big endian

// object file type
#define ET_NONE   0      // no file type
#define ET_ELF    1      // Relocatable file
#define ET_EXEC   2      // Executable file
#define ET_DYN    3      // Shared object file
#define ET_CORE   4      // Core file
#define ET_LOPROC 0xff00 // Processor-specfic
#define ET_HIPROC 0xffff // Processor-specfic

// machine architecture
#define EM_NONE  0 // No machine
#define EM_M32   1 // AT&T WE 32100
#define EM_SPARC 2 // SPARC
#define EM_386   3 // Intel 80386
#define EM_68K   4 // Motorola 68000
#define EM_88K   5 // Motorola 88000
#define EM_860   7 // Intel 80860
#define EM_MIPS  8 // MIPS RS3000

// object file version
#define EV_NONE    0 // Invalid version
#define EV_CURRENT 1 // Current version

typedef struct {
  u8  ident[EI_NIDENT]; // elf identification
  u16 type;             // object file type
  u16 machine;          // required architecture
  u32 version;          // object file version
  u32 entry;            // first transfer control virtual address
  u32 e_phoff;          // program header table offset
  u32 e_shoff;          // section header table offset
  u32 e_flags;          // processor-specific flags
  u16 e_ehsize;         // elf header size
  u16 e_phentsize;
  u16 e_phnum;
  u16 e_shentsize;
  u16 e_shnum;
  u16 e_shstrndx;
} Elf32Header;

// SECTION HEADER

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

typedef struct {
  u32 sh_name;
  u32 sh_type;
  u32 sh_flags;
  u32 sh_addr;
  u32 sh_offset;
  u32 sh_size;
  u32 sh_link;
  u32 sh_info;
  u32 sh_addralign;
  u32 sh_entsize;
} Elf32SectionHeader;

// PROGRAM HEADER

// segment type
#define PT_NULL  0
#define PT_LOAD  1
#define PT_SHLIB 5
#define PT_PHDR  6
#define LOPROC   0x70000000
#define HIPROC   0x7fffffff

typedef struct {
  u32 p_type;
  u32 p_offset;
  u32 p_vaddr;
  u32 p_paddr;
  u32 p_filesz;
  u32 p_memsz;
  u32 p_flags;
  u32 p_align;
} Elf32ProgramHeader;

bool  elf32_is_validate(const Elf32Header *hdr);
void  elf32_load_data(Elf32Header *elfhdr, u8 *ptr);
u32   elf32_get_max_vaddr(const Elf32Header *hdr);
usize load_elf(Elf32Header *hdr);
