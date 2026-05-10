#ifndef MINE_SPRITES_H
#define MINE_SPRITES_H

#include <stdint.h>

// =============================================================================
// Sprite declarations
// -----------------------------------------------------------------------------
// All tile-sized sprites are 14x14 uint8_t arrays of palette indices.
// PX_TRANSPARENT (255) means "don't draw this pixel".
//
// Draw with:
//   LCD_Draw_Sprite_Scaled(x, y, 14, 14, (uint8_t*)SPR_XXX, 1)
//
// The scale=1 means no scaling (native 14x14 pixels).
// =============================================================================

// --- Miner girl (4 animation frames, 14x14) ---
extern const uint8_t SPR_GIRL_IDLE[14 * 14];
extern const uint8_t SPR_GIRL_WALK_A[14 * 14];
extern const uint8_t SPR_GIRL_WALK_B[14 * 14];
extern const uint8_t SPR_GIRL_MINE[14 * 14];

// --- Map tiles (14x14) ---
extern const uint8_t SPR_WALL_A[14 * 14];
extern const uint8_t SPR_WALL_B[14 * 14];
extern const uint8_t SPR_WALL_C[14 * 14];
extern const uint8_t SPR_WALL_D[14 * 14];
extern const uint8_t SPR_FLOOR_A[14 * 14];
extern const uint8_t SPR_FLOOR_B[14 * 14];
extern const uint8_t SPR_ORE_COPPER[14 * 14];
extern const uint8_t SPR_ORE_GOLD[14 * 14];
extern const uint8_t SPR_ORE_DIAMOND[14 * 14];   // diamond revealed (from cracked geode)
extern const uint8_t SPR_EXIT_STAIRS[14 * 14];

// --- HUD icons (10x10, smaller for the status bar) ---
extern const uint8_t ICO_TORCH[10 * 10];
extern const uint8_t ICO_COPPER[10 * 10];
extern const uint8_t ICO_GOLD[10 * 10];
extern const uint8_t ICO_DIAMOND[10 * 10];
extern const uint8_t ICO_COIN[10 * 10];

// --- Item drawer icons (14x14, scaled up in UI) ---
extern const uint8_t ICO_ITEM_TORCH[14 * 14];
extern const uint8_t ICO_ITEM_DYNAMITE[14 * 14];
extern const uint8_t ICO_ITEM_POTION[14 * 14];

// --- Home scene (sleeping girl on pillow, 10x8) ---
extern const uint8_t SPR_GIRL_SLEEP[10 * 8];

// --- Home scene (curled sleeping cat, 24x12) ---
extern const uint8_t SPR_CAT_SLEEP[24 * 12];

// --- Shopkeeper (14x14 square sprite, 3 expressions) ---
extern const uint8_t SPR_SHOPKEEPER_IDLE[14 * 14];
extern const uint8_t SPR_SHOPKEEPER_HAPPY[14 * 14];
extern const uint8_t SPR_SHOPKEEPER_WORRY[14 * 14];

// --- Bat enemy (2 animation frames, 14x14) ---
extern const uint8_t SPR_BAT_A[14 * 14];
extern const uint8_t SPR_BAT_B[14 * 14];

#endif // MINE_SPRITES_H