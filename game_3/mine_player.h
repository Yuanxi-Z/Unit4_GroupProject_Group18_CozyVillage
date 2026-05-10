#ifndef MINE_PLAYER_H
#define MINE_PLAYER_H

#include <stdint.h>
#include "Joystick.h"

#define TORCH_MAX 120

typedef struct {
    int8_t x;
    int8_t y;
    int16_t torch_ticks;
    uint32_t last_move_tick;
    uint32_t last_mine_tick;
    uint8_t anim_frame;
    uint8_t depth;
    uint8_t is_poisoned;
} Player_t;

extern Player_t player;

void player_init(void);
void player_init_on_floor(uint8_t floor);
uint8_t player_move(Direction dir, uint32_t now);
uint8_t player_mine(uint32_t now);
uint8_t player_is_mining(uint32_t now);

#endif // MINE_PLAYER_H