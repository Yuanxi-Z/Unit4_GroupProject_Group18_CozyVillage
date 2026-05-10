#ifndef MINE_RENDER_H
#define MINE_RENDER_H

#include <stdint.h>

// =============================================================================
// Render - high-level draw functions called by Game_3.c FSM dispatcher
// -----------------------------------------------------------------------------
// LAYOUT v3 (big-tile, single-screen):
//   - Tiles: 14x14 sprite source drawn at scale 2 -> 28x28 on the 240x240 LCD.
//   - Map grid: 8 cols x 6 rows, fully visible at once (no camera scroll).
//   - Torch bar: 24 px tall, scale-2 label + bar + scale-2 countdown number.
//   - HUD: 48 px tall, scale-2 icons (10x10 -> 20x20) + scale-2 numbers.
//   - Total: 168 (map) + 24 (torch) + 48 (HUD) = 240.
//   - Right edge dead space: 240 - 8*28 = 16 px (filled with HUD_BG).
// =============================================================================

#define MAP_AREA_TOP      0
#define MAP_AREA_BOTTOM   168
#define TORCH_BAR_TOP     168
#define TORCH_BAR_BOTTOM  192
#define HUD_TOP           192
#define HUD_BOTTOM        240
#define TILE_PX           28

// Main map + player render. Draws floor, walls, ores, exit, and player.
// Tiles outside visibility radius are drawn as solid black.
// @param now_ms  HAL_GetTick() for animations
void render_map(uint32_t now_ms);

// Bottom HUD + torch progress bar. Always call after render_map.
// @param now_ms for flashing animations
void render_hud(uint32_t now_ms);

// Overlay: items drawer at bottom (replaces HUD while open)
// @param selected_index  0=torch, 1=dynamite, 2=potion
// @param now_ms for selection pulse
void render_items_drawer(uint8_t selected_index, uint32_t now_ms);

void render_floor_transition_dialog(uint32_t now_ms);

// Full white flash for explosion
void render_explosion_flash(void);

#endif // MINE_RENDER_H