#include "FarmHome.h"
#include "farm_palette.h"
#include "LCD.h"
#include "../game_3/mine_sprites.h"
#include "stm32l4xx_hal.h"

/*
 * All colour aliases are defined in farm_palette.h.
 * This file only uses semantic palette names to keep the home scene consistent
 * with the main farm scene.
 */

/* -------------------------------------------------------------------------- */
/* Helper drawing functions                                                    */
/* -------------------------------------------------------------------------- */

/*
 * Draw a decorative window with a sunset/night scenery outside.
 * The scene includes a warm sky gradient, mountains, stars, curtains,
 * window bars, and a small window sill.
 */
static void draw_window_with_scenery(int x, int y)
{
    LCD_Draw_Rect(x, y, 64, 54, CLR_DARK_BROWN, 1);

    /*
     * Outdoor sky gradient.
     * The stacked rectangles create a warm transition from amber to purple.
     */
    LCD_Draw_Rect(x + 3, y + 3,  58, 16, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 3, y + 19, 58, 10, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 3, y + 29, 58, 22, CLR_DUSK_PURPLE, 1);

    /*
     * Setting sun.
     */
    LCD_Draw_Circle(x + 48, y + 26, 6, CLR_WARM_ORANGE, 1);
    LCD_Draw_Circle(x + 48, y + 26, 4, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 48, y + 25, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 42, y + 28, 14, 1, CLR_AMBER, 1);

    /*
     * Distant mountain silhouettes.
     */
    LCD_Draw_Rect(x + 4,  y + 36, 20, 15, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 8,  y + 32, 14, 19, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 12, y + 28, 8,  23, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 28, y + 34, 26, 17, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 34, y + 28, 18, 23, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 40, y + 22, 8,  29, CLR_DARK_BROWN, 1);

    /*
     * Warm highlights on the mountain peaks.
     */
    LCD_Draw_Rect(x + 12, y + 28, 8, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 40, y + 22, 8, 1, CLR_CORAL, 1);

    /*
     * Small stars in the sky area.
     */
    LCD_Draw_Rect(x + 10, y + 8,  1, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 20, y + 14, 1, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 36, y + 10, 1, 1, CLR_CREAM, 1);

    /*
     * Window cross bars.
     */
    LCD_Draw_Rect(x + 30, y + 3,  4, 48, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 3,  y + 26, 58, 4, CLR_DARK_BROWN, 1);

    /*
     * Window sill.
     */
    LCD_Draw_Rect(x - 2, y + 52, 68, 4, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x - 2, y + 52, 68, 1, CLR_HONEY_GOLD, 1);

    /*
     * Curtains and curtain ties.
     */
    LCD_Draw_Rect(x - 4,  y,      4, 54, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 64, y,      4, 54, CLR_CORAL, 1);
    LCD_Draw_Rect(x - 5,  y + 26, 6, 2,  CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 63, y + 26, 6, 2,  CLR_SOFT_RED, 1);
}

/*
 * Draw a small framed landscape painting on the wall.
 * This adds detail to the room without using extra image assets.
 */
static void draw_painting(int x, int y)
{
    LCD_Draw_Rect(x, y, 36, 22, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 32, 18, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 3, y + 3, 30,  5, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 3, y + 8, 30,  3, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 5,  y + 8,  8, 10, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 7,  y + 6,  4, 12, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 15, y + 7, 12, 11, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 19, y + 5,  6, 13, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 19, y + 5,  6,  1, CLR_LEAF_GREEN, 1);
    LCD_Draw_Circle(x + 29, y + 6, 2, CLR_AMBER, 1);
}

/*
 * Draw a wooden bookshelf with three shelves and small book details.
 * The colourful books help the home scene feel less empty.
 */
