#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void *qoi_encode(const void *data, size_t *size, int width, int height, int channels);

extern void *qoi_decode(const void *data, size_t size, int *width, int *height, int *channels);

extern void *qoi_decode_f32(const void *data, size_t size, int *width, int *height, int *channels);

extern void *qoi_load(const char *filename, int *width, int *height, int *channels);

extern int qoi_save(const char *filename, const void *data, int width, int height, int channels);

#ifdef __cplusplus
}
#endif
