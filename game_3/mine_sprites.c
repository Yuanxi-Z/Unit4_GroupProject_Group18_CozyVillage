// mine_sprites.c - All sprite pixel data for Cozy Miner.
// Each byte is a palette index (see mine_palette.h) or __ (PX_TRANSPARENT).
// This file is pure const data - no logic.

#include "mine_sprites.h"
#include "mine_palette.h"

// Short aliases for readability (undef'd at bottom of file)
#define __ PX_TRANSPARENT

#define BL CLR_BLACK
#define IK CLR_INK
#define WD CLR_WOOD_DARK
#define WM CLR_WOOD_MID
#define WL CLR_WOOD_LIGHT
#define PR CLR_PARCH
#define CR CLR_CREAM

#define WS CLR_WALL_SHADOW
#define WH CLR_WALL_HI

#define CD CLR_COPPER_DARK
#define CU CLR_COPPER
#define CH CLR_COPPER_HI
#define GK CLR_GOLD_DARK
#define GD CLR_GOLD
#define GH CLR_GOLD_HI
#define DD CLR_DIAMOND_DARK
#define DI CLR_DIAMOND
#define DH CLR_DIAMOND_HI

#define SK CLR_SKIN
#define SS CLR_SKIN_SHADE
#define HD CLR_HAIR_DARK
#define HR CLR_HAIR
#define OV CLR_OVERALL
#define OH CLR_OVERALL_HI
#define UN CLR_UNDER
#define DN CLR_DENIM
#define HY CLR_HAT
#define HS CLR_HAT_SHINE
#define HB CLR_HAT_BRIM
#define BT CLR_BOOT
#define BU CLR_BLUSH
#define MT CLR_MOUTH

// =============================================================================
// MINER GIRL - 14x14, 4 animation frames
// =============================================================================

const uint8_t SPR_GIRL_IDLE[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,__,__,__,
    __,__,__,HY,HY,HS,HY,HY,HY,HY,HY,__,__,__,
    __,__,HB,HB,HB,HB,HB,HB,HB,HB,HB,HB,__,__,
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,HR,HR,__,SK,SK,SK,SK,SK,SK,__,HR,HR,__,
    __,HR,HR,SK,IK,SK,SK,SK,SK,IK,SK,HR,HR,__,
    __,HR,HR,BU,SK,SK,MT,MT,SK,SK,BU,HR,HR,__,
    __,HR,OV,SK,SK,SK,SK,SK,SK,SK,SK,OV,HR,__,
    __,__,OV,OV,UN,UN,UN,UN,UN,UN,OV,OV,__,__,
    __,__,OV,OV,OH,UN,OH,OH,UN,OH,OV,OV,__,__,
    __,__,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,__,__,
    __,SK,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,SK,__,
    __,__,__,__,DN,DN,__,__,DN,DN,__,__,__,__,
    __,__,__,BT,BT,BT,__,__,BT,BT,BT,__,__,__,
};

const uint8_t SPR_GIRL_WALK_A[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,__,__,__,
    __,__,__,HY,HY,HS,HY,HY,HY,HY,HY,__,__,__,
    __,__,HB,HB,HB,HB,HB,HB,HB,HB,HB,HB,__,__,
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,HR,HR,__,SK,SK,SK,SK,SK,SK,__,HR,HR,__,
    __,HR,HR,SK,IK,SK,SK,SK,SK,IK,SK,HR,HR,__,
    __,HR,HR,BU,SK,SK,MT,MT,SK,SK,BU,HR,HR,__,
    __,HR,OV,SK,SK,SK,SK,SK,SK,SK,SK,OV,HR,__,
    __,__,OV,OV,UN,UN,UN,UN,UN,UN,OV,OV,__,__,
    __,__,OV,OV,OH,UN,OH,OH,UN,OH,OV,OV,__,__,
    __,__,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,__,__,
    __,SK,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,SK,__,
    __,__,__,DN,DN,DN,__,__,__,DN,DN,__,__,__,
    __,__,BT,BT,BT,BT,__,__,__,BT,BT,BT,__,__,
};

