#pragma once
#include <define.h>
#include <sound.h>
void sb16_open(int rate, sound_pcmfmt_t fmt);
void sb16_set_volume(u8 level);
void sb16_close();
int  sb16_write(void *data, size_t size);
