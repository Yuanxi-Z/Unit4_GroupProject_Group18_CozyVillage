#include "sevenseg.h"
#include "main.h"
#include "stm32l4xx_hal.h"

/*
 * Seven-segment digit map.
 *
 * Segment order:
 *   A, B, C, D, E, F, G
 *
 * 1 means the segment is turned on.
 * 0 means the segment is turned off.
 *
 * This table assumes a common-cathode or active-high segment connection.
 */
static const uint8_t seg_map[10][7] = {
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,0,1,1,1,1,1}, // 6
    {1,1,1,0,0,0,0}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}  // 9
};

/*
 * Initialise the seven-segment display module.
 *
 * GPIO pins are already configured by CubeMX, so no extra setup is needed here.
 * This function is kept for a consistent driver interface.
 */
void SevenSeg_Init(void)
{
    /* GPIO configuration is handled in main.c / CubeMX generated code. */
}

/*
 * Display one decimal digit on the seven-segment display.
 *
 * If the input value is outside 0-9, it is safely displayed as 0.
 */
void SevenSeg_DisplayDigit(uint8_t digit)
{
    if (digit > 9) {
        digit = 0;
    }

    /*
     * Write each segment according to the digit lookup table.
     */
    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, seg_map[digit][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, seg_map[digit][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, seg_map[digit][2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, seg_map[digit][3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, seg_map[digit][4] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, seg_map[digit][5] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, seg_map[digit][6] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /*
     * Decimal point is not used in this project.
     */
    HAL_GPIO_WritePin(SEG_DP_GPIO_Port, SEG_DP_Pin, GPIO_PIN_RESET);
}

/*
 * Turn off all seven-segment display segments.
 *
 * This is used when leaving the farm game or clearing the display.
 */
void SevenSeg_Blank(void)
{
    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_DP_GPIO_Port, SEG_DP_Pin, GPIO_PIN_RESET);
}