static void draw_bookshelf(int x, int y)
{
    LCD_Draw_Rect(x, y, 28, 56, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 24, 52, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 24,  1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 14, 24, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 28, 24, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 42, 24, 2, CLR_DARK_BROWN, 1);

    /*
     * First shelf books.
     */
    LCD_Draw_Rect(x + 4,  y + 5,  3,  9, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 8,  y + 4,  3, 10, CLR_GRASS, 1);
    LCD_Draw_Rect(x + 12, y + 6,  3,  8, CLR_DENIM, 1);
    LCD_Draw_Rect(x + 16, y + 5,  3,  9, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 20, y + 4,  3, 10, CLR_CORAL, 1);

    /*
     * Second shelf books and small box.
     */
    LCD_Draw_Rect(x + 4,  y + 18, 3, 10, CLR_GRASS, 1);
    LCD_Draw_Rect(x + 8,  y + 19, 3,  9, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 14, y + 22, 6,  6, CLR_SKY, 1);
    LCD_Draw_Rect(x + 15, y + 20, 4,  2, CLR_DARK_BROWN, 1);

    /*
     * Third shelf books.
     */
    LCD_Draw_Rect(x + 4,  y + 32, 3, 10, CLR_DENIM, 1);
    LCD_Draw_Rect(x + 8,  y + 33, 3,  9, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 12, y + 32, 3, 10, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 16, y + 34, 3,  8, CLR_GRASS, 1);
    LCD_Draw_Rect(x + 20, y + 33, 3,  9, CLR_SOFT_RED, 1);

    /*
     * Bottom storage box.
     */
    LCD_Draw_Rect(x + 6,  y + 46, 16, 8, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 7,  y + 47, 14, 6, CLR_SKY, 1);
    LCD_Draw_Rect(x + 13, y + 49,  2, 2, CLR_SOFT_RED, 1);
}

/*
 * Draw a nightstand with drawers and a small alarm clock.
 */
static void draw_nightstand(int x, int y)
{
    LCD_Draw_Rect(x, y, 20, 30, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 1, y + 1, 18, 28, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 1, y + 1, 18,  1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x, y, 20, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 14, 16, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 8, y + 9,   4, 2, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 8, y + 18,  4, 2, CLR_AMBER, 1);

    /*
     * Alarm clock on top of the nightstand.
     */
    LCD_Draw_Circle(x + 10, y - 4, 4, CLR_CREAM, 1);
    LCD_Draw_Circle(x + 10, y - 4, 4, CLR_DARK_BROWN, 0);
    LCD_Draw_Rect(x + 10, y - 6, 1, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 10, y - 4, 3, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 6,  y - 9, 2, 2, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 12, y - 9, 2, 2, CLR_AMBER, 1);
}

/*
 * Draw the main bed.
 * It includes a wooden headboard, mattress, blanket, pillow, bed legs,
 * and small decorative patterns on the blanket.
 */
static void draw_bed(int x, int y)
{
    /*
     * Wooden headboard.
     */
    LCD_Draw_Rect(x, y - 15, 16, 40, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x, y - 15, 16,  3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y + 22, 16,  3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 1, y - 13, 14, 35, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 1, y - 13, 14,  1, CLR_HONEY_GOLD, 1);

    /*
     * Small headboard carving.
     */
    int sx = x + 5;
    int sy = y - 8;

    LCD_Draw_Rect(sx + 2, sy,     2, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx + 1, sy + 1, 4, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx,     sy + 2, 6, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx + 1, sy + 3, 4, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx + 2, sy + 4, 2, 1, CLR_HONEY_GOLD, 1);

    /*
     * Mattress and blanket.
     */
    LCD_Draw_Rect(x + 16, y + 18, 72, 12, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 16, y,      72, 20, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 16, y + 18, 72,  2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 16, y,      72,  1, CLR_CORAL, 1);

    /*
     * Small repeated blanket pattern.
     */
    for (int bx = x + 22; bx < x + 86; bx += 8) {
        LCD_Draw_Rect(bx, y + 5,  2, 2, CLR_AMBER, 1);
        LCD_Draw_Rect(bx, y + 12, 2, 2, CLR_AMBER, 1);
    }

    /*
     * Pillow.
     */
    LCD_Draw_Rect(x + 18, y - 3, 20, 10, CLR_SOFT_PINK, 1);
    LCD_Draw_Rect(x + 18, y - 3, 20,  2, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 18, y - 3,  2, 10, CLR_CREAM, 1);

    /*
     * Bed legs and lower frame.
     */
    LCD_Draw_Rect(x + 2,  y + 28, 3, 8, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 84, y + 28, 3, 8, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x,      y + 25, 88, 3, CLR_DARK_BROWN, 1);
}

