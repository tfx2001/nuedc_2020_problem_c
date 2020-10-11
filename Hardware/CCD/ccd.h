#ifndef _CCD_H
#define _CCD_H

#include <stdint.h>

void CCD_init();
void CCD_read(uint16_t Vaules[128]);
#endif