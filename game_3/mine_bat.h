#ifndef MINE_BAT_H
#define MINE_BAT_H

#include <stdint.h>

#define BAT_MAX               4   
#define BAT_MOVE_INTERVAL_MS  1500  // Slowed from 600ms to 1500ms to give the player enough time to escape after curing poison.

typedef struct {
    int8_t   x;
    int8_t   y;
    uint8_t  alive;          
    uint8_t  anim_frame;      
    uint32_t last_move_tick; 
} Bat_t;

extern Bat_t bats[BAT_MAX];
extern uint8_t bat_count;

void bat_init_for_floor(uint8_t floor);

// Advance all bats: pick random move + flip anim frame.
// Pass player pos to prevent bats from stepping onto the player.
// Returns 1 if any bat collided with the player.
uint8_t bat_update(uint32_t now_ms, int8_t player_x, int8_t player_y);

uint8_t bat_at(int8_t x, int8_t y);

#endif // MINE_BAT_H