const uint8_t SPR_GIRL_WALK_B[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,__,__,__,
    __,__,__,HY,HY,HS,HY,HY,HY,HY,HY,__,__,__,
    __,__,HB,HB,HB,HB,HB,HB,HB,HB,HB,HB,__,__,
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,HR,HR,__,SK,SK,SK,SK,SK,SK,__,HR,HR,__,
    __,HR,HR,SK,IK,SK,SK,SK,SK,IK,SK,HR,HR,__,
    __,HR,HR,BU,SK,SK,MT,MT,SK,SK,BU,HR,HR,__,
    __,HR,OV,SK,SK,SK,SK,SK,SK,SK,SK,OV,HR,__,
    __,__,OV,OV,UN,UN,UN,UN,UN,UN,OV,OV,__,__,
    __,__,OV,OV,OH,UN,OH,OH,UN,OH,OV,OV,__,__,
    __,__,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,__,__,
    __,SK,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,SK,__,
    __,__,__,DN,DN,__,__,__,DN,DN,DN,__,__,__,
    __,__,__,BT,BT,BT,__,__,BT,BT,BT,BT,__,__,
};

const uint8_t SPR_GIRL_MINE[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,WH,WH,__,
    __,__,__,HY,HY,HS,HY,HY,HY,HY,HY,WH,WH,__,
    __,__,HB,HB,HB,HB,HB,HB,HB,HB,HB,HB,WH,__,
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,WM,__,__,
    __,HR,HR,__,SK,SK,SK,SK,SK,SK,WM,__,HR,__,
    __,HR,HR,SK,IK,IK,SK,SK,IK,IK,SK,__,HR,__,
    __,HR,HR,BU,SK,SK,IK,IK,SK,SK,BU,HR,HR,__,
    __,HR,OV,SK,SK,SK,SK,SK,SK,SK,SK,OV,HR,__,
    __,__,OV,OV,UN,UN,UN,UN,UN,UN,OV,OV,__,__,
    __,__,OV,OV,OH,UN,OH,OH,UN,OH,OV,OV,__,__,
    __,__,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,__,__,
    __,SK,OV,OV,OV,OV,OV,OV,OV,OV,OV,OV,SK,__,
    __,__,__,__,DN,DN,__,__,DN,DN,__,__,__,__,
    __,__,__,BT,BT,BT,__,__,BT,BT,BT,__,__,__,
};

// =============================================================================
// WALL - 4 variants
// =============================================================================

const uint8_t SPR_WALL_A[14 * 14] = {
    WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,WS,
    WS,WH,WS,WS,IK,WS,WS,WS,WH,WS,IK,WS,WS,WS,
    WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,IK,WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,IK,
    WS,WS,IK,WS,WH,WS,WS,WH,WS,WS,WS,WS,WS,IK,
    WS,WS,IK,WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,IK,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,
    WS,WS,WH,WS,WS,IK,WS,WS,WS,WH,WS,IK,WS,WS,
    WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,IK,WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,IK,
    WS,WS,IK,WS,WS,WS,WH,WS,WS,WS,WS,WS,WS,IK,
};

const uint8_t SPR_WALL_B[14 * 14] = {
    WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,
    WS,WH,WH,IK,WS,WS,WS,WH,WS,IK,WS,WS,WH,WS,
    WS,WH,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,WS,WS,
    WS,WH,WS,WS,WS,IK,WS,WH,WH,WS,WS,WS,WS,WS,
    WS,WS,WS,WS,WS,IK,WS,WH,WS,WS,WS,WS,WH,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,IK,WS,WS,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,
    WS,IK,WS,WH,WS,WS,WS,WS,WS,IK,WS,WS,WS,WH,
    WS,IK,WS,WS,WS,WS,WH,WS,WS,IK,WS,WS,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,WS,
    WS,WH,WS,WS,IK,WS,WS,WS,WH,WS,IK,WS,WS,WS,
};

