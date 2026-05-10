#include "mine_home.h"
#include "mine_palette.h"
#include "mine_sprites.h"
#include "mine_inventory.h"
#include "LCD.h"
#include <stdio.h>

extern uint16_t g_faint_ores_value_lost;
extern uint16_t g_faint_coins_saved;
extern uint32_t g_faint_start_tick;

#define PHASE_A_MS  2500    
#define PHASE_B_MS  500     

// =============================================================================
// Phase A: YOU FAINTED overlay
// =============================================================================
static void render_faint_overlay(void) {
    LCD_Fill_Buffer(CLR_DUSK_PURPLE);
    LCD_Draw_Rect(0, 0,   240, 28, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 212, 240, 28, CLR_DARK_BROWN, 1);

    static const uint8_t stars[][2] = {
        {30, 60}, {60, 35}, {90, 80}, {150, 45}, {200, 70},
        {220, 100}, {40, 150}, {180, 170}, {210, 180}, {60, 190},
        {110, 55}, {170, 100}, {35, 95}, {215, 145}
    };
    for (int i = 0; i < (int)(sizeof(stars) / sizeof(stars[0])); i++) {
        LCD_Draw_Rect(stars[i][0], stars[i][1], 1, 1, CLR_CREAM, 1);
    }

    LCD_printString("YOU FAINTED!", 12, 45, CLR_DANGER, 3);
    LCD_Draw_Rect(36, 72, 168, 2, CLR_DANGER, 1);
    LCD_printString("The kind villagers", 12, 90,  CLR_CREAM, 2);
    LCD_printString("found you...",       48, 112, CLR_CREAM, 2);

    LCD_Draw_Rect(28, 133, 184, 60, CLR_DARK_BROWN, 1);  
    LCD_Draw_Rect(30, 135, 180, 56, CLR_CREAM, 1);       
    LCD_Draw_Rect(30, 135, 180, 2,  CLR_HONEY_GOLD, 1);  

    char buf[40];
    LCD_Draw_Circle(48, 155, 6, CLR_AMBER, 1);
    LCD_Draw_Circle(48, 155, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Rect(48, 153, 1, 4, CLR_DARK_BROWN, 1);
    snprintf(buf, sizeof(buf), "Lost:  -%dc", g_faint_ores_value_lost);
    LCD_printString(buf, 62, 150, CLR_SOFT_RED, 2);

    LCD_Draw_Circle(48, 178, 6, CLR_AMBER, 1);
    LCD_Draw_Circle(48, 178, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Rect(48, 176, 1, 4, CLR_DARK_BROWN, 1);
    snprintf(buf, sizeof(buf), "Saved: +%dc", g_faint_coins_saved);
    LCD_printString(buf, 62, 173, CLR_HONEY_GOLD, 2);

    LCD_printString("[BT3] Wake up...", 65, 222, CLR_AMBER, 1);
}

// =============================================================================
// Helper Drawing Functions - Re-architected for new layout
// =============================================================================

// Bigger window (64x54), positioned on the right
static void draw_window_with_scenery(int x, int y) {
    LCD_Draw_Rect(x, y, 64, 54, CLR_DARK_BROWN, 1);

    // Sky gradient
    LCD_Draw_Rect(x + 3, y + 3,  58, 16, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 3, y + 19, 58, 10, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 3, y + 29, 58, 22, CLR_DUSK_PURPLE, 1);

    // Sun
    LCD_Draw_Circle(x + 48, y + 26, 6, CLR_WARM_ORANGE, 1);
    LCD_Draw_Circle(x + 48, y + 26, 4, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 48, y + 25, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 42, y + 28, 14, 1, CLR_AMBER, 1);

    // Distant mountain silhouettes 
    LCD_Draw_Rect(x + 4,  y + 36, 20, 15, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 8,  y + 32, 14, 19, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 12, y + 28, 8,  23, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 28, y + 34, 26, 17, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 34, y + 28, 18, 23, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 40, y + 22, 8,  29, CLR_DARK_BROWN, 1);

    // Warm mountain-peak highlights
    LCD_Draw_Rect(x + 12, y + 28, 8, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 40, y + 22, 8, 1, CLR_CORAL, 1);

    // Stars
    LCD_Draw_Rect(x + 10, y + 8,  1, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 20, y + 14, 1, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 36, y + 10, 1, 1, CLR_CREAM, 1);

    // Window cross 
    LCD_Draw_Rect(x + 30, y + 3,  4, 48, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 3,  y + 26, 58, 4, CLR_DARK_BROWN, 1);

    // Window sill
    LCD_Draw_Rect(x - 2, y + 52, 68, 4, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x - 2, y + 52, 68, 1, CLR_HONEY_GOLD, 1);

    // Curtains
    LCD_Draw_Rect(x - 4, y,      4, 54, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 64, y,     4, 54, CLR_CORAL, 1);
    LCD_Draw_Rect(x - 5, y + 26, 6, 2,  CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 63, y + 26, 6, 2, CLR_SOFT_RED, 1);
}

// Painting moved to the left
static void draw_painting(int x, int y) {
    LCD_Draw_Rect(x, y, 36, 22, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 32, 18, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 3, y + 3, 30, 5, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 3, y + 8, 30, 3, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 5,  y + 8, 8, 10, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 7,  y + 6, 4, 12, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 15, y + 7, 12, 11, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 19, y + 5, 6,  13, CLR_SAGE, 1);
    LCD_Draw_Rect(x + 19, y + 5, 6, 1,  CLR_LEAF_GREEN, 1);
    LCD_Draw_Circle(x + 29, y + 6, 2, CLR_AMBER, 1);
}

static void draw_bookshelf(int x, int y) {
    LCD_Draw_Rect(x, y, 28, 56, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 24, 52, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 24, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 14, 24, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 28, 24, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 42, 24, 2, CLR_DARK_BROWN, 1);

    LCD_Draw_Rect(x + 4,  y + 5,  3, 9,  CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 8,  y + 4,  3, 10, CLR_GRASS, 1);
    LCD_Draw_Rect(x + 12, y + 6,  3, 8,  CLR_DENIM, 1);
    LCD_Draw_Rect(x + 16, y + 5,  3, 9,  CLR_AMBER, 1);
    LCD_Draw_Rect(x + 20, y + 4,  3, 10, CLR_CORAL, 1);

    LCD_Draw_Rect(x + 4,  y + 18, 3, 10, CLR_GRASS, 1);
    LCD_Draw_Rect(x + 8,  y + 19, 3, 9,  CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 14, y + 22, 6, 6,  CLR_SKY, 1);
    LCD_Draw_Rect(x + 15, y + 20, 4, 2,  CLR_DARK_BROWN, 1);

    LCD_Draw_Rect(x + 4,  y + 32, 3, 10, CLR_DENIM, 1);
    LCD_Draw_Rect(x + 8,  y + 33, 3, 9,  CLR_AMBER, 1);
    LCD_Draw_Rect(x + 12, y + 32, 3, 10, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 16, y + 34, 3, 8,  CLR_GRASS, 1);
    LCD_Draw_Rect(x + 20, y + 33, 3, 9,  CLR_SOFT_RED, 1);

    LCD_Draw_Rect(x + 6, y + 46, 16, 8, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 7, y + 47, 14, 6, CLR_SKY, 1);
    LCD_Draw_Rect(x + 13, y + 49, 2, 2, CLR_SOFT_PINK, 1);
}

static void draw_nightstand(int x, int y) {
    LCD_Draw_Rect(x, y, 20, 30, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 1, y + 1, 18, 28, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 1, y + 1, 18, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x, y, 20, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 14, 16, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 8, y + 9,  4, 2, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 8, y + 18, 4, 2, CLR_AMBER, 1);

    LCD_Draw_Circle(x + 10, y - 4, 4, CLR_CREAM, 1);
    LCD_Draw_Circle(x + 10, y - 4, 4, CLR_DARK_BROWN, 0);
    LCD_Draw_Rect(x + 10, y - 6, 1, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 10, y - 4, 3, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 6, y - 9,  2, 2, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 12, y - 9, 2, 2, CLR_AMBER, 1);
}

static void draw_bed(int x, int y) {
    LCD_Draw_Rect(x, y - 15, 16, 40, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x, y - 15, 16, 3,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y + 22, 16, 3,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 1, y - 13, 14, 35, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 1, y - 13, 14, 1, CLR_HONEY_GOLD, 1);

    int sx = x + 5, sy = y - 8;
    LCD_Draw_Rect(sx + 2, sy,     2, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx + 1, sy + 1, 4, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx,     sy + 2, 6, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx + 1, sy + 3, 4, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(sx + 2, sy + 4, 2, 1, CLR_HONEY_GOLD, 1);

    LCD_Draw_Rect(x + 16, y + 18, 72, 12, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 16, y, 72, 20, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 16, y + 18, 72, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 16, y, 72, 1, CLR_CORAL, 1);

    for (int bx = x + 22; bx < x + 86; bx += 8) {
        LCD_Draw_Rect(bx, y + 5,  2, 2, CLR_AMBER, 1);
        LCD_Draw_Rect(bx, y + 12, 2, 2, CLR_AMBER, 1);
    }

    LCD_Draw_Rect(x + 18, y - 3, 20, 10, CLR_SOFT_PINK, 1);
    LCD_Draw_Rect(x + 18, y - 3, 20, 2,  CLR_CREAM, 1);
    LCD_Draw_Rect(x + 18, y - 3, 2, 10,  CLR_CREAM, 1);

    LCD_Draw_Rect(x + 2,  y + 28, 3, 8, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 84, y + 28, 3, 8, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y + 25, 88, 3, CLR_DARK_BROWN, 1);
}

// Re-designed taller and puffier rug
static void draw_rug(int x, int y) {
    LCD_Draw_Rect(x + 12, y,      40, 2, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 6,  y + 2,  52, 3, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x,      y + 5,  64, 6, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 6,  y + 11, 52, 3, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 12, y + 14, 40, 2, CLR_SOFT_RED, 1);
    
    LCD_Draw_Rect(x + 14, y + 3,  36, 10, CLR_CORAL, 1);
    LCD_Draw_Rect(x + 8,  y + 6,  48, 4,  CLR_CORAL, 1);
    LCD_Draw_Rect(x + 18, y + 6,  28, 4,  CLR_AMBER, 1);
}

static void draw_yarn_ball(int x, int y) {
    LCD_Draw_Circle(x, y, 4, CLR_SOFT_PINK, 1);
    LCD_Draw_Circle(x, y, 4, CLR_SOFT_RED, 0);
    LCD_Draw_Rect(x - 3, y, 6, 1, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x, y - 3, 1, 6, CLR_SOFT_RED, 1);
    LCD_Draw_Rect(x + 4, y + 2, 6, 1, CLR_SOFT_PINK, 1);
    LCD_Draw_Rect(x + 10, y + 1, 3, 1, CLR_SOFT_PINK, 1);
}

// =============================================================================
// Phase C: Full home scene (NEW LAYOUT)
// =============================================================================
static void render_home_scene(void) {
    LCD_Fill_Buffer(CLR_CREAM);

    for (int y = 5; y < 70; y += 10) {
        LCD_Draw_Rect(0, y, 4, 2, CLR_CORAL, 1);
        LCD_Draw_Rect(10, y, 220, 1, CLR_CORAL, 1);
    }

    // 1. FLOOR LEVEL MOVED UP to Y=180 (Height=42)
    int floor_y = 180;
    
    // Left wall: Wood Paneling
    LCD_Draw_Rect(0, 75, 126, floor_y - 75, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, 75, 126, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(0, 95,  126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 115, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 135, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 155, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 175, 126, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(36,  75, 1, floor_y - 75, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(92, 75, 1, floor_y - 75, CLR_DARK_BROWN, 1);

    // Right wall: Cream wallpaper behind bed
    LCD_Draw_Rect(126, 75, 114, floor_y - 75, CLR_CREAM, 1);
    LCD_Draw_Rect(126, 80,  114, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(126, 92,  114, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(126, 104, 114, 1, CLR_CORAL, 1);
    LCD_Draw_Rect(126, 116, 114, 1, CLR_CORAL, 1);

    // 2. SWAPPED & BIGGER WINDOW/PAINTING
    draw_window_with_scenery(148, 10); 
    draw_painting(30, 25);            

    // 3. BOOKSHELF & NIGHTSTAND (Shifted slightly for new floor)
    draw_bookshelf(36, 124);          // Bottom is at 180
    draw_nightstand(86, 150);         // Bottom is at 180

    // 4. BED 
    draw_bed(134, 152);               // Legs reach 180

    // Sleeping girl
    LCD_Draw_Sprite_Scaled(152, 143, 10, 8, (uint8_t*)SPR_GIRL_SLEEP, 2);
    LCD_printString("z", 181, 145, CLR_DARK_BROWN, 1);
    LCD_printString("Z", 186, 138, CLR_DARK_BROWN, 2);

    // FLOOR PLANKS
    LCD_Draw_Rect(0, floor_y, 240, 240 - floor_y - 18, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, floor_y, 240, 1, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(0, 189, 240, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 201, 240, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 213, 240, 1, CLR_DARK_BROWN, 1);
    for (int px = 0; px < 240; px += 32) {
        LCD_Draw_Rect(px, floor_y, 1, 9,  CLR_DARK_BROWN, 1);   
        LCD_Draw_Rect(px, 202, 1, 11, CLR_DARK_BROWN, 1);
    }
    for (int px = 16; px < 240; px += 32) {
        LCD_Draw_Rect(px, 190, 1, 11, CLR_DARK_BROWN, 1);  
        LCD_Draw_Rect(px, 214, 1, 8, CLR_DARK_BROWN, 1);
    }

    // 5. RUG & CAT & YARN
    draw_rug(10, 188);
    draw_yarn_ball(110, 198);
    
    // CRITICAL FIX: The cat is 24 width x 12 height. 
    // It was passed backwards before (12, 24).
    LCD_Draw_Sprite(192, 166, 24, 12, (uint8_t*)SPR_CAT_SLEEP);

    // Bottom text ribbon
    LCD_Draw_Rect(0, 222, 240, 18, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 222, 240, 1,  CLR_HONEY_GOLD, 1);  
    LCD_printString("Rest well, little miner...", 24, 228, CLR_HONEY_GOLD, 1);
}

void home_render(uint32_t now_ms) {
    uint32_t elapsed = now_ms - g_faint_start_tick;

    if (elapsed < PHASE_A_MS) {
        render_faint_overlay();
    }
    else if (elapsed < PHASE_A_MS + PHASE_B_MS) {
        render_home_scene();
        uint32_t phase_b_elapsed = elapsed - PHASE_A_MS;
        uint32_t fade_pct = (100 * phase_b_elapsed) / PHASE_B_MS;

        uint8_t step = (fade_pct < 25) ? 2 :
                       (fade_pct < 50) ? 3 :
                       (fade_pct < 75) ? 5 : 8;
        for (int y = 0; y < 240; y += step) {
            LCD_Draw_Rect(0, y, 240, 1, CLR_DUSK_PURPLE, 1);
        }
    }
    else {
        render_home_scene();
    }
}