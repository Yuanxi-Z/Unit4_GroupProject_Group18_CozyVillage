#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>

void DS18B20_Init(void);
uint8_t DS18B20_StartConversion(void);
uint8_t DS18B20_IsConversionDone(void);
uint8_t DS18B20_ReadTemperatureC(float *temp_c);

#endif