/*
 * Draw a small warm-colour rug on the floor.
 */
static void draw_rug(int x, int y)
{
    LCD_Draw_Rect(x + 12, y,      40, 2, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 6,  y + 2,  52, 3, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x,      y + 5,  64, 6, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 6,  y + 11, 52, 3, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 12, y + 14, 40, 2, CLR_SOFT_RED, 1);

    LCD_Draw_Rect(x + 14, y + 3,  36, 10, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 8,  y + 6,  48,  4, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 18, y + 6,  28,  4, CLR_AMBER, 1);
}

/*
 * Draw a small yarn ball and loose thread.
 * This decoration works together with the sleeping cat sprite.
 */
static void draw_yarn_ball(int x, int y)
{
    LCD_Draw_Circle(x, y, 4, CLR_SOFT_PINK, 1);
    LCD_Draw_Circle(x, y, 4, CLR_SOFT_RED, 0);
    LCD_Draw_Rect(x - 3, y,  6, 1, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x, y - 3,  1, 6, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 4,  y + 2, 6, 1, CLR_SOFT_PINK, 1);
    LCD_Draw_Rect(x + 10, y + 1, 3, 1, CLR_SOFT_PINK, 1);
}

/* -------------------------------------------------------------------------- */
/* Full home scene rendering                                                   */
/* -------------------------------------------------------------------------- */

/*
 * Render the complete home scene.
 * The layout matches the original mine_home room style, but it is drawn using
 * the unified farm palette so that it fits the farm module visually.
 */
