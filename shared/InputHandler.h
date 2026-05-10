#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <stdint.h>

/*
 * Input state for one frame.
 * Each flag is set to 1 when the corresponding button press is detected.
 */
typedef struct {
    uint8_t btn2_pressed;  /* BTN2 input. */
    uint8_t btn3_pressed;  /* Physical BTN3 input. */
    uint8_t joy_pressed;   /* Joystick push input. */
} InputState;

/*
 * Global input state shared by the game modules.
 */
extern InputState current_input;

/*
 * Initialise input state and clear all pending button flags.
 */
void Input_Init(void);

/*
 * Read the latest button events captured by interrupts.
 * Each press event is consumed once after this function is called.
 */
void Input_Read(void);

#endif /* INPUT_HANDLER_H */