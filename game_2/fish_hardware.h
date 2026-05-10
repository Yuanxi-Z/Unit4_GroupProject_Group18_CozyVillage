#ifndef FISH_HARDWARE_H
#define FISH_HARDWARE_H

#include "stm32l4xx_hal.h"
#include <stdint.h>

// Initialize I2C, ADC, and RGB LED GPIOs
void Fish_Hardware_Init(void);

// Initialize MMA8452Q Accelerometer
void Fish_Sensor_Init(void);

// Set RGB LED state (1 = ON, 0 = OFF)
void Fish_RGB_Set(uint8_t r, uint8_t g, uint8_t b);

// Detect rod swing via accelerometer (Returns 1 if swung, 0 otherwise)
uint8_t Fish_Detect_Rod_Swing(void);

// Read pressure sensor via ADC
uint32_t Fish_Read_Pressure(void);

#endif // FISH_HARDWARE_H