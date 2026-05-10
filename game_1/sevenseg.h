#ifndef SEVENSEG_H
#define SEVENSEG_H

#include <stdint.h>

void SevenSeg_Init(void);
void SevenSeg_DisplayDigit(uint8_t digit);
void SevenSeg_AllOff(void);
void SevenSeg_Blank(void);

#endif