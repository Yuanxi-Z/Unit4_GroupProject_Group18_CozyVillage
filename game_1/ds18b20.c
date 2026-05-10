#include "ds18b20.h"
#include "main.h"
#include "stm32l4xx_hal.h"

/*
 * Generate a microsecond-level delay using the Cortex-M DWT cycle counter.
 * DS18B20 communication is timing-sensitive, so HAL_Delay() is not accurate enough.
 */
static void DS18B20_DelayUs(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000U);

    while ((DWT->CYCCNT - start) < ticks) { }
}

/*
 * Configure the DS18B20 data pin as open-drain output.
 * Open-drain mode is required for the 1-Wire bus because the line is shared
 * and released high by the external pull-up resistor.
 */
static void DS18B20_PinOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = A3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(A3_GPIO_Port, &GPIO_InitStruct);
}

/*
 * Configure the DS18B20 data pin as input.
 * This releases the 1-Wire bus so that the sensor or pull-up resistor can control the line.
 */
static void DS18B20_PinInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = A3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(A3_GPIO_Port, &GPIO_InitStruct);
}

/*
 * Send a reset pulse to the DS18B20 and check for the presence response.
 * Return 1 if the sensor responds correctly, otherwise return 0.
 */
static uint8_t DS18B20_ResetPulse(void)
{
    uint8_t presence = 0;

    /* Pull the bus low for at least 480 us to start the reset sequence. */
    DS18B20_PinOutput();
    HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);
    DS18B20_DelayUs(480);

    /* Release the bus and wait for the DS18B20 presence pulse. */
    DS18B20_PinInput();
    DS18B20_DelayUs(70);

    /* A valid DS18B20 pulls the line low during the presence window. */
    presence = (HAL_GPIO_ReadPin(A3_GPIO_Port, A3_Pin) == GPIO_PIN_RESET) ? 1 : 0;

    /* Wait until the full reset time slot is complete. */
    DS18B20_DelayUs(410);

    return presence;
}

/*
 * Write one bit to the DS18B20.
 * The timing width of the low pulse determines whether the written bit is 0 or 1.
 */
static void DS18B20_WriteBit(uint8_t bit)
{
    DS18B20_PinOutput();
    HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);

    if (bit) {
        /* Write logic 1: short low pulse, then release the bus. */
        DS18B20_DelayUs(6);
        DS18B20_PinInput();
        DS18B20_DelayUs(64);
    } else {
        /* Write logic 0: keep the bus low for most of the time slot. */
        DS18B20_DelayUs(60);
        DS18B20_PinInput();
        DS18B20_DelayUs(10);
    }
}

/*
 * Read one bit from the DS18B20.
 * The master starts the read slot, then releases the bus and samples the line.
 */
static uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit = 0;

    /* Pull the bus low briefly to start a read time slot. */
    DS18B20_PinOutput();
    HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);
    DS18B20_DelayUs(6);

    /* Release the bus and allow the DS18B20 to output the bit value. */
    DS18B20_PinInput();
    DS18B20_DelayUs(9);

    /* Sample the bus level: high means 1, low means 0. */
    bit = (HAL_GPIO_ReadPin(A3_GPIO_Port, A3_Pin) == GPIO_PIN_SET) ? 1 : 0;

    /* Wait until the read time slot is complete. */
    DS18B20_DelayUs(55);

    return bit;
}

/*
 * Write one byte to the DS18B20.
 * Data is transmitted least significant bit first according to the 1-Wire protocol.
 */
static void DS18B20_WriteByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        DS18B20_WriteBit(data & 0x01);
        data >>= 1;
    }
}

/*
 * Read one byte from the DS18B20.
 * Data is received least significant bit first and reconstructed into one byte.
 */
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t data = 0;

    for (uint8_t i = 0; i < 8; i++) {
        data >>= 1;

        if (DS18B20_ReadBit()) {
            data |= 0x80;
        }
    }

    return data;
}

/*
 * Initialise the DS18B20 driver.
 * The DWT cycle counter is enabled to provide accurate microsecond delays.
 */
void DS18B20_Init(void)
{
    /* Enable access to the DWT peripheral. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* Enable the DWT cycle counter for timing control. */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* Reset the cycle counter before use. */
    DWT->CYCCNT = 0;

    /* Keep the 1-Wire data line released when idle. */
    DS18B20_PinInput();
}

/*
 * Start a temperature conversion on the DS18B20.
 * Return 1 if the command is accepted, otherwise return 0.
 */
uint8_t DS18B20_StartConversion(void)
{
    /* Check that the sensor is present before sending commands. */
    if (!DS18B20_ResetPulse()) {
        return 0;
    }

    /* Address all devices on the bus. This is suitable when only one DS18B20 is used. */
    DS18B20_WriteByte(0xCC);

    /* Start temperature conversion. */
    DS18B20_WriteByte(0x44);

    return 1;
}

/*
 * Check whether the temperature conversion has finished.
 * During conversion, the DS18B20 holds the bus low; when complete, it releases the bus high.
 */
uint8_t DS18B20_IsConversionDone(void)
{
    return DS18B20_ReadBit();
}

/*
 * Read the measured temperature in degrees Celsius.
 * The function reads the first two bytes of the scratchpad and converts the raw value.
 * Return 1 if the reading is successful, otherwise return 0.
 */
uint8_t DS18B20_ReadTemperatureC(float *temp_c)
{
    uint8_t temp_lsb, temp_msb;
    int16_t raw;

    /* Protect against invalid output pointer. */
    if (!temp_c) {
        return 0;
    }

    /* Confirm that the sensor is present before reading data. */
    if (!DS18B20_ResetPulse()) {
        return 0;
    }

    /* Address all devices on the bus. This is suitable for a single-sensor setup. */
    DS18B20_WriteByte(0xCC);

    /* Read the DS18B20 scratchpad memory. */
    DS18B20_WriteByte(0xBE);

    /* The first two scratchpad bytes contain the temperature result. */
    temp_lsb = DS18B20_ReadByte();
    temp_msb = DS18B20_ReadByte();

    /* Combine MSB and LSB into a signed 16-bit raw temperature value. */
    raw = (int16_t)((temp_msb << 8) | temp_lsb);

    /* DS18B20 default resolution is 12-bit, so each count represents 0.0625°C. */
    *temp_c = (float)raw / 16.0f;

    return 1;
}