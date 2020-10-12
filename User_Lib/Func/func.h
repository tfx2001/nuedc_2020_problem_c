#ifndef _FUNC_H
#define _FUNC_H

#include <stdint.h>

#define BLOCK_BLACK 0x00
#define BLOCK_WHITE 0x01

#define BLOCK_OK    0x00
#define BLOCK_ERROR 0x01

uint8_t Block_Judge(uint8_t Value[], uint8_t Size);
uint8_t Block_MidPoint(uint16_t Value[], uint8_t Size);
void Block_Filter(uint16_t Value[], uint16_t Filted[], uint8_t Szie);

#endif