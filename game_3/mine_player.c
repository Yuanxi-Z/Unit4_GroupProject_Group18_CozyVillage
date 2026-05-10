#include "mine_player.h"
#include "mine_map.h"
#include "mine_inventory.h"
#include "mine_bat.h" // Used to detect bats

#define MOVE_COOLDOWN_MS    200
#define PICKAXE_TORCH_COST    5
#define TORCH_START          60
#define MINE_ANIM_MS        250

Player_t player;

void player_init(void) {
    player.torch_ticks   = TORCH_START;
    player.last_move_tick = 0;
    player.anim_frame    = 0;
    player.last_mine_tick = 0;
    player.depth         = FLOOR_ENTRY;
    player.is_poisoned   = 0; // Initialize as healthy

    int8_t sx, sy;
    map_get_spawn(FLOOR_ENTRY, &sx, &sy);
    player.x = sx;
    player.y = sy;
}

void player_init_on_floor(uint8_t floor) {
    // Keeps torch & inventory. Only teleport.
    player.depth = floor;
    player.last_move_tick = 0;
    player.last_mine_tick = 0;
    player.anim_frame = 0;
    player.is_poisoned = 0; // Clear status when descending

    int8_t sx, sy;
    map_get_spawn(floor, &sx, &sy);
    player.x = sx;
    player.y = sy;
}

uint8_t player_move(Direction dir, uint32_t now) {
    if (now - player.last_move_tick < MOVE_COOLDOWN_MS) return 0;

    int8_t dx = 0, dy = 0;
    if (dir == N || dir == NE || dir == NW) dy = -1;
    if (dir == S || dir == SE || dir == SW) dy = 1;
    if (dir == E || dir == NE || dir == SE) dx = 1;
    if (dir == W || dir == NW || dir == SW) dx = -1;

    if (dx == 0 && dy == 0) return 0;

    int8_t nx = player.x + dx, ny = player.y + dy;
    uint8_t target = map_get(nx, ny);

    if (target == TILE_WALL || target == TILE_GEODE) return 0;

    // If the player bumps into a bat, return 5 to indicate poisoning
    if (bat_at(nx, ny)) return 5; 

    player.x = nx;
    player.y = ny;
    player.last_move_tick = now;
    player.anim_frame ^= 1;

    if (target == TILE_ORE_CU) {
        ore_add(0);
        map_set(nx, ny, TILE_FLOOR);
        return 2;
    }
    if (target == TILE_ORE_AU) {
        ore_add(1);
        map_set(nx, ny, TILE_FLOOR);
        return 3;
    }
    if (target == TILE_ITEM_DIAMOND) {
        ore_add(2);
        map_set(nx, ny, TILE_FLOOR);
        return 4;  // picked up dropped diamond
    }

    return 1;
}

uint8_t player_mine(uint32_t now) {
    player.last_mine_tick = now;
    int8_t dirs[4][2] = {{0,-1},{0,1},{-1,0},{1,0}};

    for (int i = 0; i < 4; i++) {
        int8_t mx = player.x + dirs[i][0];
        int8_t my = player.y + dirs[i][1];
        uint8_t t = map_get(mx, my);

        if (t == TILE_ORE_CU) {
            map_set(mx, my, TILE_FLOOR);
            ore_add(0);
            return 0;
        }
        if (t == TILE_ORE_AU) {
            map_set(mx, my, TILE_FLOOR);
            ore_add(1);
            return 1;
        }
        if (t == TILE_GEODE) {
            map_set(mx, my, TILE_FLOOR);
            ore_add(2);
            return 2;
        }
        if (t == TILE_WALL) {
            map_set(mx, my, TILE_FLOOR);
            player.torch_ticks -= PICKAXE_TORCH_COST;
            if (player.torch_ticks < 0) player.torch_ticks = 0;
            return 3;
        }
    }
    return 0xFF;
}

uint8_t player_is_mining(uint32_t now) {
    if (player.last_mine_tick == 0) return 0;
    return (now - player.last_mine_tick) < MINE_ANIM_MS;
}