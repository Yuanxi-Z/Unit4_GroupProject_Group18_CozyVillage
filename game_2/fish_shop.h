#ifndef FISH_SHOP_H
#define FISH_SHOP_H

#include <stdint.h>
#include <stdbool.h>
#include "Joystick.h"

// Initialize shop state
void Fish_Shop_Init(void);

// Draw the shop UI
void Fish_Shop_Draw_UI(uint32_t tick);

// Handle shop inputs. Returns true if the player presses BT3 to exit.
bool Fish_Shop_Handle_Input(uint8_t bt2, uint8_t bt3, Direction dir, uint32_t tick);

#endif // FISH_SHOP_H