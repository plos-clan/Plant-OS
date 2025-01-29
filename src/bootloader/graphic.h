//
// Created by NekoMiao on 1/28/25.
//

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "uefi/uefi.h"

#define LOGO_X_OFFSET 0
#define LOGO_Y_OFFSET 0

typedef struct {
  uint16_t type;             // "BM" (0x42, 0x4D)
  uint32_t size;             // file size
  uint16_t reserved1;        // not used (0)
  uint16_t reserved2;        // not used (0)
  uint32_t offset;           // offset to image data (54B)
  uint32_t dib_size;         // DIB header size (40B)
  uint32_t width;            // width in pixels
  uint32_t height;           // height in pixels
  uint16_t planes;           // 1
  uint16_t bpp;              // bits per pixel (24)
  uint32_t compression;      // compression type (0/1/2) 0
  uint32_t image_size;       // size of picture in bytes, 0
  uint32_t x_ppm;            // X Pixels per meter (0)
  uint32_t y_ppm;            // X Pixels per meter (0)
  uint32_t num_colors;       // number of colors (0)
  uint32_t important_colors; // important colors (0)
} __attribute__((__packed__)) BMP_HEADER;

efi_status_t graphic_init(efi_gop_t **gop);
efi_status_t draw_logo(efi_gop_t *gop);

#endif //GRAPHIC_H
