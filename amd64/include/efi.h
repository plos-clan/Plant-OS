#pragma once

#include <stdint.h>

#define EFIAPI __attribute__((ms_abi))

typedef struct {
  uint32_t field1;
  uint16_t field2;
  uint16_t field3;
  uint8_t  field4[8];
} efi_guid_t;

typedef int64_t efi_status_t;
typedef void   *efi_handle_t;

typedef struct {
  uint64_t signature;
  uint32_t revision;
  uint32_t header_size;
  uint32_t crc32;
  uint32_t reserved;
} efi_table_header;

enum efi_allocation_type {
  EfiAllocateAnyPages,
  EfiAllocateMaxAddress,
  EfiAllocateAddress,
  EfiMaxAllocateType
};

enum efi_memory_type {
  EfiReservedMemoryType,
  EfiLoaderCode,
  EfiLoaderData,
  EfiBootServicesCode,
  EfiBootServicesData,
  EfiRuntimeServicesCode,
  EfiRuntimeServicesData,
  EfiConventionalMemory,
  EfiUnusableMemory,
  EfiACPIReclaimMemory,
  EfiACPIMemoryNVS,
  EfiMemoryMappedIO,
  EfiMemoryMappedIOPortSpace,
  EfiPalCode,
  EfiPersistentMemory,
  EfiMaxMemoryType
};

typedef struct {
  uint32_t type;
  uint32_t pad;
  uint64_t physical_start;
  uint64_t virtual_start;
  uint64_t number_of_pages;
  uint64_t attribute;

  uint64_t reserved;
} efi_memory_descriptor;

typedef struct {
  efi_table_header hdr;

  void *unused0[2];

  efi_status_t(EFIAPI *allocate_pages)(enum efi_allocation_type type,
                                       enum efi_memory_type memory_type, uint64_t count,
                                       void **buffer);

  void *unused1;

  efi_status_t(EFIAPI *get_memory_map)(uint64_t *size, efi_memory_descriptor *map,
                                       uint64_t *map_key, uint64_t *descriptor_size,
                                       uint32_t *descriptor_version);
  efi_status_t(EFIAPI *allocate_pool)(enum efi_memory_type type, uint64_t size, void **buffer);

  void *unused2[18];

  efi_status_t(EFIAPI *exit)(efi_handle_t image_handle, efi_status_t status,
                             uint64_t exit_data_size, void *exit_data);
  void *unused3;
  efi_status_t(EFIAPI *exit_boot_services)(efi_handle_t image_handle, uint64_t map_key);

  void *unused4[15];

  efi_status_t(EFIAPI *setmem)(void *buffer, uint64_t size, uint8_t value);

  void *unused5;
} efi_boot_services;

typedef struct {
  efi_guid_t guid;
  void      *table;
} efi_configuration_table;

typedef struct {
  efi_table_header      hdr;
  const unsigned short *firmware_vendor;
  uint32_t              firmware_revision;

  void *conout_handle;
  void *conout;

  void *conin_handle;
  void *conin;

  void *stderr_handle;
  void *stderr;

  void              *runtime_services;
  efi_boot_services *boot_services;

  uint64_t                 number_of_configuration_table;
  efi_configuration_table *configuration_tables;
} efi_system_table;