const uint8_t SPR_WALL_C[14 * 14] = {
    WS,WS,IK,WS,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,
    WS,WS,IK,WS,WH,WS,WS,WS,WS,IK,WS,WS,WS,WH,
    WS,WS,IK,WS,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,WH,WS,
    WH,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,WS,WS,
    WS,WS,WS,WH,WS,IK,WS,WS,WS,WH,WS,WS,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,
    WS,WH,WS,WS,WS,WS,WS,IK,WS,WS,WH,WS,WS,WS,
    WS,WS,WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,IK,WS,WS,WS,
    WS,WS,WS,IK,WS,WH,WS,WS,WH,WS,IK,WS,WS,WS,
};

const uint8_t SPR_WALL_D[14 * 14] = {
    WS,WH,WS,WS,IK,WS,WS,WH,WS,WS,IK,WS,WH,WS,
    WH,WS,WS,WS,IK,WH,WS,WS,WS,WS,IK,WS,WS,WS,
    WS,WS,WS,WH,IK,WS,WS,WS,WH,WS,IK,WS,WS,WH,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,IK,WS,WS,WS,WS,WH,WS,WS,WS,WS,WS,WS,IK,
    WH,IK,WS,WH,WS,WS,WS,WS,WS,WS,WH,WS,WS,IK,
    WS,IK,WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,WS,IK,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WH,WS,WS,IK,WS,WS,WS,WH,WS,IK,WS,WS,
    WS,WS,WS,WS,WH,IK,WS,WH,WS,WS,WS,IK,WH,WS,
    WH,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,IK,WS,WS,
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    WS,WS,WS,WS,WS,IK,WS,WS,WS,WS,WS,WS,WS,IK,
    WS,WH,WS,WS,WS,IK,WH,WS,WS,WS,WH,WS,WS,IK,
};

// =============================================================================
// FLOOR - 2 variants
// =============================================================================

const uint8_t SPR_FLOOR_A[14 * 14] = {
    WM,WM,WM,WL,WL,WM,WM,WM,WM,WD,WM,WM,WM,WM,
    WM,WM,WL,WL,WM,WM,WM,WD,WD,WM,WM,WM,WM,WM,
    WM,WM,WM,WM,WM,WM,WD,WM,WM,WM,WM,WL,WM,WM,
    WM,WL,WM,WM,WM,WM,WM,WM,WM,WM,WL,WL,WM,WM,
    WM,WL,WL,WM,WM,WD,WM,WM,WM,WM,WM,WM,WM,WM,
    WM,WM,WM,WM,WD,BL,WD,WM,WM,WM,WM,WM,WD,WM,
    WM,WM,WM,WM,WM,WD,WM,WM,WL,WL,WM,WM,WM,WM,
    WM,WM,WD,WM,WM,WM,WM,WL,WL,WM,WM,WM,WM,WM,
    WM,WM,WM,WM,WM,WM,WM,WM,WM,WM,WM,WD,WD,WM,
    WL,WM,WM,WL,WL,WM,WM,WM,WM,WD,WM,WM,WM,WM,
    WL,WL,WM,WM,WM,WM,WD,WD,WM,WM,WM,WM,WL,WM,
    WM,WM,WM,WM,WM,WM,WM,WM,WM,WM,WL,WL,WL,WM,
    WM,WM,WM,WD,WM,WM,WM,WL,WM,WM,WM,WM,WM,WM,
    WM,WM,WD,WD,WM,WM,WM,WM,WM,WM,WM,WM,WD,WM,
};

