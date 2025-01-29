//
// Created by NekoMiao on 1/28/25.
//

#include "graphic.h"
#include "file.h"
#include "uefi/uefi.h"

efi_status_t graphic_init(efi_gop_t **gop) {
  efi_guid_t   graphic_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  efi_handle_t graphic_handle;
  uintn_t      graphic_handle_count;
  efi_status_t status = EFI_SUCCESS;
  status =
      BS->LocateHandle(ByProtocol, &graphic_guid, NULL, &graphic_handle_count, &graphic_handle);
  // CHECKSTATUS(status);
  status = BS->OpenProtocol(graphic_handle, &graphic_guid, (void **)gop, IM, NULL,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  (*gop)->SetMode(*gop, 22);
  return EFI_SUCCESS;
}
efi_status_t draw_logo(efi_gop_t *gop) {
  efi_status_t status;
  void        *logo_buffer = NULL;
  uintn_t      file_size;
  read_file("\\logo.bmp", &logo_buffer, &file_size);
  BMP_HEADER *bmp_header = logo_buffer;

  uintn_t  logo_width = bmp_header->width, logo_height = bmp_header->height;
  uint64_t screen_width  = gop->Mode->Information->HorizontalResolution,
           screen_height = gop->Mode->Information->VerticalResolution;
  uint64_t x             = (screen_width - logo_width) / 2 + LOGO_X_OFFSET,
           y             = (screen_height - logo_height) / 2 + LOGO_Y_OFFSET;

  void *bmp_content = logo_buffer + sizeof(BMP_HEADER);

  status =
      gop->Blt(gop, bmp_content, EfiBltBufferToVideo, 0, 0, x, y / 2, logo_width, logo_height, 0);
  return EFI_SUCCESS;
}