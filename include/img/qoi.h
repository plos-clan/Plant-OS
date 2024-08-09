#pragma once
#include <libc-base.h>

extern void *qoi_encode_rgb(const void *_data, int width, int height, size_t *size);

extern void *qoi_encode_rgba(const void *_data, int width, int height, size_t *size);

extern void *qoi_decode_rgb(const void *data, size_t size, int width, int height);

extern void *qoi_decode_rgba(const void *data, size_t size, int width, int height);

extern void *qoi_encode(const void *data, size_t *size, int width, int height, int channels);

extern void *qoi_decode(const void *data, size_t size, int *width, int *height, int *channels);

extern void *qoi_decode_f32(const void *data, size_t size, int *width, int *height, int *channels);

extern void *qoi_load(const char *filename, int *width, int *height, int *channels);

extern int qoi_save(const char *filename, const void *data, int width, int height, int channels);
