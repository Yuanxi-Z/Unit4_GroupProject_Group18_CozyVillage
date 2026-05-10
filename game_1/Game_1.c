#include "Game_1.h"
#include "Farm.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

/*
 * External hardware control objects.
 * These are configured elsewhere and reused by the game module.
 */
extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Buzzer_cfg_t buzzer_cfg;

/*
 * Target frame time for the farm game.
 * 30 ms gives approximately 33 frames per second.
 */
#define GAME1_FRAME_TIME_MS 30

/*
 * Run Game 1: Harvest farm game.
 *
 * This function owns the main game loop. It initialises the farm module,
 * updates input and gameplay, renders each frame, and returns to the menu
 * when the exit button combination is pressed.
 */
MenuState Game1_Run(void)
{
    MenuState exit_state = MENU_STATE_HOME;

    /*
     * Initialise the complete farm game state.
     */
    Farm_Init();

    /*
     * Play a short startup tone when the game begins.
     */
    buzzer_tone(&buzzer_cfg, 1000, 30);
    HAL_Delay(50);
    buzzer_off(&buzzer_cfg);

    while (1) {
        uint32_t frame_start = HAL_GetTick();

        /*
         * Read all button and joystick inputs once per frame.
         */
        Input_Read();

        /*
         * Press BT2 and BT3 together to leave the farm game
         * and return to the main menu.
         */
        if (current_input.btn2_pressed && current_input.btn3_pressed) {
            Farm_Deinit();

            /*
             * Restore LED PWM to the default menu brightness.
             */
            PWM_SetDuty(&pwm_cfg, 50);

            exit_state = MENU_STATE_HOME;
            break;
        }

        /*
         * Update gameplay logic, sensors, sounds, animations, and scene state.
         */
        Farm_Update();

        /*
         * Draw the current farm scene and push the frame to the LCD.
         */
        Farm_Render();
        LCD_Refresh(&cfg0);

        /*
         * Keep a stable frame rate by delaying the remaining frame time.
         */
        {
            uint32_t frame_time = HAL_GetTick() - frame_start;

            if (frame_time < GAME1_FRAME_TIME_MS) {
                HAL_Delay(GAME1_FRAME_TIME_MS - frame_time);
            }
        }
    }

    return exit_state;
}