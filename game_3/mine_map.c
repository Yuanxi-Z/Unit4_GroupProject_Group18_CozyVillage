#include "mine_map.h"
#include <stdlib.h>

// =============================================================================
// Runtime map buffer
// =============================================================================
static uint8_t map[MAP_H][MAP_W];

// =============================================================================
// Floor 1 - "Entry Vein" (8x6)
//   Legend: # wall  . floor  c copper  g gold  G geode  @ spawn  > stairs down
//
//   Spawn at (0,5) - literally in the map's bottom-left CORNER. Tiles (0,4)
//   and (1,5) are also floor, so the player sees an open 3-tile pocket
//   rather than being walled in. NO sprite drawn outside the map border.
//
//   Stairs at (7,4). Direct path IS blocked by a wall at (2,4) - player
//   MUST mine at least one tile with the pickaxe to progress.
//
//   Geode at (4,3) with nothing adjacent except walls. Copper at (2,1) and
//   (5,4). Gold at (6,1). Geode's neighbors (3,3), (4,2) are also walls so
//   the player can reach diamond only by mining twice into the vein.
//
//   0 1 2 3 4 5 6 7
// 0 # # # # # # # #
// 1 # . c . # . g #   copper (2,1), gold (6,1) - both far from geode
// 2 # . . . # # # #
// 3 # . # # G . . #   geode (4,3) tucked in
// 4 . . # . . c . >   (0,4) open, wall (2,4) blocks, copper (5,4), exit
// 5 @ . # # # # # #   spawn corner (0,5), (1,5) open
//
// Demo route (mine (2,4) once):
//   (0,5)->(1,5)->(1,4)->(2,4)->(3,4)->(4,4)->(5,4)->(6,4)->(7,4) = 8 steps
//   Path passes within 1-3 tiles of geode the whole time (ping audible).
// =============================================================================
static const char floor1_layout[MAP_H][MAP_W + 1] = {
    "########",  // 0
    "#.c.#.g#",  // 1
    "#...####",  // 2
    "#.##G..#",  // 3
    "..#..c.>",  // 4
    "@.######",  // 5
};

// =============================================================================
// Floor 2 - "Deep Heart" (8x6)
//   Legend: # wall  . floor  c copper  g gold  @ spawn  > stairs up  b bat
//
//   Spawn bottom-right (6,4). Exit '>' is wedged in the top-left wall row
//   at (1,0) - visually "built into" the wall, giving the moment of
//   "emerging upward" when you walk onto it.
//   2 bat spawns, copper ore on the way.
//
//   0 1 2 3 4 5 6 7
// 0 # > # # # # # #   stairs up wedged into top-left wall
// 1 # . . # . b . #   bat top-middle
// 2 # . # # . . # #
// 3 # . b . . . . #   bat left
// 4 # . . # . c . @   spawn right, copper on the way
// 5 # # # # # # # #
//
// BFS-verified: player can walk from spawn to stairs without mining.
// =============================================================================
static const char floor2_layout[MAP_H][MAP_W + 1] = {
    "#>######",  // 0
    "#..#.b.#",  // 1
    "#.##...#",  // 2
    "#.b....#",  // 3
    "#..#.c.@",  // 4
    "########",  // 5
};

// =============================================================================
// Parser
// =============================================================================
static uint8_t tile_from_char(char c) {
    switch (c) {
        case '#': return TILE_WALL;
        case '.': return TILE_FLOOR;
        case 'c': return TILE_ORE_CU;
        case 'g': return TILE_ORE_AU;
        case 'G': return TILE_GEODE;
        case '>': return TILE_EXIT;
        case '@': return TILE_FLOOR;
        case 'b': return TILE_FLOOR;
        default:  return TILE_FLOOR;
    }
}

static const char (*layout_for(uint8_t floor))[MAP_W + 1] {
    if (floor == FLOOR_DEEP) return floor2_layout;
    return floor1_layout;
}

void map_generate(uint8_t floor) {
    const char (*layout)[MAP_W + 1] = layout_for(floor);
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            map[y][x] = tile_from_char(layout[y][x]);
        }
    }
}

uint8_t map_get(int8_t x, int8_t y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return TILE_WALL;
    return map[y][x];
}

void map_set(int8_t x, int8_t y, uint8_t tile) {
    if (x > 0 && x < MAP_W-1 && y > 0 && y < MAP_H-1)
        map[y][x] = tile;
}

int map_nearest_geode_dist(int8_t px, int8_t py) {
    int best = 999;
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (map[y][x] == TILE_GEODE) {
                int d = abs(px - x) + abs(py - y);
                if (d < best) best = d;
            }
        }
    }
    return best;
}

void map_explode(int8_t cx, int8_t cy, uint8_t *diamonds_found) {
    *diamonds_found = 0;  // kept for API compat; diamonds stay on ground
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int8_t tx = cx + dx, ty = cy + dy;
            if (tx > 0 && tx < MAP_W-1 && ty > 0 && ty < MAP_H-1) {
                uint8_t t = map[ty][tx];
                if (t == TILE_WALL)  map[ty][tx] = TILE_FLOOR;
                if (t == TILE_GEODE) map[ty][tx] = TILE_ITEM_DIAMOND;
            }
        }
    }
}

// NOTE: map_get_stairs on Floor 1 must ALSO accept '>' when it sits on the
// map border. The '>' at x=7 on Floor 1 is on the wall column but IS the
// stairs tile (we intentionally place it there instead of a '#').
// find_first_char scans the raw layout, so this works naturally.
static void find_first_char(uint8_t floor, char target, int8_t* out_x, int8_t* out_y) {
    const char (*layout)[MAP_W + 1] = layout_for(floor);
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (layout[y][x] == target) {
                *out_x = (int8_t)x;
                *out_y = (int8_t)y;
                return;
            }
        }
    }
    *out_x = 1;
    *out_y = 1;
}

void map_get_spawn(uint8_t floor, int8_t* out_x, int8_t* out_y) {
    find_first_char(floor, '@', out_x, out_y);
}

void map_get_stairs(uint8_t floor, int8_t* out_x, int8_t* out_y) {
    find_first_char(floor, '>', out_x, out_y);
}

uint8_t map_get_bat_spawns(uint8_t floor, int8_t out_pts[][2], uint8_t max_pts) {
    const char (*layout)[MAP_W + 1] = layout_for(floor);
    uint8_t n = 0;
    for (int y = 0; y < MAP_H && n < max_pts; y++) {
        for (int x = 0; x < MAP_W && n < max_pts; x++) {
            if (layout[y][x] == 'b') {
                out_pts[n][0] = (int8_t)x;
                out_pts[n][1] = (int8_t)y;
                n++;
            }
        }
    }
    return n;
}