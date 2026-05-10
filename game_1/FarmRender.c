#include "FarmRender.h"
#include "FarmConfig.h"
#include "FarmEffects.h"
#include "farm_palette.h"
#include "LCD.h"
#include "FarmSprites.h"
#include "farm_girl_sprite.h"
#include "farm_coin_sprite.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

/*
 * Draw text with a dark one-pixel shadow.
 * This improves readability on bright or detailed backgrounds.
 */
static void Farm_PrintBold(const char *str, uint16_t x, uint16_t y,
                           uint8_t colour, uint8_t size)
{
    LCD_printString(str, x + 1, y + 1, CLR_INK, size);
    LCD_printString(str, x, y, colour, size);
}

/*
 * Draw a small wooden-style panel.
 * The panel is used for compact UI elements such as the water preview bar.
 */
static void Farm_DrawPanel(int x, int y, int w, int h)
{
    LCD_Draw_Rect(x, y, w, h, CLR_WOOD_DARK, 1);
    LCD_Draw_Rect(x + 2, y + 2, w - 4, h - 4, CLR_PARCH, 1);
    LCD_Draw_Rect(x + 2, y + 2, w - 4, 1, CLR_WOOD_LIGHT, 1);
    LCD_Draw_Rect(x + 2, y + 2, 1, h - 4, CLR_WOOD_LIGHT, 1);
}

/*
 * Draw a simple selection highlight around a tile or UI object.
 */
static void Farm_DrawSelectHighlight(int x, int y, int w, int h)
{
    LCD_Draw_Rect(x, y, w, h, CLR_SELECT_EDGE, 0);
    LCD_Draw_Rect(x + 1, y + 1, w - 2, h - 2, CLR_SELECT_BG, 0);
}

/*
 * Draw a pulsing selection highlight.
 * This is used for the home and shop icons to make the selected target clearer.
 */
static void Farm_DrawAnimatedHighlight(int x, int y, int w, int h)
{
    uint32_t now = HAL_GetTick();
    uint8_t phase = (uint8_t)((now / 180U) % 4U);

    if (phase == 0U || phase == 2U) {
        LCD_Draw_Rect(x - 1, y - 1, w + 2, h + 2, CLR_SELECT_EDGE, 0);
        LCD_Draw_Rect(x,     y,     w,     h,     CLR_SELECT_BG,   0);
    } else {
        LCD_Draw_Rect(x - 2, y - 2, w + 4, h + 4, CLR_SELECT_EDGE, 0);
        LCD_Draw_Rect(x,     y,     w,     h,     CLR_SELECT_BG,   0);
        LCD_Draw_Rect(x + 1, y + 1, w - 2, h - 2, CLR_SELECT_EDGE, 0);
    }
}

/*
 * Draw a soft pixel-style cloud.
 */
static void Farm_DrawCloud(int x, int y)
{
    LCD_Draw_Circle(x + 6,  y + 6,  6, CLR_CREAM, 1);
    LCD_Draw_Circle(x + 14, y + 3,  8, CLR_CREAM, 1);
    LCD_Draw_Circle(x + 24, y + 6,  6, CLR_CREAM, 1);
    LCD_Draw_Rect  (x + 4,  y + 8, 20, 5, CLR_CREAM, 1);
}

/*
 * Draw the moon icon used in the night scene.
 */
static void Farm_DrawMoon(int x, int y)
{
    LCD_Draw_Circle(x, y, 8, CLR_HONEY_GOLD, 1);
}

/*
 * Generate a ping-pong animation offset.
 * The value moves from 0 to range, then back from range to 0.
 */
static int Farm_PingPongOffset(uint32_t now_ms, uint32_t one_way_ms, int range)
{
    uint32_t cycle = one_way_ms * 2U;
    uint32_t phase = now_ms % cycle;

    if (phase < one_way_ms) {
        return (int)((phase * (uint32_t)range) / one_way_ms);
    } else {
        uint32_t back = cycle - phase;
        return (int)((back * (uint32_t)range) / one_way_ms);
    }
}

/*
 * Draw a small sun/day icon for the HUD.
 */
static void Farm_DrawDayIcon(int x, int y)
{
    LCD_Draw_Circle(x + 5, y + 5, 3, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 5, y,     1, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 5, y + 8, 1, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x,     y + 5, 2, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 8, y + 5, 2, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 2, 1, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 7, y + 2, 1, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 7, 1, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 7, y + 7, 1, 1, CLR_HONEY_GOLD, 1);
}

/*
 * Draw a small battery-like energy icon for the HUD.
 */
static void Farm_DrawEnergyIcon(int x, int y)
{
    LCD_Draw_Rect(x + 1, y + 2, 8, 6, CLR_WATER_CYAN, 0);
    LCD_Draw_Rect(x + 9, y + 4, 1, 2, CLR_WATER_CYAN, 1);
    LCD_Draw_Rect(x + 2, y + 3, 2, 4, CLR_WATER_CYAN, 1);
    LCD_Draw_Rect(x + 4, y + 3, 2, 4, CLR_WATER_CYAN, 1);
    LCD_Draw_Rect(x + 6, y + 3, 2, 4, CLR_WATER_CYAN, 1);
}

/*
 * Draw a small seed icon for the HUD.
 */