const uint8_t SPR_FLOOR_B[14 * 14] = {
    WM,WM,WD,WM,WM,WM,WM,WM,WL,WL,WM,WM,WM,WM,
    WM,WD,WD,WM,WM,WM,WM,WL,WL,WM,WM,WM,WM,WM,
    WM,WM,WM,WM,WL,WL,WM,WM,WM,WM,WM,WM,WD,WD,
    WM,WM,WM,WL,WL,WM,WM,WM,WM,WD,WM,WM,WD,WM,
    WL,WM,WM,WM,WM,WM,WM,WD,BL,WD,WM,WM,WM,WM,
    WL,WM,WM,WM,WM,WM,WM,WM,WD,WM,WM,WL,WM,WM,
    WM,WM,WM,WL,WM,WM,WM,WM,WM,WM,WL,WL,WM,WM,
    WM,WD,WD,WM,WM,WM,WD,WM,WM,WM,WM,WM,WM,WM,
    WM,WM,WM,WM,WD,WD,WM,WM,WM,WL,WM,WM,WM,WM,
    WM,WM,WM,WM,WM,WM,WM,WL,WL,WM,WM,WM,WM,WD,
    WM,WM,WM,WM,WL,WL,WM,WM,WM,WM,WM,WD,WD,WM,
    WM,WM,WM,WL,WL,WM,WM,WM,WM,WM,WM,WM,WM,WM,
    WD,WM,WM,WM,WM,WM,WM,WM,WM,WD,WM,WM,WM,WM,
    WD,WD,WM,WM,WM,WM,WM,WM,WD,WD,WM,WM,WM,WL,
};

// =============================================================================
// ORES - 14x14
// =============================================================================

const uint8_t SPR_ORE_COPPER[14 * 14] = {
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,CU,CU,CU,CU,CU,CU,__,__,__,__,
    __,__,__,CU,CU,CH,CH,CH,CH,CU,CU,__,__,__,
    __,__,CD,CU,CU,CH,CR,CH,CU,CU,CU,CD,__,__,
    __,__,CD,CU,CH,CU,CU,CU,CU,CH,CU,CD,__,__,
    __,__,CD,CU,CU,CU,CH,CU,CU,CU,CU,CD,__,__,
    __,__,CD,CU,CU,CH,CU,CU,CU,CU,CU,CD,__,__,
    __,__,__,CD,CU,CU,CU,CU,CU,CU,CD,__,__,__,
    __,__,__,__,CD,CU,CU,CU,CU,CD,__,__,__,__,
    __,__,__,__,__,CD,CD,CD,CD,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
};

const uint8_t SPR_ORE_GOLD[14 * 14] = {
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,GD,GD,GD,GD,__,__,__,__,__,
    __,__,__,__,GD,GD,GH,GH,GD,GD,__,__,__,__,
    __,__,__,GD,GD,GH,GH,GH,GH,GD,GD,__,__,__,
    __,__,GK,GD,GH,GD,GD,GD,GD,GH,GD,GK,__,__,
    __,__,GK,GD,GD,GH,GD,GD,GH,GD,GD,GK,__,__,
    __,__,GK,GD,GD,GD,GH,GH,GD,GD,GD,GK,__,__,
    __,__,__,GK,GD,GD,GD,GD,GD,GD,GK,__,__,__,
    __,__,__,__,GK,GD,GD,GD,GD,GK,__,__,__,__,
    __,__,__,__,__,GK,GD,GD,GK,__,__,__,__,__,
    __,__,__,__,__,__,GK,GK,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
};

const uint8_t SPR_ORE_DIAMOND[14 * 14] = {
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,DI,DI,__,__,__,__,__,__,
    __,__,__,__,__,DI,DH,DH,DI,__,__,__,__,__,
    __,__,__,__,DI,DH,DH,DI,DI,DD,__,__,__,__,
    __,__,__,DI,DH,DI,DI,DI,DI,DD,DD,__,__,__,
    __,__,DI,DI,DI,DI,DI,DI,DI,DI,DD,DD,__,__,
    __,__,DI,DI,DI,DI,DH,DI,DI,DI,DI,DD,__,__,
    __,__,__,DI,DI,DI,DI,DI,DI,DI,DD,__,__,__,
    __,__,__,__,DI,DI,DI,DI,DI,DD,__,__,__,__,
    __,__,__,__,__,DI,DI,DI,DD,__,__,__,__,__,
    __,__,__,__,__,__,DI,DD,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
};

