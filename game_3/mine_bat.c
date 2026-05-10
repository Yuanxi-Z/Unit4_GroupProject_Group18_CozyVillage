#include "mine_bat.h"
#include "mine_map.h"
#include <stdlib.h>

Bat_t bats[BAT_MAX];
uint8_t bat_count = 0;

void bat_init_for_floor(uint8_t floor) {
    int8_t pts[BAT_MAX][2];
    bat_count = map_get_bat_spawns(floor, pts, BAT_MAX);

    for (uint8_t i = 0; i < BAT_MAX; i++) {
        if (i < bat_count) {
            bats[i].x = pts[i][0];
            bats[i].y = pts[i][1];
            bats[i].alive = 1;
            bats[i].anim_frame = 0;
            bats[i].last_move_tick = 0;
        } else {
            bats[i].alive = 0;
        }
    }
}

uint8_t bat_at(int8_t x, int8_t y) {
    for (uint8_t i = 0; i < bat_count; i++) {
        if (bats[i].alive && bats[i].x == x && bats[i].y == y) return 1;
    }
    return 0;
}

static uint8_t bat_can_walk_to(int8_t nx, int8_t ny,
                               int8_t player_x, int8_t player_y,
                               uint8_t self_idx) {
    uint8_t t = map_get(nx, ny);
    if (t != TILE_FLOOR) return 0;
    if (nx == player_x && ny == player_y) return 0;
    for (uint8_t i = 0; i < bat_count; i++) {
        if (i == self_idx || !bats[i].alive) continue;
        if (bats[i].x == nx && bats[i].y == ny) return 0;
    }
    return 1;
}

uint8_t bat_update(uint32_t now_ms, int8_t player_x, int8_t player_y) {
    static const int8_t dirs[4][2] = {{0,-1},{0,1},{-1,0},{1,0}};
    uint8_t bit_player = 0; 

    for (uint8_t i = 0; i < bat_count; i++) {
        if (!bats[i].alive) continue;
        if (now_ms - bats[i].last_move_tick < BAT_MOVE_INTERVAL_MS) continue;

        bats[i].last_move_tick = now_ms;
        bats[i].anim_frame ^= 1;

        uint8_t start = rand() & 3;
        for (uint8_t k = 0; k < 4; k++) {
            uint8_t d = (start + k) & 3;
            int8_t nx = bats[i].x + dirs[d][0];
            int8_t ny = bats[i].y + dirs[d][1];

            // Check if the bat attacks the player
            if (nx == player_x && ny == player_y) {
                bit_player = 1;
                break; // Stop moving if the player is attacked
            }

            if (bat_can_walk_to(nx, ny, player_x, player_y, i)) {
                bats[i].x = nx;
                bats[i].y = ny;
                break;
            }
        }
    }
    return bit_player;
}