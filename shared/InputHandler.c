#include "InputHandler.h"
#include "main.h"

/*
 * Global input state used by the game modules.
 * Each flag represents a single button press event for the current frame.
 */
InputState current_input = {0};

/*
 * Raw press flags set inside the GPIO interrupt callback.
 * These are copied into current_input in Input_Read().
 */
static volatile uint8_t btn2_raw_press = 0;
static volatile uint8_t btn3_raw_press = 0;
static volatile uint8_t joy_raw_press  = 0;

void Input_Init(void)
{
    current_input.btn2_pressed = 0;
    current_input.btn3_pressed = 0;
    current_input.joy_pressed  = 0;

    btn2_raw_press = 0;
    btn3_raw_press = 0;
    joy_raw_press  = 0;
}

void Input_Read(void)
{
    /*
     * Transfer interrupt-captured button events into the public input state.
     * The raw flags are cleared afterwards so each press is handled once.
     */
    current_input.btn2_pressed = btn2_raw_press;
    current_input.btn3_pressed = btn3_raw_press;
    current_input.joy_pressed  = joy_raw_press;

    btn2_raw_press = 0;
    btn3_raw_press = 0;
    joy_raw_press  = 0;
}

/*
 * GPIO external interrupt callback for button inputs.
 * A simple time-based debounce prevents one physical press from being
 * detected multiple times.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last_btn2_interrupt = 0;
    static uint32_t last_btn3_interrupt = 0;
    static uint32_t last_joy_interrupt  = 0;
    uint32_t current_time = HAL_GetTick();

    /*
     * BTN2 press event.
     */
    if (GPIO_Pin == BTN2_Pin) {
        if ((current_time - last_btn2_interrupt) > 200) {
            last_btn2_interrupt = current_time;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            btn2_raw_press = 1;
        }
    }

    /*
     * Joystick push event.
     */
    if (GPIO_Pin == BTN3_Pin) {
        if ((current_time - last_joy_interrupt) > 200) {
            last_joy_interrupt = current_time;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            joy_raw_press = 1;
        }
    }

    /*
     * BTN3 press event.
     */
    if (GPIO_Pin == BTN6_Pin) {
        if ((current_time - last_btn3_interrupt) > 200) {
            last_btn3_interrupt = current_time;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            btn3_raw_press = 1;
        }
    }
}