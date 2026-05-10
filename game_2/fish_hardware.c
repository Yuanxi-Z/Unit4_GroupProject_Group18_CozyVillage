#include "fish_hardware.h"
#include <stdlib.h> // Needed for abs()

I2C_HandleTypeDef hi2c1; 
extern ADC_HandleTypeDef hadc1;

// MMA8452Q Accelerometer I2C addresses and registers
#define MMA8452Q_ADDR         0x3A  
#define MMA8452Q_XYZ_DATA_CFG 0x0E
#define MMA8452Q_CTRL_REG1    0x2A
#define MMA8452Q_CTRL_REG2    0x2B  

void Fish_Hardware_Init(void) {
    // 1. Initialize I2C1 for the accelerometer (PB8, PB9)
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;       
    GPIO_InitStruct.Pull = GPIO_PULLUP;           
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x10909CEC; 
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1); 

    // 2. Initialize ADC1 for the pressure sensor (PA4 -> Channel 9)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_ADC_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; 
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;           
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    HAL_ADC_Init(&hadc1);

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5; 
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    // 3. Initialize GPIO for RGB LED (PA8, PA9, PA10)
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Fish_Sensor_Init(void) {
    // Wake up and configure the MMA8452Q accelerometer
    uint8_t Data;
    Data = 0x40; HAL_I2C_Mem_Write(&hi2c1, MMA8452Q_ADDR, MMA8452Q_CTRL_REG2, 1, &Data, 1, 100);
    HAL_Delay(50); 
    Data = 0x00; HAL_I2C_Mem_Write(&hi2c1, MMA8452Q_ADDR, MMA8452Q_CTRL_REG1, 1, &Data, 1, 100);
    HAL_Delay(10);
    Data = 0x01; HAL_I2C_Mem_Write(&hi2c1, MMA8452Q_ADDR, MMA8452Q_XYZ_DATA_CFG, 1, &Data, 1, 100);
    Data = 0x01; HAL_I2C_Mem_Write(&hi2c1, MMA8452Q_ADDR, MMA8452Q_CTRL_REG1, 1, &Data, 1, 100);
    HAL_Delay(50); 
}

void Fish_RGB_Set(uint8_t r, uint8_t g, uint8_t b) {
    // Update RGB pins. Mapping depends on the physical board wiring.
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, b ? GPIO_PIN_SET : GPIO_PIN_RESET);  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, g ? GPIO_PIN_SET : GPIO_PIN_RESET);  
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t Fish_Detect_Rod_Swing(void) {
    uint8_t Rec_Data[6]; 
    int16_t z_accel = 0;
    
    // Read raw XYZ data from the accelerometer
    if (HAL_I2C_Mem_Read(&hi2c1, MMA8452Q_ADDR, 0x01, 1, Rec_Data, 6, 100) == HAL_OK) {
        // Extract Z-axis acceleration
        z_accel = (int16_t)((Rec_Data[4] << 8) | Rec_Data[5]);
        z_accel >>= 4; 
        
        // Return 1 if a strong casting motion is detected
        if (abs(z_accel) > 800) {  // Experience Threshold
            return 1; 
        }
    }
    return 0; 
}

uint32_t Fish_Read_Pressure(void) {
    /* * Force ADC to Channel 9 (PA4) before reading.
     * This prevents channel conflicts when switching between different game modules
     * that might be using the ADC for other sensors.
     */
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_9; 
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5; 
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
    
    // Wait for conversion with a 10ms timeout
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        return HAL_ADC_GetValue(&hadc1); 
    }
    
    return 0; // Return 0 if the read times out or fails
}