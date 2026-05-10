#ifndef MENU_H
#define MENU_H

#include <stdint.h>

// ==============================================
// MENU STATE MACHINE
// ==============================================

typedef enum {
    MENU_STATE_INTRO = 0,       // Opening animation - plays once at startup
    MENU_STATE_HOME,            // Main menu - select game
    MENU_STATE_GAME_1,          // Running Game 1
    MENU_STATE_GAME_2,          // Running Game 2
    MENU_STATE_GAME_3,          // Running Game 3
} MenuState;

// Menu system structure
typedef struct {
    uint8_t selected_option;    // Which menu option is highlighted (0-2)
} MenuSystem;

// ==============================================
// INITIALIZATION AND STATE MANAGEMENT
// ==============================================

/**
 * @brief Initialize the menu system
 */
void Menu_Init(MenuSystem* menu);

/**
 * @brief Play the 9-second opening animation (Stardew-style village dawn).
 *        Blocks until animation finishes OR user presses BT3 to skip.
 *        Sequence: starry night -> sunrise -> villager walks in -> cottage
 *        slides in -> title card. LED fades in; buzzer plays a 3-note chord.
 */
void Menu_RunIntro(void);

/**
 * @brief Run the main menu - a 2D village map where the player walks a
 *        villager sprite with the joystick and presses BT3 when standing
 *        next to a building (Farm / Lake / Mine) to enter that game.
 *
 * Runs its own loop and returns the selected game state.
 *
 * @return MenuState - The game that was selected (GAME_1, GAME_2, or GAME_3)
 */
MenuState Menu_Run(MenuSystem* menu);

#endif // MENU_H