static void render_home_scene(void)
{
    LCD_Fill_Buffer(CLR_CREAM);

    /*
     * Ceiling wallpaper stripes.
     */
    for (int y = 5; y < 70; y += 10) {
        LCD_Draw_Rect(0,  y, 4,   2, CLR_CORAL, 1);
        LCD_Draw_Rect(10, y, 220, 1, CLR_CORAL, 1);
    }

    int floor_y = 180;

    /*
     * Left wall with wooden panelling.
     */
    LCD_Draw_Rect(0, 75, 126, floor_y - 75, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, 75, 126, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(0,  95, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 115, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 135, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 155, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 175, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(36, 75, 1, floor_y - 75, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(92, 75, 1, floor_y - 75, CLR_DARK_BROWN, 1);

    /*
     * Right wall with cream wallpaper behind the bed.
     */
    LCD_Draw_Rect(126, 75,  114, floor_y - 75, CLR_CREAM, 1);
    LCD_Draw_Rect(126, 80,  114, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(126, 92,  114, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(126, 104, 114, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(126, 116, 114, 1, CLR_CORAL, 1);

    /*
     * Main furniture and room decorations.
     */
    draw_window_with_scenery(148, 10);
    draw_painting(30, 25);
    draw_bookshelf(36, 124);
    draw_nightstand(86, 150);
    draw_bed(134, 152);

    /*
     * Sleeping girl sprite and floating sleep letters.
     */
    LCD_Draw_Sprite_Scaled(152, 143, 10, 8, (uint8_t *)SPR_GIRL_SLEEP, 2);
    LCD_printString("z", 181, 145, CLR_DARK_BROWN, 1);
    LCD_printString("Z", 186, 138, CLR_DARK_BROWN, 2);

    /*
     * Wooden floor planks.
     */
    LCD_Draw_Rect(0, floor_y, 240, 240 - floor_y - 18, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, floor_y, 240, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(0, 189, 240, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 201, 240, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 213, 240, 1, CLR_DARK_BROWN, 1);

    for (int px = 0; px < 240; px += 32) {
        LCD_Draw_Rect(px, floor_y, 1,  9, CLR_DARK_BROWN, 1);
        LCD_Draw_Rect(px, 202,     1, 11, CLR_DARK_BROWN, 1);
    }

    for (int px = 16; px < 240; px += 32) {
        LCD_Draw_Rect(px, 190, 1, 11, CLR_DARK_BROWN, 1);
        LCD_Draw_Rect(px, 214, 1,  8, CLR_DARK_BROWN, 1);
    }

    /*
     * Floor decorations.
     */
    draw_rug(10, 188);
    draw_yarn_ball(110, 198);
    LCD_Draw_Sprite(192, 166, 24, 12, (uint8_t *)SPR_CAT_SLEEP);

    /*
     * Bottom instruction ribbon.
     * The actual text is added later depending on the current home state.
     */
    LCD_Draw_Rect(0, 222, 240, 18, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 222, 240,  1, CLR_HONEY_GOLD, 1);
}

/*
 * Print small bold text by drawing the same string twice with a one-pixel offset.
 */
static void FarmHome_PrintBoldClean(const char *str, int x, int y, uint8_t colour)
{
    LCD_printString(str, x,     y, colour, 1);
    LCD_printString(str, x + 1, y, colour, 1);
}

/*
 * Draw a crescent moon icon for the sleep overlay.
 * The crescent is made by drawing a night-colour circle over a gold circle.
 */
static void FarmHome_DrawMoonIcon(int x, int y)
{
    LCD_Draw_Circle(x, y, 10, CLR_HONEY_GOLD, 1);
    LCD_Draw_Circle(x + 4, y - 2, 9, CLR_NIGHT_BLUE, 1);
}

/*
 * Draw a simple pixel-style four-point star.
 */
static void FarmHome_DrawStar(int x, int y, uint8_t colour)
{
    LCD_Draw_Rect(x + 1, y,     1, 5, colour, 1);
    LCD_Draw_Rect(x,     y + 2, 5, 1, colour, 1);
}

/*
 * Draw the night fade animation.
 * The screen darkens from both the top and bottom, then becomes fully dark
 * before the sleep message changes to the next-day message.
 */
static void FarmHome_DrawNightFade(uint32_t elapsed_ms)
{
    uint32_t curtain_h;

    /*
     * The maximum curtain height is 111 pixels.
     * Two curtains cover the scene from the top and bottom.
     */
    if (elapsed_ms >= 1600U) {
        curtain_h = 111U;
    } else {
        curtain_h = (elapsed_ms * 111U) / 1600U;
    }

    if (curtain_h > 0U) {
        LCD_Draw_Rect(0, 0, 240, (int)curtain_h, CLR_SKY_NIGHT, 1);
        LCD_Draw_Rect(0, 222 - (int)curtain_h, 240, (int)curtain_h, CLR_SKY_NIGHT, 1);
    }

    /*
     * After the curtains meet, fill the whole visible scene area with night blue.
     */
    if (elapsed_ms >= 1600U) {
        LCD_Draw_Rect(0, 0, 240, 222, CLR_SKY_NIGHT, 1);
    }
}

/*
 * Draw the sleep overlay animation.
 * It contains two clear message stages:
 *   Stage 1: "Sleeping..." with a soft night message.
 *   Stage 2: "DAY +1" with restored-energy feedback.
 */
static void FarmHome_DrawSleepOverlay(uint32_t elapsed_ms)
{
    uint8_t star_phase;

    /*
     * First draw the top-bottom night fade.
     */
    FarmHome_DrawNightFade(elapsed_ms);

    /*
     * Show the central sleep message after the fade has already started.
     */
    if (elapsed_ms >= 900U) {
        LCD_Draw_Rect(26, 60, 188, 92, CLR_NIGHT_BLUE, 1);
        LCD_Draw_Rect(26, 60, 188, 92, CLR_HONEY_GOLD, 0);
        LCD_Draw_Rect(30, 64, 180, 84, CLR_PANEL_BLUE, 0);

        FarmHome_DrawMoonIcon(60, 92);

        /*
         * Twinkling star positions change over time.
         */
        star_phase = (uint8_t)((HAL_GetTick() / 420U) % 3U);

        if (star_phase == 0U) {
            FarmHome_DrawStar(92, 78,   CLR_CREAM);
            FarmHome_DrawStar(170, 84,  CLR_HONEY_GOLD);
            FarmHome_DrawStar(138, 124, CLR_CREAM);
        } else if (star_phase == 1U) {
            FarmHome_DrawStar(112, 82,  CLR_HONEY_GOLD);
            FarmHome_DrawStar(184, 106, CLR_CREAM);
            FarmHome_DrawStar(88, 122,  CLR_CREAM);
        } else {
            FarmHome_DrawStar(102, 108, CLR_CREAM);
            FarmHome_DrawStar(162, 74,  CLR_HONEY_GOLD);
            FarmHome_DrawStar(192, 126, CLR_CREAM);
        }

        /*
         * Text stage 1: sleeping message.
         */
        if (elapsed_ms < 3100U) {
            FarmHome_PrintBoldClean("Sleeping...", 82, 88, CLR_CREAM);
            LCD_printString("Night falls softly", 62, 113, CLR_HONEY_GOLD, 1);
        }
        /*
         * Text stage 2: next-day message.
         */
        else {
            FarmHome_PrintBoldClean("DAY +1", 92, 88, CLR_HONEY_GOLD);
            LCD_printString("Energy restored", 70, 113, CLR_CREAM, 1);
        }
    }

    /*
     * Redraw the bottom ribbon so the instruction area stays clean.
     */
    LCD_Draw_Rect(0, 222, 240, 18, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 222, 240, 1, CLR_HONEY_GOLD, 1);

    /*
     * Bottom ribbon text changes with the sleep animation stage.
     */
    if (elapsed_ms < 3100U) {
        LCD_printString("Sleeping through the night", 34, 228, CLR_HONEY_GOLD, 1);
    } else {
        LCD_printString("Day +1   Energy restored", 40, 228, CLR_HONEY_GOLD, 1);
    }
}

/* -------------------------------------------------------------------------- */
/* Public rendering entry point                                                */
/* -------------------------------------------------------------------------- */

/*
 * Render the farm home scene.
 *
 * Parameters:
 *   sleeping_message - 0 displays the normal home scene controls.
 *                      non-zero displays the sleep transition overlay.
 */
void FarmHome_Render(uint8_t sleeping_message)
{
    static uint8_t prev_sleeping_message = 0;
    static uint32_t sleep_anim_start_ms = 0;

    uint32_t now = HAL_GetTick();

    /*
     * Use the unified farm palette before drawing any home scene graphics.
     */
    LCD_Set_Palette(PALETTE_FARM_UNIFIED);

    /*
     * Draw the full home background every frame.
     * The sleep overlay is then drawn above it when needed.
     */
    render_home_scene();

    /*
     * Record the start time only on the rising edge of the sleep state.
     */
    if (sleeping_message && !prev_sleeping_message) {
        sleep_anim_start_ms = now;
    }

    if (sleeping_message) {
        uint32_t elapsed = now - sleep_anim_start_ms;
        FarmHome_DrawSleepOverlay(elapsed);
    } else {
        LCD_printString("B2:BACK   B3:SLEEP", 38, 228, CLR_HONEY_GOLD, 1);
    }

    prev_sleeping_message = sleeping_message;
}