static void Farm_DrawSeedIcon(int x, int y)
{
    LCD_Draw_Rect(x + 2, y + 2, 6, 6, CLR_SOIL_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 6, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Rect(x + 3, y + 1, 4, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 4, y + 4, 2, 2, CLR_LEAF_GREEN, 1);
}

/*
 * Draw the soil panel behind the 4 x 4 crop grid.
 */
static void Farm_DrawFieldPatch(void)
{
    LCD_Draw_Rect(FIELD_X, FIELD_Y, FIELD_W, FIELD_H, CLR_WOOD_DARK, 1);
    LCD_Draw_Rect(FIELD_X + 2, FIELD_Y + 2, FIELD_W - 4, FIELD_H - 4, CLR_SOIL_BROWN, 1);
}

/*
 * Draw one crop tile.
 * The fill colour changes according to crop state and water level.
 */
static void Farm_DrawTile(int x, int y, const CropTile *tile, uint8_t selected)
{
    uint8_t fill_colour;
    uint8_t border_colour = CLR_TILE_BORDER;

    /*
     * Select tile colour according to the current crop state.
     */
    switch (tile->state) {
        case TILE_SEEDED:
            fill_colour = (tile->water_level > 0) ? CLR_TILE_WET : CLR_TILE_SEEDED;
            break;

        case TILE_GROWING:
            fill_colour = (tile->water_level > 0) ? CLR_TILE_WET : CLR_TILE_GROWING_DRY;
            break;

        case TILE_MATURE:
            fill_colour = CLR_TILE_MATURE;
            break;

        case TILE_DEAD:
            fill_colour = CLR_TILE_DEAD;
            break;

        case TILE_EMPTY:
        default:
            fill_colour = CLR_TILE_EMPTY;
            break;
    }

    LCD_Draw_Rect(x, y, TILE_SIZE, TILE_SIZE, fill_colour, 1);
    LCD_Draw_Rect(x, y, TILE_SIZE, TILE_SIZE, border_colour, 0);

    /*
     * Draw cursor highlight if this tile is currently selected.
     */
    if (selected) {
        Farm_DrawSelectHighlight(x - CURSOR_PAD, y - CURSOR_PAD,
                                 TILE_SIZE + CURSOR_PAD * 2,
                                 TILE_SIZE + CURSOR_PAD * 2);
    }

    /*
     * Mature crops receive a blinking glow to remind the player to harvest.
     */
    FarmEffects_DrawMatureGlow(x, y, TILE_SIZE,
                               (tile->state == TILE_MATURE),
                               CLR_HONEY_GOLD);

    /*
     * Add a small sparkle on mature crops for extra visual feedback.
     */
    if (tile->state == TILE_MATURE) {
        uint32_t now = HAL_GetTick();

        if (((now / 280U) % 2U) == 0U) {
            LCD_Draw_Rect(x + TILE_SIZE - 5, y + 2, 2, 2, CLR_HONEY_GOLD, 1);
            LCD_Draw_Rect(x + TILE_SIZE - 3, y + 4, 1, 1, CLR_CREAM, 1);
        }
    }
}

/*
 * Draw the watering amount preview bar.
 * The bar shows how much water will be added when the player waters a crop.
 */
static void Farm_DrawWaterPreviewBar(uint8_t water_preview_amount)
{
    uint16_t fill_w = (uint16_t)(((uint32_t)(water_preview_amount - POT_WATER_MIN) *
                       WATER_BAR_W) / (POT_WATER_MAX - POT_WATER_MIN));

    Farm_DrawPanel(WATER_BAR_X - 3, WATER_BAR_Y - 4, WATER_BAR_W + 6, WATER_BAR_H + 8);

    LCD_Draw_Rect(WATER_BAR_X, WATER_BAR_Y, WATER_BAR_W, WATER_BAR_H, CLR_INK, 0);
    LCD_Draw_Rect(WATER_BAR_X + 1, WATER_BAR_Y + 1, WATER_BAR_W - 2, WATER_BAR_H - 2, CLR_WOOD_DARK, 1);

    if (fill_w > 0) {
        if (fill_w > (WATER_BAR_W - 2)) {
            fill_w = WATER_BAR_W - 2;
        }

        LCD_Draw_Rect(WATER_BAR_X + 1, WATER_BAR_Y + 1, fill_w, WATER_BAR_H - 2, CLR_WATER_CYAN, 1);
    }
}

/*
 * Draw the home building area.
 * If selected is true, an animated highlight is drawn around the house.
 */
static void Farm_DrawHouseArea(uint8_t selected)
{
    LCD_Draw_Sprite_Scaled(HOUSE_X, HOUSE_Y, SPR_HOUSE_H, SPR_HOUSE_W, (uint8_t *)spr_house, 2);

    if (selected) {
        Farm_DrawAnimatedHighlight(HOUSE_FRAME_X, HOUSE_FRAME_Y, HOUSE_FRAME_W, HOUSE_FRAME_H);
    }
}

/*
 * Draw the shop building area using simple pixel shapes.
 * If selected is true, an animated highlight is drawn around the shop.
 */
static void Farm_DrawShopArea(uint8_t selected)
{
    int x = SHOP_X;
    int y = SHOP_Y;

    /*
     * Main shop body.
     */
    LCD_Draw_Rect(x + 2, y + 12, 36, 24, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 12, 36, 24, CLR_DARK_BROWN, 0);

    /*
     * Shop window.
     */
    LCD_Draw_Rect(x + 9, y + 17, 22, 11, CLR_BLACK, 1);

    /*
     * Striped shop awning.
     */
    LCD_Draw_Rect(x, y + 4, 40, 10, CLR_SOFT_RED, 1);
    for (int i = 0; i < 40; i += 8) {
        LCD_Draw_Rect(x + i, y + 4, 4, 10, CLR_CREAM, 1);
    }
    LCD_Draw_Rect(x, y + 4, 40, 10, CLR_DARK_BROWN, 0);

    /*
     * Front counter.
     */
    LCD_Draw_Rect(x + 5, y + 30, 30, 4, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 5, y + 30, 30, 1, CLR_DARK_BROWN, 1);

    /*
     * Signboard.
     */
    LCD_Draw_Rect(x + 7, y - 2, 26, 8, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 7, y - 2, 26, 8, CLR_DARK_BROWN, 0);
    LCD_printString("SHOP", x + 9, y - 1, CLR_DARK_BROWN, 1);

    /*
     * Door.
     */
    LCD_Draw_Rect(x + 17, y + 22, 6, 14, CLR_SOIL_BROWN, 1);
    LCD_Draw_Rect(x + 17, y + 22, 6, 14, CLR_DARK_BROWN, 0);

    if (selected) {
        Farm_DrawAnimatedHighlight(SHOP_FRAME_X, SHOP_FRAME_Y, SHOP_FRAME_W, SHOP_FRAME_H);
    }
}

/*
 * Draw the farm girl in the village area.
 * The selected sprite depends on movement, mood, and spin animation state.
 */
static void Farm_DrawVillageGirl(const FarmState *state)
{
    uint32_t now = HAL_GetTick();

    const uint8_t *spr;

    /*
     * Choose the correct sprite according to the character state.
     */
    if (state->girl_spin_active || state->girl_mood == GIRL_MOOD_HAPPY) {
        spr = SPR_FARM_GIRL_HAPPY;
    } else if (state->girl_mood == GIRL_MOOD_SAD) {
        spr = SPR_FARM_GIRL_SAD;
    } else if (state->girl_is_moving) {
        switch (state->girl_walk_frame) {
            case 0:  spr = SPR_FARM_GIRL_WALK0; break;
            case 1:  spr = SPR_FARM_GIRL_WALK1; break;
            case 2:  spr = SPR_FARM_GIRL_WALK2; break;
            default: spr = SPR_FARM_GIRL_WALK3; break;
        }
    } else {
        spr = SPR_FARM_GIRL_IDLE;
    }

    {
        /*
         * Add a tiny idle bob so the character does not look completely static.
         */
        int bob = (!state->girl_is_moving && !state->girl_spin_active &&
                   (now / 350U) % 2U == 0U) ? 1 : 0;

        int draw_x = state->girl_x;
        int draw_y = state->girl_y - bob;

        /*
         * Happy spin animation moves the sprite around a small circular path.
         */
        if (state->girl_spin_active) {
            uint32_t spin_t = now - state->girl_spin_start;
            uint32_t phase = (spin_t * 4U) / 800U;

            if (phase > 3U) {
                phase = 3U;
            }

            const int8_t spin_ox[4] = {  0,  4,  0, -4 };
            const int8_t spin_oy[4] = { -3,  0,  3,  0 };

            draw_x += spin_ox[phase];
            draw_y += spin_oy[phase];
        }

        LCD_Draw_Sprite_Scaled(draw_x, draw_y, 14, 14, (uint8_t *)spr, 2);
    }
}

/* -------------------------------------------------------------------------- */
/* Intro page animation                                                        */
/* -------------------------------------------------------------------------- */

/*
 * Total intro animation duration.
 * After this time, the final frame is held on screen.
 */
#define INTRO_LOOP_MS          15600U

/*
 * Timeline markers for the intro animation.
 * Each stage starts after the specified elapsed time.
 */
#define INTRO_STAGE_COVER         0U
#define INTRO_STAGE_OPEN       3400U
#define INTRO_STAGE_HEADER     6200U
#define INTRO_STAGE_LEFT_1     7000U
#define INTRO_STAGE_LEFT_2     7900U
#define INTRO_STAGE_LEFT_3     8800U
#define INTRO_STAGE_RULE_1     9800U
#define INTRO_STAGE_RULE_2    10600U
#define INTRO_STAGE_RULE_3    11400U
#define INTRO_STAGE_RULE_4    12200U
#define INTRO_STAGE_PROMPT    13200U

/*
 * If the rules page has not been rendered for this long, the intro animation
 * is restarted the next time the page appears.
 */
#define INTRO_REENTER_RESET_MS  700U

/*
 * Intro animation runtime state.
 */
static uint8_t intro_v9_started = 0;
static uint32_t intro_v9_start_ms = 0;
static uint32_t intro_v9_last_call_ms = 0;

/*
 * Return 1 when the current timeline has reached a given stage.
 */
static uint8_t Intro_IsVisible(uint32_t t, uint32_t start)
{
    return (t >= start) ? 1U : 0U;
}

/*
 * Integer ease-out interpolation.
 * This creates smoother slide-in and opening animations without floating point.
 */
static int Intro_EaseOutInt(uint32_t t, uint32_t start, uint32_t dur, int from, int to)
{
    if (t <= start) {
        return from;
    }

    if (t >= start + dur) {
        return to;
    }

    {
        uint32_t p = t - start;
        uint32_t q = dur - p;
        int range = to - from;
        uint32_t eased = dur * dur - q * q;

        return from + (int)((int64_t)range * (int64_t)eased / (int64_t)(dur * dur));
    }
}

/*
 * Draw small bold text without a dark shadow.
 * This keeps labels clean on the intro journal pages.
 */
static void Intro_PrintBoldClean(const char *str, int x, int y, uint8_t colour)
{
    LCD_printString(str, x,     y, colour, 1);
    LCD_printString(str, x + 1, y, colour, 1);
}

/*
 * Draw a tiny sparkle icon.
 */
static void Intro_DrawTinySpark(int x, int y, uint8_t colour)
{
    LCD_Draw_Rect(x + 1, y,     1, 5, colour, 1);
    LCD_Draw_Rect(x,     y + 2, 5, 1, colour, 1);
}

/*
 * Draw the soft outdoor background behind the intro journal.
 * It includes sky, grass, moving clouds, flowers, and small sparkles.
 */
static void Intro_DrawBackground(uint32_t now_ms)
{
    int c1, c2;
    int i;

    LCD_Draw_Rect(0,   0, 240, 100, CLR_SKY_DAY, 1);
    LCD_Draw_Rect(0, 100, 240,  20, CLR_PARCH, 1);
    LCD_Draw_Rect(0, 120, 240,  94, CLR_SAGE, 1);
    LCD_Draw_Rect(0, 214, 240,  26, CLR_PANEL_BLUE, 1);

    /*
     * Rounded grass hills.
     */
    LCD_Draw_Circle( 28, 155, 42, CLR_GRASS_GREEN, 1);
    LCD_Draw_Circle( 88, 150, 48, CLR_GRASS_GREEN, 1);
    LCD_Draw_Circle(160, 154, 44, CLR_GRASS_GREEN, 1);
    LCD_Draw_Circle(224, 149, 42, CLR_GRASS_GREEN, 1);

    /*
     * Grass layers.
     */
    LCD_Draw_Rect(0, 158, 240, 8, CLR_GRASS_GREEN, 1);
    LCD_Draw_Rect(0, 176, 240, 5, CLR_GRASS_GREEN, 1);
    LCD_Draw_Rect(0, 190, 240, 4, CLR_GRASS_GREEN, 1);

    /*
     * Moving clouds use ping-pong offsets.
     */
    c1 = Farm_PingPongOffset(now_ms, 3900U, 10);
    c2 = Farm_PingPongOffset(now_ms, 5200U, 8);

    Farm_DrawCloud(18 + c1, 18);
    Farm_DrawCloud(160 - c2, 13);

    /*
     * Small sparkles change position over time.
     */
    if (((now_ms / 320U) % 3U) == 0U) {
        Intro_DrawTinySpark(28, 84, CLR_CREAM);
        Intro_DrawTinySpark(202, 92, CLR_CREAM);
    } else if (((now_ms / 320U) % 3U) == 1U) {
        Intro_DrawTinySpark(44, 74, CLR_PARCH);
        Intro_DrawTinySpark(190, 86, CLR_PARCH);
    } else {
        Intro_DrawTinySpark(34, 94, CLR_CREAM);
        Intro_DrawTinySpark(214, 78, CLR_PARCH);
    }

    /*
     * Decorative flowers near the bottom of the grass area.
     */
    for (i = 0; i < 6; i++) {
        int fx = 16 + i * 38;
        uint8_t flower_col = (i % 2 == 0) ? CLR_SOFT_RED : CLR_CREAM;

        LCD_Draw_Rect(fx,     199, 2, 2, flower_col, 1);
        LCD_Draw_Rect(fx + 1, 201, 1, 4, CLR_LEAF_GREEN, 1);
    }
}

/*
 * Draw the closed journal cover.
 * The cover grows slightly during the first intro stage.
 */
static void Intro_DrawClosedJournal(uint32_t t, uint32_t now_ms)
{
    int w = Intro_EaseOutInt(t, INTRO_STAGE_COVER, 1350U, 74, 128);
    int h = Intro_EaseOutInt(t, INTRO_STAGE_COVER, 1350U, 94, 158);
    int x = 120 - w / 2;
    int y = 116 - h / 2;
    uint8_t shine = (uint8_t)(((now_ms / 620U) % 2U) == 0U);

    /*
     * Journal shadow and main cover.
     */
    LCD_Draw_Rect(x + 6, y + 6, w, h, CLR_WOOD_DARK, 1);

    LCD_Draw_Rect(x, y, w, h, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(x, y, w, h, CLR_DARK_BROWN, 0);

    /*
     * Cover border details.
     */
    LCD_Draw_Rect(x + 4, y + 4, w - 8, h - 8, CLR_NIGHT_BLUE, 0);
    LCD_Draw_Rect(x + 6, y + 6, w - 12, 2, CLR_WATER_CYAN, 1);
    LCD_Draw_Rect(x + 6, y + 6, 2, h - 12, CLR_WATER_CYAN, 1);

    /*
     * Journal spine and side strap.
     */
    LCD_Draw_Rect(x + 10, y, 13, h, CLR_NIGHT_BLUE, 1);
    LCD_Draw_Rect(x + 15, y + 9, 2, h - 18, CLR_WOOD_LIGHT, 1);

    LCD_Draw_Rect(x + w - 21, y + 12, 9, h - 24, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + w - 19, y + 18, 5, h - 36, CLR_WOOD_DARK, 0);

    /*
     * Small lock decoration.
     */
    LCD_Draw_Rect(x + w - 24, y + h / 2 - 7, 13, 14, CLR_PARCH, 1);
    LCD_Draw_Rect(x + w - 22, y + h / 2 - 5, 9, 10, CLR_TRUE_BROWN, 0);

    /*
     * Title plaque with strong contrast.
     */
    LCD_Draw_Rect(x + 27, y + 27, w - 54, 29, CLR_NIGHT_BLUE, 1);
    LCD_Draw_Rect(x + 30, y + 30, w - 60, 23, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(x + 30, y + 30, w - 60, 23, CLR_CREAM, 0);

    if (w > 96) {
        Intro_PrintBoldClean("FARM",    x + w / 2 - 16, y + 68, CLR_CREAM);
        Intro_PrintBoldClean("JOURNAL", x + w / 2 - 24, y + 83, CLR_CREAM);
    }

    /*
     * Clock-like emblem near the bottom of the cover.
     */
    LCD_Draw_Circle(x + w / 2, y + h - 34, 10, CLR_PARCH, 1);
    LCD_Draw_Circle(x + w / 2, y + h - 34, 10, CLR_TRUE_BROWN, 0);
    LCD_Draw_Rect(x + w / 2 - 1, y + h - 40, 2, 12, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + w / 2 - 5, y + h - 35, 10, 2, CLR_TRUE_BROWN, 1);

    if (shine) {
        Intro_DrawTinySpark(x + w - 36, y + 22, CLR_CREAM);
    }
}

/*
 * Draw the open journal.
 * The left and right pages expand out from the centre.
 */
static void Intro_DrawOpenJournal(uint32_t t)
{
    int y = 18;
    int half_w = Intro_EaseOutInt(t, INTRO_STAGE_OPEN, 2200U, 8, 100);
    int outer_x = 120 - half_w - 11;
    int outer_w = half_w * 2 + 22;
    int page_h = 170;

    /*
     * Journal shadow and outer cover.
     */
    LCD_Draw_Rect(outer_x + 4, y + 5, outer_w, 182, CLR_WOOD_DARK, 1);

    LCD_Draw_Rect(outer_x, y, outer_w, 182, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(outer_x, y, outer_w, 182, CLR_DARK_BROWN, 0);

    LCD_Draw_Rect(outer_x + 4, y + 4, outer_w - 8, 174, CLR_NIGHT_BLUE, 0);

    /*
     * Left and right paper pages.
     */
    LCD_Draw_Rect(120 - half_w, y + 8, half_w - 8, page_h, CLR_PARCH, 1);
    LCD_Draw_Rect(120 - half_w, y + 8, half_w - 8, page_h, CLR_TRUE_BROWN, 0);

    LCD_Draw_Rect(128, y + 8, half_w - 8, page_h, CLR_PARCH, 1);
    LCD_Draw_Rect(128, y + 8, half_w - 8, page_h, CLR_TRUE_BROWN, 0);

    /*
     * Page highlights and page edge details.
     */
    LCD_Draw_Rect(120 - half_w + 2, y + 10, 2, page_h - 4, CLR_CREAM, 1);
    LCD_Draw_Rect(130,               y + 10, 2, page_h - 4, CLR_CREAM, 1);
    LCD_Draw_Rect(120 + half_w - 12, y + 10, 2, page_h - 4, CLR_CREAM, 1);

    /*
     * Centre binding.
     */
    LCD_Draw_Rect(117, y + 6, 4, page_h + 4, CLR_NIGHT_BLUE, 1);
    LCD_Draw_Rect(119, y + 10, 2, page_h - 4, CLR_WOOD_LIGHT, 1);

    {
        int sy;

        for (sy = y + 18; sy < y + 162; sy += 14) {
            LCD_Draw_Rect(119, sy, 2, 2, CLR_CREAM, 1);
        }
    }

    /*
     * Page headers appear after the journal has opened.
     */
    if (t >= INTRO_STAGE_HEADER) {
        LCD_Draw_Rect(50,  y + 12, 22, 6, CLR_SAGE, 1);
        LCD_Draw_Rect(164, y + 12, 22, 6, CLR_SOFT_RED, 1);

        Intro_PrintBoldClean("PROFILE", 42,  y + 21, CLR_TRUE_BROWN);
        Intro_PrintBoldClean("TASKS",   158, y + 21, CLR_TRUE_BROWN);

        LCD_Draw_Rect(34,  y + 36, 66, 1, CLR_TRUE_BROWN, 1);
        LCD_Draw_Rect(146, y + 36, 56, 1, CLR_TRUE_BROWN, 1);
    }
}

/*
 * Draw a paper sticker with a small pin.
 * This is used as the base for intro profile stickers.
 */
static void Intro_DrawStickerBase(int x, int y, int w, int h, uint8_t pin_col)
{
    LCD_Draw_Rect(x + 2, y + 2, w, h, CLR_WOOD_LIGHT, 1);
    LCD_Draw_Rect(x, y, w, h, CLR_CREAM, 1);
    LCD_Draw_Rect(x, y, w, h, CLR_TRUE_BROWN, 0);

    LCD_Draw_Circle(x + w - 7, y + 7, 4, pin_col, 1);
    LCD_Draw_Circle(x + w - 7, y + 7, 4, CLR_TRUE_BROWN, 0);
}

/*
 * Draw the cartoon girl avatar sticker.
 * The avatar includes blinking eyes and a small cheek animation.
 */
static void Intro_DrawGirlAvatar(int x, int y, uint32_t now_ms)
{
    uint8_t blink = (uint8_t)(((now_ms / 1800U) % 4U) == 0U);
    uint8_t cheek = (uint8_t)(((now_ms / 700U) % 2U) == 0U);

    Intro_DrawStickerBase(x, y, 50, 45, CLR_SOFT_RED);

    /*
     * Hair and hat base.
     */
    LCD_Draw_Rect(x + 15, y + 12, 20, 18, CLR_TRUE_BROWN, 1);
    LCD_Draw_Circle(x + 19, y + 20, 6, CLR_TRUE_BROWN, 1);
    LCD_Draw_Circle(x + 31, y + 20, 6, CLR_TRUE_BROWN, 1);

    LCD_Draw_Rect(x + 12, y + 8, 26, 3, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 18, y + 5, 14, 4, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 20, y + 6, 10, 1, CLR_CREAM, 1);

    /*
     * Face shape.
     */
    LCD_Draw_Rect(x + 17, y + 15, 18, 12, CLR_CREAM, 1);
    LCD_Draw_Circle(x + 17, y + 21, 6, CLR_CREAM, 1);
    LCD_Draw_Circle(x + 35, y + 21, 6, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 17, y + 15, 18, 6, CLR_CREAM, 1);

    /*
     * Fringe and side hair.
     */
    LCD_Draw_Rect(x + 18, y + 13, 16, 3, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 15, y + 18, 3, 9, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 34, y + 18, 3, 9, CLR_TRUE_BROWN, 1);

    /*
     * Blinking eyes.
     */
    if (blink) {
        LCD_Draw_Rect(x + 21, y + 20, 3, 1, CLR_INK, 1);
        LCD_Draw_Rect(x + 29, y + 20, 3, 1, CLR_INK, 1);
    } else {
        LCD_Draw_Rect(x + 21, y + 19, 2, 2, CLR_INK, 1);
        LCD_Draw_Rect(x + 30, y + 19, 2, 2, CLR_INK, 1);
    }

    /*
     * Animated cheek pixels.
     */
    if (cheek) {
        LCD_Draw_Rect(x + 18, y + 23, 2, 1, CLR_SOFT_RED, 1);
        LCD_Draw_Rect(x + 33, y + 23, 2, 1, CLR_SOFT_RED, 1);
    }

    LCD_Draw_Rect(x + 24, y + 25, 5, 1, CLR_SOFT_RED, 1);

    /*
     * Small collar and label.
     */
    LCD_Draw_Rect(x + 21, y + 31, 11, 4, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 24, y + 31, 5, 1, CLR_CREAM, 1);

    LCD_printString("GIRL", x + 14, y + 35, CLR_TRUE_BROWN, 1);
}

/*
 * Draw the farm field sticker.
 * It shows a small 4 x 4 farm grid with mixed tile states.
 */
static void Intro_DrawFieldSticker(int x, int y, uint32_t now_ms)
{
    int r, c;
    uint8_t glow = (uint8_t)(((now_ms / 420U) % 2U) == 0U);

    Intro_DrawStickerBase(x, y, 52, 40, CLR_HONEY_GOLD);

    LCD_Draw_Rect(x + 7, y + 8, 38, 19, CLR_WOOD_DARK, 1);
    LCD_Draw_Rect(x + 9, y + 10, 34, 15, CLR_SOIL_BROWN, 1);

    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            int tx = x + 10 + c * 8;
            int ty = y + 11 + r * 3;

            LCD_Draw_Rect(tx, ty, 6, 2, CLR_TILE_EMPTY, 1);
            LCD_Draw_Rect(tx, ty, 6, 2, CLR_TRUE_BROWN, 0);

            /*
             * Small crop sprouts.
             */
            if ((r == 0 && c == 1) || (r == 1 && c == 3) ||
                (r == 2 && c == 0) || (r == 3 && c == 2)) {
                LCD_Draw_Rect(tx + 2, ty, 2, 2, CLR_LEAF_GREEN, 1);
            }

            /*
             * Watered tiles.
             */
            if ((r == 0 && c == 3) || (r == 2 && c == 2)) {
                LCD_Draw_Rect(tx + 1, ty, 4, 1, CLR_WATER_CYAN, 1);
            }

            /*
             * Mature crop highlight.
             */
            if (r == 1 && c == 1) {
                LCD_Draw_Rect(tx + 2, ty, 2, 2, glow ? CLR_HONEY_GOLD : CLR_CREAM, 1);
            }
        }
    }

    if (glow) {
        Intro_DrawTinySpark(x + 41, y + 9, CLR_HONEY_GOLD);
    }

    LCD_printString("FARM", x + 14, y + 30, CLR_TRUE_BROWN, 1);
}

/*
 * Draw the tools sticker.
 * It includes a watering can, seed pouch, and coin icon.
 */
static void Intro_DrawToolsSticker(int x, int y, uint32_t now_ms)
{
    uint8_t drop_phase = (uint8_t)((now_ms / 280U) % 3U);
    uint8_t coin_flash = (uint8_t)(((now_ms / 500U) % 2U) == 0U);

    Intro_DrawStickerBase(x, y, 52, 34, CLR_WATER_CYAN);

    /*
     * Watering can.
     */
    LCD_Draw_Rect(x + 6,  y + 10, 10, 7, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(x + 6,  y + 10, 10, 7, CLR_TRUE_BROWN, 0);
    LCD_Draw_Rect(x + 8,  y + 8,   5, 2, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 8,  y + 8,   5, 2, CLR_TRUE_BROWN, 0);
    LCD_Draw_Circle(x + 5, y + 14, 3, CLR_TRUE_BROWN, 0);
    LCD_Draw_Rect(x + 16, y + 12, 4, 2, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(x + 20, y + 11, 2, 1, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(x + 20, y + 15, 2, 1, CLR_PANEL_BLUE, 1);

    /*
     * Animated water droplet.
     */
    if (drop_phase == 0U) {
        LCD_Draw_Rect(x + 21, y + 18, 1, 2, CLR_WATER_CYAN, 1);
    } else if (drop_phase == 1U) {
        LCD_Draw_Rect(x + 23, y + 19, 1, 2, CLR_WATER_CYAN, 1);
    } else {
        LCD_Draw_Rect(x + 21, y + 21, 1, 2, CLR_WATER_CYAN, 1);
    }

    /*
     * Seed pouch.
     */
    LCD_Draw_Rect(x + 28, y + 10, 8, 11, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 28, y + 10, 8, 11, CLR_TRUE_BROWN, 0);
    LCD_Draw_Rect(x + 30, y + 12, 4, 3, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 31, y + 16, 2, 2, CLR_LEAF_GREEN, 1);

    /*
     * Coin.
     */
    LCD_Draw_Circle(x + 43, y + 15, 5, coin_flash ? CLR_HONEY_GOLD : CLR_PARCH, 1);
    LCD_Draw_Circle(x + 43, y + 15, 5, CLR_TRUE_BROWN, 0);
    LCD_Draw_Rect(x + 42, y + 12, 2, 5, CLR_CREAM, 1);

    if (coin_flash) {
        Intro_DrawTinySpark(x + 46, y + 8, CLR_HONEY_GOLD);
    }

    LCD_printString("TOOLS", x + 10, y + 24, CLR_TRUE_BROWN, 1);
}

/*
 * Draw the left page of the intro journal.
 * Profile stickers appear one by one with short slide-in animations.
 */
static void Intro_DrawLeftPage(uint32_t t, uint32_t now_ms)
{
    if (Intro_IsVisible(t, INTRO_STAGE_LEFT_1)) {
        int y1 = Intro_EaseOutInt(t, INTRO_STAGE_LEFT_1, 430U, 78, 62);
        Intro_DrawGirlAvatar(32, y1, now_ms);
    }

    if (Intro_IsVisible(t, INTRO_STAGE_LEFT_2)) {
        int x2 = Intro_EaseOutInt(t, INTRO_STAGE_LEFT_2, 430U, 5, 31);
        Intro_DrawFieldSticker(x2, 110, now_ms);
    }

    if (Intro_IsVisible(t, INTRO_STAGE_LEFT_3)) {
        int y3 = Intro_EaseOutInt(t, INTRO_STAGE_LEFT_3, 430U, 192, 154);
        Intro_DrawToolsSticker(31, y3, now_ms);
    }
}

/*
 * Draw a small checked box for the rule list.
 */
static void Intro_DrawCheckBox(int x, int y, uint8_t checked)
{
    LCD_Draw_Rect(x, y, 10, 10, CLR_TRUE_BROWN, 0);

    if (checked) {
        LCD_Draw_Rect(x + 2, y + 5, 2, 2, CLR_LEAF_GREEN, 1);
        LCD_Draw_Rect(x + 4, y + 6, 2, 2, CLR_LEAF_GREEN, 1);
        LCD_Draw_Rect(x + 6, y + 4, 2, 2, CLR_LEAF_GREEN, 1);
        LCD_Draw_Rect(x + 7, y + 2, 2, 2, CLR_LEAF_GREEN, 1);
    }
}

/*
 * Draw one task/rule row on the right page.
 */
static void Intro_DrawRuleRow(int x, int y, const char *title, const char *sub, uint8_t accent)
{
    LCD_Draw_Rect(x + 2, y + 2, 66, 20, CLR_WOOD_LIGHT, 1);
    LCD_Draw_Rect(x,     y,     66, 20, CLR_CREAM, 1);
    LCD_Draw_Rect(x,     y,     66, 20, CLR_TRUE_BROWN, 0);

    LCD_Draw_Rect(x, y, 4, 20, accent, 1);

    Intro_DrawCheckBox(x + 7, y + 5, 1);

    LCD_printString(title, x + 22, y + 2,  CLR_DARK_BROWN, 1);
    LCD_printString(sub,   x + 22, y + 11, CLR_TRUE_BROWN, 1);
}

/*
 * Draw the right page of the intro journal.
 * The rule rows slide in from the right side one by one.
 */
static void Intro_DrawRightPage(uint32_t t)
{
    if (Intro_IsVisible(t, INTRO_STAGE_RULE_1)) {
        int x1 = Intro_EaseOutInt(t, INTRO_STAGE_RULE_1, 360U, 228, 142);
        Intro_DrawRuleRow(x1, 64, "PLANT", "seed", CLR_SAGE);
    }

    if (Intro_IsVisible(t, INTRO_STAGE_RULE_2)) {
        int x2 = Intro_EaseOutInt(t, INTRO_STAGE_RULE_2, 360U, 228, 142);
        Intro_DrawRuleRow(x2, 92, "WATER", "daily", CLR_WATER_CYAN);
    }

    if (Intro_IsVisible(t, INTRO_STAGE_RULE_3)) {
        int x3 = Intro_EaseOutInt(t, INTRO_STAGE_RULE_3, 360U, 228, 142);
        Intro_DrawRuleRow(x3, 120, "PICK", "ripe", CLR_HONEY_GOLD);
    }

    if (Intro_IsVisible(t, INTRO_STAGE_RULE_4)) {
        int x4 = Intro_EaseOutInt(t, INTRO_STAGE_RULE_4, 360U, 228, 142);
        Intro_DrawRuleRow(x4, 148, "REST", "home", CLR_SOFT_RED);
    }
}

/*
 * Draw the intro footer message.
 * The footer text changes according to the current intro stage.
 */
static void Intro_DrawFooter(uint32_t t, uint32_t now_ms)
{
    LCD_Draw_Rect(0, 212, 240, 2, CLR_PARCH, 1);
    LCD_Draw_Rect(0, 214, 240, 26, CLR_PANEL_BLUE, 1);

    if (t < INTRO_STAGE_OPEN) {
        LCD_printString("Opening journal...", 66, 223, CLR_CREAM, 1);
        return;
    }

    if (t < INTRO_STAGE_PROMPT) {
        LCD_printString("Farm guide ready", 70, 223, CLR_CREAM, 1);
        return;
    }

    /*
     * Blink the final start prompt.
     */
    if (((now_ms / 520U) % 2U) == 0U) {
        LCD_printString("PRESS JOY TO START", 54, 223, CLR_PARCH, 1);
    } else {
        LCD_printString("PRESS JOY TO START", 54, 223, CLR_CREAM, 1);
    }
}

/*
 * Render the animated rules / intro page.
 * The animation presents a journal opening sequence and then holds on the
 * final rule page until the player presses the joystick button.
 */
void FarmRender_RenderRules(uint32_t now_ms)
{
    uint32_t t;

    LCD_Set_Palette(PALETTE_FARM_UNIFIED);

    /*
     * Restart the intro animation when returning to the rules page after
     * the page has not been rendered for a short time.
     */
    if (!intro_v9_started ||
        (intro_v9_last_call_ms != 0U &&
         (now_ms - intro_v9_last_call_ms) > INTRO_REENTER_RESET_MS)) {
        intro_v9_started = 1;
        intro_v9_start_ms = now_ms;
    }

    intro_v9_last_call_ms = now_ms;

    t = now_ms - intro_v9_start_ms;

    /*
     * Hold the final frame instead of looping back to the cover.
     */
    if (t >= INTRO_LOOP_MS) {
        t = INTRO_LOOP_MS - 1U;
    }

    Intro_DrawBackground(now_ms);

    if (t < INTRO_STAGE_OPEN) {
        Intro_DrawClosedJournal(t, now_ms);
        Intro_DrawFooter(t, now_ms);
        return;
    }

    Intro_DrawOpenJournal(t);

    if (t >= INTRO_STAGE_HEADER) {
        Intro_DrawLeftPage(t, now_ms);
        Intro_DrawRightPage(t);
    }

    Intro_DrawFooter(t, now_ms);
}

/* -------------------------------------------------------------------------- */
/* Dry crop alarm icon                                                         */
/* -------------------------------------------------------------------------- */

/*
 * Draw warning burst pixels around the dry alarm icon.
 */
static void Farm_DrawAlarmBurst(int cx, int cy, uint8_t c1, uint8_t c2)
{
    LCD_Draw_Rect(cx - 2,  cy - 16, 4, 4, c1, 1);
    LCD_Draw_Rect(cx - 17, cy - 3,  6, 4, c2, 1);
    LCD_Draw_Rect(cx + 11, cy - 3,  6, 4, c2, 1);
    LCD_Draw_Rect(cx - 11, cy + 12, 5, 4, c1, 1);
    LCD_Draw_Rect(cx + 6,  cy + 12, 5, 4, c1, 1);
}

/*
 * Draw the animated dry-crop alarm.
 * This appears above the selected tile when a living crop is close to drought.
 */
static void Farm_DrawDryAlarm(int x, int y, uint32_t now_ms)
{
    int shake_x = ((now_ms / 90U) % 2U == 0U) ? -1 : 1;
    int hop_y   = ((now_ms / 150U) % 2U == 0U) ? 0 : -3;

    int bx = x + shake_x;
    int by = y + hop_y;

    /*
     * Flashing warning burst.
     */
    if (((now_ms / 120U) % 2U) == 0U) {
        Farm_DrawAlarmBurst(bx, by, CLR_SOFT_RED, CLR_HONEY_GOLD);
    } else {
        Farm_DrawAlarmBurst(bx, by, CLR_HONEY_GOLD, CLR_CREAM);
    }

    /*
     * Alarm bells.
     */
    LCD_Draw_Circle(bx - 7, by - 5, 4, CLR_SOFT_RED, 1);
    LCD_Draw_Circle(bx + 7, by - 5, 4, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(bx - 9, by - 1, 4, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(bx + 5, by - 1, 4, 2, CLR_DARK_BROWN, 1);

    /*
     * Clock body.
     */
    LCD_Draw_Circle(bx, by + 6, 10, CLR_CREAM, 1);
    LCD_Draw_Circle(bx, by + 6, 10, CLR_DARK_BROWN, 0);
    LCD_Draw_Circle(bx, by + 6, 7, CLR_HONEY_GOLD, 0);

    /*
     * Clock hands.
     */
    LCD_Draw_Rect(bx - 1, by + 2, 2, 6, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(bx, by + 5, 5, 2, CLR_SOFT_RED, 1);

    /*
     * Alarm legs.
     */
    LCD_Draw_Rect(bx - 6, by + 15, 3, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(bx + 3, by + 15, 3, 3, CLR_DARK_BROWN, 1);
}

/*
 * Convert the current farm mode into a short display string.
 */
const char *FarmRender_GetModeName(FarmMode mode)
{
    switch (mode) {
        case MODE_PLANT:   return "PLANT";
        case MODE_WATER:   return "WATER";
        case MODE_HARVEST: return "HARVEST";
        default:           return "PLANT";
    }
}

/*
 * Convert a tile state into a short display string.
 */
const char *FarmRender_GetTileStateName(TileState state)
{
    switch (state) {
        case TILE_EMPTY:   return "EMPTY";
        case TILE_SEEDED:  return "SEEDED";
        case TILE_GROWING: return "GROW";
        case TILE_MATURE:  return "MATURE";
        case TILE_DEAD:    return "DEAD";
        default:           return "EMPTY";
    }
}

/*
 * Generate the footer hint for the selected tile.
 * The hint changes according to energy, seed count, temperature, water level,
 * and crop state.
 */
const char *FarmRender_GetTileHint(const FarmState *state, const CropTile *tile)
{
    if (state->game.energy == 0) {
        return "No energy, go sleep";
    }

    if (state->game.mode == MODE_PLANT && state->game.seeds == 0) {
        return "No seeds, go shop";
    }

    if (tile->state == TILE_SEEDED || tile->state == TILE_GROWING) {
        if (state->temp_state == TEMP_STATE_HOT) {
            return "Hot - water more";
        }

        if (state->temp_state == TEMP_STATE_COLD) {
            return "Too cold";
        }

        if (tile->water_level == 0 && tile->dry_ticks >= FARM_DRY_ALERT_TICKS) {
            return "WATER NOW!";
        }

        if (tile->water_level == 0) {
            return "Need water";
        }
    }

    switch (tile->state) {
        case TILE_EMPTY:   return "Plant here";
        case TILE_SEEDED:  return "Seed growing";
        case TILE_GROWING: return "Growing...";
        case TILE_MATURE:  return "Ready to harvest";
        case TILE_DEAD:    return "Clear the tile";
        default:           return "";
    }
}

/*
 * Render the main farm field scene.
 * This includes the sky, HUD, village area, crop grid, selected-tile hint,
 * dry alarm, temporary effects, and popup messages.
 */
void FarmRender_RenderField(const FarmState *state)
{
    char buf[64];
    const CropTile *sel_tile = &state->game.tiles[state->game.cursor_row][state->game.cursor_col];

    LCD_Set_Palette(PALETTE_FARM_UNIFIED);

    /*
     * Draw the top sky area.
     */
    LCD_Draw_Rect(0, 0, SCR_W, TOP_H, state->is_daytime ? CLR_SKY_DAY : CLR_SKY_NIGHT, 1);

    if (state->is_daytime) {
        uint32_t now = HAL_GetTick();
        int cloud_offset = Farm_PingPongOffset(now, 2600U, 18);

        Farm_DrawCloud(10 + cloud_offset, 12);
        Farm_DrawCloud(180 - cloud_offset, 14);
    } else {
        Farm_DrawMoon(200, 18);
    }

    /*
     * Game title.
     */
    Farm_PrintBold("HARVEST", 72, 12, CLR_TEXT_ACCENT, 2);

    /*
     * Draw the HUD background and borders.
     */
    LCD_Draw_Rect(0, HUD_Y, SCR_W, HUD_H, CLR_HUD_BG, 1);
    LCD_Draw_Rect(0, HUD_Y, SCR_W, 1, CLR_HUD_BORDER, 1);
    LCD_Draw_Rect(0, HUD_Y + HUD_H - 1, SCR_W, 1, CLR_HUD_BORDER, 1);

    /*
     * Coin display.
     */
    {
        int coin_x = 6;
        int coin_y = HUD_Y + 6;

        if (FarmEffects_IsCoinBounceActive()) {
            coin_x = 4;
            coin_y = HUD_Y + 4;
            LCD_Draw_Rect(coin_x - 1, coin_y - 1, 12, 12, CLR_HONEY_GOLD, 0);
        }

        LCD_Draw_Sprite_Scaled(coin_x, coin_y, 10, 10, (uint8_t *)ICO_FARM_COIN, 1);
    }

    sprintf(buf, "%d", state->game.coins);
    Farm_PrintBold(buf, 19, HUD_Y + 7, CLR_HUD_TEXT, 1);

    /*
     * Mode display.
     */
    LCD_Draw_Rect(50, HUD_Y + 3, 1, HUD_H - 6, CLR_HUD_BORDER, 1);

    Farm_PrintBold(FarmRender_GetModeName(state->game.mode), 58, HUD_Y + 7, CLR_CREAM, 1);

    /*
     * Day display.
     */
    LCD_Draw_Rect(108, HUD_Y + 3, 1, HUD_H - 6, CLR_HUD_BORDER, 1);
    Farm_DrawDayIcon(114, HUD_Y + 6);

    sprintf(buf, "%d", state->game.day);
    Farm_PrintBold(buf, 127, HUD_Y + 7, CLR_HONEY_GOLD, 1);

    /*
     * Energy display.
     */
    LCD_Draw_Rect(150, HUD_Y + 3, 1, HUD_H - 6, CLR_HUD_BORDER, 1);
    Farm_DrawEnergyIcon(156, HUD_Y + 6);

    sprintf(buf, "%d", state->game.energy);
    Farm_PrintBold(buf, 169, HUD_Y + 7, CLR_WATER_CYAN, 1);

    /*
     * Seed display.
     */
    LCD_Draw_Rect(192, HUD_Y + 3, 1, HUD_H - 6, CLR_HUD_BORDER, 1);
    Farm_DrawSeedIcon(198, HUD_Y + 6);

    sprintf(buf, "%d", state->game.seeds);
    Farm_PrintBold(buf, 209, HUD_Y + 7, CLR_LEAF_GREEN, 1);

    /*
     * Main field background.
     */
    LCD_Draw_Rect(0, SCENE_Y, SCR_W, FOOTER_Y - 18 - SCENE_Y, CLR_GRASS_GREEN, 1);

    /*
     * Village objects and character.
     */
    Farm_DrawHouseArea(state->game.selecting_home);
    Farm_DrawShopArea(state->game.selecting_shop);
    Farm_DrawVillageGirl(state);

    /*
     * Draw water preview only in watering mode.
     */
    if (state->game.mode == MODE_WATER) {
        Farm_DrawWaterPreviewBar(state->water_preview_amount);
    }

    /*
     * Draw crop field panel and all crop tiles.
     */
    Farm_DrawFieldPatch();

    for (int r = 0; r < FARM_ROWS; r++) {
        for (int c = 0; c < FARM_COLS; c++) {
            int tx = TILE_START_X + c * TILE_STEP;
            int ty = TILE_START_Y + r * TILE_STEP;

            Farm_DrawTile(tx, ty,
                          &state->game.tiles[r][c],
                          (r == state->game.cursor_row && c == state->game.cursor_col));
        }
    }

    /*
     * Draw dry alarm above the selected tile when it is close to dying.
     */
    if ((sel_tile->state == TILE_SEEDED || sel_tile->state == TILE_GROWING) &&
        sel_tile->water_level == 0 &&
        sel_tile->dry_ticks >= FARM_DRY_ALERT_TICKS &&
        sel_tile->dry_ticks < DROUGHT_DEATH_TICKS) {
        int alarm_x = TILE_START_X + state->game.cursor_col * TILE_STEP + TILE_SIZE / 2;
        int alarm_y = TILE_START_Y + state->game.cursor_row * TILE_STEP - 24;

        Farm_DrawDryAlarm(alarm_x, alarm_y, HAL_GetTick());
    }

    /*
     * Draw temporary tile action effects.
     */
    FarmEffects_DrawTileEffect(TILE_START_X,
                               TILE_START_Y,
                               TILE_STEP,
                               TILE_SIZE,
                               CLR_LEAF_GREEN,
                               CLR_WATER_CYAN,
                               CLR_HONEY_GOLD);

    /*
     * Footer panels.
     */
    LCD_Draw_Rect(0, FOOTER_Y - 18, SCR_W, 18, CLR_PANEL_BLUE, 1);
    LCD_Draw_Rect(0, FOOTER_Y,      SCR_W, FOOTER_H, CLR_NIGHT_BLUE, 1);
    LCD_Draw_Rect(0, FOOTER_Y - 19, SCR_W, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(0, FOOTER_Y - 1,  SCR_W, 1, CLR_CREAM, 1);

    /*
     * Context hint line.
     */
    if (state->game.selecting_home) {
        Farm_PrintBold("GO HOME - PRESS JOY", 4, FOOTER_Y - 14, CLR_CREAM, 1);
    } else if (state->game.selecting_shop) {
        Farm_PrintBold("GO SHOP - PRESS JOY", 4, FOOTER_Y - 14, CLR_CREAM, 1);
    } else {
        sprintf(buf, "R%d C%d  %s - %s",
                state->game.cursor_row + 1,
                state->game.cursor_col + 1,
                FarmRender_GetTileStateName(sel_tile->state),
                FarmRender_GetTileHint(state, sel_tile));

        Farm_PrintBold(buf, 4, FOOTER_Y - 14, CLR_CREAM, 1);
    }

    /*
     * Control hint line.
     */
    Farm_PrintBold("B2:MODE",        4,   FOOTER_Y + 5, CLR_HONEY_GOLD, 1);
    Farm_PrintBold("B3:SELECTION",  70,   FOOTER_Y + 5, CLR_CREAM, 1);
    Farm_PrintBold("JOY:ENTER",    160,   FOOTER_Y + 5, CLR_WATER_CYAN, 1);

    /*
     * Draw popup messages above the field when active.
     */
    FarmEffects_DrawPopup(SCR_W,
                          90,
                          CLR_CREAM,
                          CLR_DARK_BROWN,
                          CLR_DARK_BROWN);
}