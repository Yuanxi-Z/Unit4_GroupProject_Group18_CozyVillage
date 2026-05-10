#ifndef GAME_2_H
#define GAME_2_H

#include "Menu.h"

/**
 * @brief Game 2 - Fishing Minigame
 * * This file implements the fishing minigame for the project. 
 * The core gameplay relies on two main external peripherals:
 * - Accelerometer: Detects the player's physical rod swinging motion to cast the line.
 * - Pressure Sensor: Reads the pressing force to control the line tension during the reeling phase.
 * * The Game2_Run() function contains the main state machine for the game loop,
 * which also handles the shop and fish collection logic. It returns the exit state 
 * to the menu system when the player leaves the game.
 * * @return MenuState - Next state to transition to (typically MENU_STATE_HOME)
 */

MenuState Game2_Run(void);

#endif // GAME_2_H