// =============================================================================
// EXIT STAIRS - wooden staircase
// =============================================================================

const uint8_t SPR_EXIT_STAIRS[14 * 14] = {
    IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,IK,
    IK,WD,WM,WM,WM,WM,WM,WM,WM,WM,WM,WM,WD,IK,
    IK,WD,WM,WL,WM,WM,WM,WM,WM,WM,WL,WM,WD,IK,
    IK,WD,WD,WD,WD,WD,WD,WD,WD,WD,WD,WD,WD,IK,
    IK,IK,WM,WM,WM,WM,WM,WM,WM,WM,WM,WM,IK,IK,
    IK,IK,WD,WD,WD,WD,WD,WD,WD,WD,WD,WD,IK,IK,
    IK,IK,IK,WM,WM,WM,WL,WM,WM,WM,WM,IK,IK,IK,
    IK,IK,IK,WD,WD,WD,WD,WD,WD,WD,WD,IK,IK,IK,
    IK,IK,IK,IK,WM,WM,WM,WM,WM,WM,IK,IK,IK,IK,
    IK,IK,IK,IK,WD,WD,WD,WD,WD,WD,IK,IK,IK,IK,
    IK,IK,IK,IK,IK,WM,WM,WM,WM,IK,IK,IK,IK,IK,
    IK,IK,IK,IK,IK,WD,WD,WD,WD,IK,IK,IK,IK,IK,
    IK,IK,IK,IK,IK,IK,BL,BL,IK,IK,IK,IK,IK,IK,
    IK,IK,IK,IK,IK,IK,BL,BL,IK,IK,IK,IK,IK,IK,
};

// =============================================================================
// HUD ICONS - 10x10 for the bottom status bar
// =============================================================================

const uint8_t ICO_TORCH[10 * 10] = {
    __,__,__,CR,GD,CR,__,__,__,__,
    __,__,CR,CU,GD,CU,CR,__,__,__,
    __,__,CU,CH,GD,GD,CU,__,__,__,
    __,__,__,CU,CU,CU,__,__,__,__,
    __,__,__,__,WD,__,__,__,__,__,
    __,__,__,WM,WD,WM,__,__,__,__,
    __,__,__,WM,WM,WM,__,__,__,__,
    __,__,__,WD,WM,WD,__,__,__,__,
    __,__,__,WM,WD,WM,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
};

const uint8_t ICO_COPPER[10 * 10] = {
    __,__,__,WH,WH,WH,__,__,__,__,
    __,__,WH,CU,CH,CU,WH,__,__,__,
    __,WH,CU,CH,CH,CH,CU,WH,__,__,
    __,WH,CU,CU,CH,CU,CU,WH,__,__,
    __,WH,CU,CU,CU,CU,CU,WH,__,__,
    __,__,WH,CU,CU,CU,WH,__,__,__,
    __,__,__,WH,WH,WH,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
};

const uint8_t ICO_GOLD[10 * 10] = {
    __,__,__,WH,WH,WH,__,__,__,__,
    __,__,WH,GD,GH,GD,WH,__,__,__,
    __,WH,GD,GH,GH,GH,GD,WH,__,__,
    __,WH,GH,GD,GH,GD,GH,WH,__,__,
    __,WH,GD,GH,GD,GH,GD,WH,__,__,
    __,__,WH,GD,GD,GD,WH,__,__,__,
    __,__,__,WH,WH,WH,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
};

const uint8_t ICO_DIAMOND[10 * 10] = {
    __,__,__,DI,DI,__,__,__,__,__,
    __,__,DI,DH,DH,DI,__,__,__,__,
    __,DI,DH,DI,DI,DH,DI,__,__,__,
    DI,DH,DI,DI,DH,DI,DI,DI,__,__,
    __,DI,DI,DH,DI,DI,DI,__,__,__,
    __,__,DI,DI,DI,DI,__,__,__,__,
    __,__,__,DI,DI,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
};

