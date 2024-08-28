#pragma once
#include <define.h>
#include <sound.h>
int  sb16_open(int rate, int channels, sound_pcmfmt_t fmt);
void sb16_set_volume(u8 level);
void sb16_close();
int  sb16_write(void *data, size_t size);
