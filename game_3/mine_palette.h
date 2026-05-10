#ifndef MINE_PALETTE_H
#define MINE_PALETTE_H

// =============================================================================
// Cozy Miner Palette (v2 - using PALETTE_CUSTOM)
// -----------------------------------------------------------------------------
// Color index -> custom palette entry:
//   0  WARM_BLACK      near-black with warmth
//   1  CREAM           off-white parchment
//   2  SOFT_RED        muted red (overalls, blanket)
//   3  LEAF_GREEN      real plant green
//   4  DENIM_BLUE      denim / sky
//   5  WARM_ORANGE     skin, copper, torch
//   6  AMBER           hardhat, hud yellow
//   7  SOFT_PINK       blush, pillow
//   8  DUSK_PURPLE     cave wall shadow
//   9  DARK_BROWN      boots, cave deep shadow, wood outline
//   10 HONEY_GOLD      HUD text, amber accent
//   11 SAGE_GREEN      plant highlight, moss
//   12 TRUE_BROWN      wood, hair, furniture body
//   13 STONE_GREY      cave stone
//   14 SKY_CYAN        diamond, sky
//   15 CORAL_ROSE      coral highlight, warm accent
// =============================================================================

#define PX_TRANSPARENT   255

// --- Raw color indices ---
#define CLR_BLACK         0
#define CLR_CREAM         1
#define CLR_SOFT_RED      2
#define CLR_LEAF_GREEN    3
#define CLR_DENIM_BLUE    4
#define CLR_WARM_ORANGE   5
#define CLR_AMBER         6
#define CLR_SOFT_PINK     7
#define CLR_DUSK_PURPLE   8
#define CLR_DARK_BROWN    9
#define CLR_HONEY_GOLD   10
#define CLR_SAGE         11
#define CLR_TRUE_BROWN   12
#define CLR_STONE_GREY   13
#define CLR_SKY_CYAN     14
#define CLR_CORAL        15

// --- Legacy aliases (keep existing sprite/render code working) ---
#define CLR_INK           CLR_DARK_BROWN
#define CLR_WALL_SHADOW   CLR_DUSK_PURPLE
#define CLR_WALL_HI       CLR_STONE_GREY
#define CLR_WOOD_DARK     CLR_DARK_BROWN
#define CLR_WOOD_MID      CLR_TRUE_BROWN
#define CLR_WOOD_LIGHT    CLR_HONEY_GOLD
#define CLR_PARCH         CLR_CREAM
#define CLR_SKY           CLR_SKY_CYAN
#define CLR_SUN           CLR_AMBER
#define CLR_GRASS         CLR_LEAF_GREEN

// Ore coloring
#define CLR_COPPER_DARK   CLR_DARK_BROWN
#define CLR_COPPER        CLR_WARM_ORANGE
#define CLR_COPPER_HI     CLR_AMBER
#define CLR_GOLD_DARK     CLR_TRUE_BROWN
#define CLR_GOLD          CLR_AMBER
#define CLR_GOLD_HI       CLR_CREAM
#define CLR_DIAMOND_DARK  CLR_DENIM_BLUE
#define CLR_DIAMOND       CLR_SKY_CYAN
#define CLR_DIAMOND_HI    CLR_CREAM

// Floor darkness ramp
#define CLR_FLOOR_FAR     CLR_BLACK
#define CLR_FLOOR_DIM     CLR_DARK_BROWN
#define CLR_FLOOR_MID     CLR_TRUE_BROWN
#define CLR_FLOOR_NEAR    CLR_WARM_ORANGE

// Character palette aliases
#define CLR_SKIN          CLR_WARM_ORANGE
#define CLR_SKIN_SHADE    CLR_TRUE_BROWN
#define CLR_HAIR_DARK     CLR_DARK_BROWN
#define CLR_HAIR          CLR_TRUE_BROWN
#define CLR_OVERALL       CLR_SOFT_RED
#define CLR_OVERALL_HI    CLR_AMBER
#define CLR_UNDER         CLR_CREAM
#define CLR_DENIM         CLR_DENIM_BLUE
#define CLR_HAT           CLR_AMBER
#define CLR_HAT_SHINE     CLR_CREAM
#define CLR_HAT_BRIM      CLR_WARM_ORANGE
#define CLR_BOOT          CLR_DARK_BROWN
#define CLR_BLUSH         CLR_SOFT_PINK
#define CLR_MOUTH         CLR_SOFT_RED

// UI / HUD palette aliases
#define CLR_HUD_BG        CLR_DENIM_BLUE      // Dark blue background
#define CLR_HUD_BORDER    CLR_HONEY_GOLD      // Gold border
#define CLR_HUD_TEXT      CLR_AMBER           // Bright yellow text
#define CLR_HUD_DIM       CLR_STONE_GREY      // Dim secondary text
#define CLR_SELECT_BG     CLR_AMBER
#define CLR_SELECT_EDGE   CLR_CORAL           // Coral highlight edge
#define CLR_OK            CLR_LEAF_GREEN
#define CLR_DANGER        CLR_SOFT_RED
#define CLR_COIN          CLR_HONEY_GOLD

#endif // MINE_PALETTE_H