const uint8_t ICO_COIN[10 * 10] = {
    __,__,GK,GD,GD,GD,GK,__,__,__,
    __,GK,GD,GH,GH,GH,GD,GK,__,__,
    __,GD,GH,GD,GD,GH,GD,GD,__,__,
    __,GD,GH,GD,GH,GH,GD,GD,__,__,
    __,GD,GH,GD,GD,GH,GD,GD,__,__,
    __,GD,GH,GH,GH,GH,GD,GD,__,__,
    __,GK,GD,GD,GD,GD,GK,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,
};

// =============================================================================
// ITEM DRAWER ICONS - 14x14, scaled up in UI
// =============================================================================

const uint8_t ICO_ITEM_TORCH[14 * 14] = {
    __,__,__,__,__,CR,GD,CR,__,__,__,__,__,__,
    __,__,__,__,CR,GD,CU,GD,CR,__,__,__,__,__,
    __,__,__,__,CU,CU,GD,CU,CU,__,__,__,__,__,
    __,__,__,__,__,CU,CU,CU,__,__,__,__,__,__,
    __,__,__,__,__,WD,WD,WD,__,__,__,__,__,__,
    __,__,__,__,__,WM,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WD,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WM,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WD,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WM,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WD,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WM,WM,WM,__,__,__,__,__,__,
    __,__,__,__,__,WD,WD,WD,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
};

const uint8_t ICO_ITEM_DYNAMITE[14 * 14] = {
    __,__,__,__,__,__,IK,__,__,__,__,__,__,__,
    __,__,__,__,__,__,IK,__,__,__,__,__,__,__,
    __,__,__,__,__,IK,IK,IK,__,__,__,__,__,__,
    __,__,__,__,OV,OV,OV,OV,OV,__,__,__,__,__,
    __,__,__,__,OV,OV,OH,OV,OV,__,__,__,__,__,
    __,__,__,__,OV,OH,OV,OV,OV,__,__,__,__,__,
    __,__,__,__,OV,OV,OV,OH,OV,__,__,__,__,__,
    __,__,__,__,OV,OH,OV,OV,OV,__,__,__,__,__,
    __,__,__,__,OV,OV,OV,OH,OV,__,__,__,__,__,
    __,__,__,__,OV,OH,OV,OV,OV,__,__,__,__,__,
    __,__,__,__,OV,OV,OV,OH,OV,__,__,__,__,__,
    __,__,__,__,OV,OH,OV,OV,OV,__,__,__,__,__,
    __,__,__,__,OV,OV,OV,OV,OV,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,
};

const uint8_t ICO_ITEM_POTION[14 * 14] = {
    __,__,__,__,__,WD,WM,WD,__,__,__,__,__,__,
    __,__,__,__,__,WD,WM,WD,__,__,__,__,__,__,
    __,__,__,__,IK,IK,IK,IK,IK,__,__,__,__,__,
    __,__,__,IK,CR,CR,IK,CR,CR,IK,__,__,__,__,
    __,__,IK,CR,CR,CR,CR,CR,CR,CR,IK,__,__,__,
    __,IK,CR,OV,OV,OV,OV,OV,OV,CR,CR,IK,__,__,
    __,IK,CR,OV,OH,OV,OV,OV,OV,OV,CR,IK,__,__,
    __,IK,CR,OV,OV,OV,OH,OV,OV,OV,CR,IK,__,__,
    __,IK,CR,OV,OH,OV,OV,OV,OH,OV,CR,IK,__,__,
    __,IK,CR,OV,OV,OV,OH,OV,OV,OV,CR,IK,__,__,
    __,IK,CR,OV,OH,OV,OV,OV,OV,OV,CR,IK,__,__,
    __,IK,CR,CR,OV,OV,OV,OV,OV,CR,CR,IK,__,__,
    __,__,IK,CR,CR,CR,CR,CR,CR,CR,IK,__,__,__,
    __,__,__,IK,IK,IK,IK,IK,IK,IK,__,__,__,__,
};

// =============================================================================
// HOME SCENE SPRITES
// =============================================================================

