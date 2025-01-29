#ifndef _ELF_H_
#define _ELF_H_

#include "uefi/uefi.h"

#define PT_LOAD 1

typedef struct {
  uint32_t Magic;      //0x00
  uint8_t  Format;     //0x04,32 or 64 bits format
  uint8_t  Endianness; //0x05
  uint8_t  Version;    //0x06
  uint8_t  OSAbi;      //0x07
  uint8_t  AbiVersion; //0x08
  uint8_t  Rserved[7]; //0x09
  uint16_t Type;       //0x10
  uint16_t Machine;    //0x12
  uint32_t ElfVersion; //0x14
  uint64_t Entry;      //0x18
  uint64_t Phoff;      //0x20
  uint64_t Shoff;      //0x28
  uint32_t Flags;      //0x30
  uint16_t HeadSize;   //0x34,size of elf head
  uint16_t PHeadSize;  //0x36, size of a program header table entry
  uint16_t PHeadCount; //0x38, count of entries in the program header table
  uint16_t SHeadSize;  //0x3A, size of a section header table entry
  uint16_t SHeadCount; //0x3C, count of entries in the section header table
  uint16_t SNameIndex; //0x3E, index of entry that contains the section names
} ELF_HEADER_64;

typedef struct {
  uint32_t Type;         //0x00, type of segment
  uint32_t Flags;        //0x04 Segment-dependent flags
  uint64_t Offset;       //0x08
  uint64_t VAddress;     //0x10
  uint64_t PAddress;     //0x18
  uint64_t SizeInFile;   //0x20
  uint64_t SizeInMemory; //0x28
  uint64_t Align;        //0x30

} PROGRAM_HEADER_64;

efi_status_t load_kernel(char_t *filename, uintn_t **kernel_entry);

#endif