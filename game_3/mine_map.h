#ifndef MINE_MAP_H
#define MINE_MAP_H

#include <stdint.h>

// =============================================================================
// Mine Map - 8x6, two hand-designed floors
// -----------------------------------------------------------------------------
// Map size: 8x6 (was 16x16). With 28 px tiles the whole map is visible at
// once on the 240x240 LCD - no camera scroll, no exploration fog-of-war.
// Tiles, walls, and the player sprite all read much bigger and cozier.
//
// Floor 1 "Entry Vein":  spawn top-left clear corner, geode guarded by gold,
//                        stairs on the bottom-right edge.
// Floor 2 "Deep Heart":  tight maze with 2 bat spawns and no diamond.
// =============================================================================

#define MAP_W  8
#define MAP_H  6

#define FLOOR_COUNT  2
#define FLOOR_ENTRY  1
#define FLOOR_DEEP   2

// Tile types - values UNCHANGED (keeps render/player/audio code compatible)
#define TILE_FLOOR         0
#define TILE_WALL          1
#define TILE_ORE_CU        2
#define TILE_ORE_AU        3
#define TILE_EXIT          4
#define TILE_GEODE         5
#define TILE_ITEM_DIAMOND  6   // dropped diamond on ground (from dynamite)

// ---- Lifecycle ----
void map_generate(uint8_t floor);

// ---- Tile access ----
uint8_t map_get(int8_t x, int8_t y);
void    map_set(int8_t x, int8_t y, uint8_t tile);

// ---- Queries ----
int  map_nearest_geode_dist(int8_t px, int8_t py);
void map_explode(int8_t cx, int8_t cy, uint8_t *diamonds_found);

// ---- Spawn / stairs queries ----
void map_get_spawn(uint8_t floor, int8_t* out_x, int8_t* out_y);
void map_get_stairs(uint8_t floor, int8_t* out_x, int8_t* out_y);
uint8_t map_get_bat_spawns(uint8_t floor, int8_t out_pts[][2], uint8_t max_pts);

#endif // MINE_MAP_H