// Sleeping girl tucked under the blanket
const uint8_t SPR_GIRL_SLEEP[10 * 8] = {
    __,HR,HR,HR,HR,HR,HR,HR,HR,__,
    HR,HR,HR,HR,HR,HR,HR,HR,HR,HR,
    HR,SK,SK,SK,SK,SK,SK,SK,SK,HR,
    HR,BU,IK,IK,SK,IK,IK,SK,BU,HR,
    __,HR,SK,SK,MT,MT,SK,SK,HR,__,
    __,__,SK,SK,SK,SK,SK,SK,__,__,
    __,OV,OV,OV,OV,OV,OV,OV,OV,__,
    OV,OV,UN,OV,OV,OV,UN,OV,OV,OV,
};

// Redesigned curled up orange cat (24x12)
const uint8_t SPR_CAT_SLEEP[24 * 12] = {
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
    __,__,__,__,CU,CU,__,__,__,__,__,__,__,__,__,__,__,__,CU,CU,__,__,__,__,
    __,__,__,CU,CH,CU,__,__,__,__,__,__,__,__,__,__,__,CU,CH,CU,__,__,__,__,
    __,__,CU,CH,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CH,CU,__,__,__,
    __,CU,CH,CU,CU,CU,CH,CU,CU,CH,CU,CU,CH,CU,CU,CH,CU,CU,CU,CU,CH,CU,__,__,
    __,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,IK,CU,IK,CU,CU,__,__,
    __,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CR,CU,CU,CU,__,__,
    __,__,CU,CH,CU,CU,CH,CU,CU,CH,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,__,__,__,
    __,__,__,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,__,__,__,__,
    __,__,__,__,__,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,CU,__,__,__,__,__,__,__,
    __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,
};

// =============================================================================
// SHOPKEEPER - 14x14, 3 expressions (Redesigned with cute dot eyes + eyebrows)
// =============================================================================

#define AP 3     /* CLR_GRASS = green apron */

const uint8_t SPR_SHOPKEEPER_IDLE[14 * 14] = {
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,__,HR,HR,HR,HR,HR,HR,HR,HR,HR,HR,__,__,
    __,HR,HR,SK,SK,SK,SK,SK,SK,SK,SK,HR,HR,__,
    __,HR,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,HR,__, // Short curved brows
    __,HR,SK,SK,IK,IK,SK,SK,IK,IK,SK,SK,HR,__,
    __,__,SK,SK,SK,IK,SK,SK,IK,SK,SK,SK,__,__, // Cute dot eyes
    __,__,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,__,__, // Warm mustache
    __,__,SK,SK,SK,MT,MT,MT,MT,SK,SK,SK,__,__,
    __,__,__,SK,SK,SK,SK,SK,SK,SK,SK,__,__,__, // Chin
    __,__,__,__,SK,SK,SK,SK,SK,SK,__,__,__,__,
    __,__,__,AP,AP,CR,CR,CR,CR,AP,AP,__,__,__,
    __,__,AP,AP,AP,AP,AP,AP,AP,AP,AP,AP,__,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
};

const uint8_t SPR_SHOPKEEPER_HAPPY[14 * 14] = {
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,__,HR,HR,HR,HR,HR,HR,HR,HR,HR,HR,__,__,
    __,HR,HR,SK,SK,SK,SK,SK,SK,SK,SK,HR,HR,__,
    __,HR,SK,SK,IK,SK,SK,SK,SK,IK,SK,SK,HR,__, // Brows raised high (off screen)
    __,HR,SK,IK,SK,IK,SK,SK,IK,SK,IK,SK,HR,__, // Happy ^ ^ eyes
    __,__,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,__,__,
    __,BU,SK,SK,MT,MT,MT,MT,MT,MT,SK,SK,BU,__, // Wide smile mustache
    __,__,SK,SK,MT,CR,CR,CR,CR,MT,SK,SK,__,__,
    __,__,__,SK,SK,MT,MT,MT,MT,SK,SK,__,__,__,
    __,__,__,__,SK,SK,SK,SK,SK,SK,__,__,__,__,
    __,__,__,AP,AP,CR,CR,CR,CR,AP,AP,__,__,__,
    __,__,AP,AP,AP,AP,AP,AP,AP,AP,AP,AP,__,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
};

const uint8_t SPR_SHOPKEEPER_WORRY[14 * 14] = {
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,__,HR,HR,HR,HR,HR,HR,HR,HR,HR,HR,__,__,
    __,HR,HR,SK,SK,IK,SK,SK,IK,SK,SK,HR,HR,__, // Worried / \ brows
    __,HR,SK,SK,IK,SK,SK,SK,SK,IK,SK,SK,HR,__,
    __,HR,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,HR,__,
    __,__,SK,SK,IK,SK,SK,SK,SK,IK,SK,SK,__,__, // Eyes
    __,__,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,__,__,
    __,__,SK,MT,MT,MT,SK,SK,MT,MT,MT,SK,__,__, // Drooping mustache
    __,__,__,SK,SK,MT,MT,MT,MT,SK,SK,__,__,__,
    __,__,__,__,SK,SK,SK,SK,SK,SK,__,__,__,__,
    __,__,__,AP,AP,CR,CR,CR,CR,AP,AP,__,__,__,
    __,__,AP,AP,AP,AP,AP,AP,AP,AP,AP,AP,__,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
};

#undef AP

// =============================================================================
// Bat enemy - 14x14, two frames for wing flap animation
// Palette: 0 black (body/outline), 8 dusk_purple (wings),
//          2 soft_red (eyes), 1 cream (eye whites/fangs)
// =============================================================================

const uint8_t SPR_BAT_A[14 * 14] = {
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, BL, BL, __, __, __, __, __, __, __, __, BL, BL, __,
    __, BL, WS, WS, __, __, __, __, __, __, WS, WS, BL, __,
    __, BL, WS, WS, BL, __, __, __, __, BL, WS, WS, BL, __,
    __, BL, WS, WS, BL, __, BL, BL, __, BL, WS, WS, BL, __,
    __, BL, WS, WS, BL, BL, MT, MT, BL, BL, WS, WS, BL, __,
    __, BL, WS, WS, BL, MT, CR, CR, MT, BL, WS, WS, BL, __,
    __, __, BL, BL, BL, BL, MT, MT, BL, BL, BL, BL, __, __,
    __, __, __, __, __, BL, BL, BL, BL, __, __, __, __, __,
    __, __, __, __, __, __, BL, BL, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
};

const uint8_t SPR_BAT_B[14 * 14] = {
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, __, __, __, __, BL, BL, __, __, __, __, __, __, __,
    __, __, __, __, BL, WS, WS, BL, __, __, __, __, __, __,
    __, __, __, BL, WS, WS, WS, WS, BL, __, __, __, __, __,
    __, __, BL, WS, WS, WS, WS, WS, WS, BL, __, __, __, __,
    __, BL, WS, WS, WS, BL, BL, WS, WS, WS, BL, __, __, __,
    __, BL, WS, WS, BL, MT, MT, BL, WS, WS, BL, __, __, __,
    __, __, BL, WS, MT, CR, CR, MT, WS, BL, __, __, __, __,
    __, __, __, BL, BL, MT, MT, BL, __, __, __, __, __, __,
    __, __, __, __, BL, BL, BL, BL, __, __, __, __, __, __,
    __, __, __, __, __, BL, BL, __, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
};

// Clean up local aliases
#undef __
#undef BL
#undef IK
#undef WD
#undef WM
#undef WL
#undef PR
#undef CR
#undef WS
#undef WH
#undef CD
#undef CU
#undef CH
#undef GK
#undef GD
#undef GH
#undef DD
#undef DI
#undef DH
#undef SK
#undef SS
#undef HD
#undef HR
#undef OV
#undef OH
#undef UN
#undef DN
#undef HY
#undef HS
#undef HB
#undef BT
#undef BU
#undef MT