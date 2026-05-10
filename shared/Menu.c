#include "Menu.h"
#include "LCD.h"
#include "InputHandler.h"
#include "Joystick.h"
#include "Buzzer.h"
#include "PWM.h"
#include "Economy.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include <stdint.h>

extern ST7789V2_cfg_t cfg0;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern Buzzer_cfg_t buzzer_cfg;
extern PWM_cfg_t pwm_cfg;

// Menu options
static const char* menu_options[] = {
    "FARM",
    "LAKE",
    "MINE"
};
#define NUM_MENU_OPTIONS 3

#define MENU_FRAME_TIME_MS 30

// =============================================================================
// OPENING ANIMATION
// =============================================================================

#define INTRO_TOTAL_MS   12000
#define INTRO_FRAME_MS   33

#define C_BLACK     0
#define C_CREAM     1
#define C_SOFT_RED  2
#define C_ORANGE    5
#define C_AMBER     6
#define C_PURPLE    8
#define C_DBROWN    9
#define C_GOLD      10
#define C_SAGE      11
#define C_TBROWN    12
#define C_SKY       14
#define C_CORAL     15

static const uint8_t intro_stars[][2] = {
    { 18,  10}, { 42,  26}, { 68,  14}, { 96,  30},
    {128,   8}, {152,  32}, {188,  18}, {218,  28},
    { 32,  50}, { 84,  58}, {140,  50}, {196,  62},
    {110,  55}, {170,  68},
};
#define INTRO_NUM_STARS  (sizeof(intro_stars) / sizeof(intro_stars[0]))

static void intro_draw_sky(uint32_t t_ms) {
    if (t_ms < 1500) {
        LCD_Fill_Buffer(C_PURPLE);
    }
    else if (t_ms < 3000) {
        uint32_t pct = ((t_ms - 1500) * 100) / 1500;
        int amber_top = 180 - (int)(pct * 60) / 100;
        int coral_top = amber_top - 20;
        if (coral_top < 0) coral_top = 0;

        LCD_Draw_Rect(0, 0,          240, coral_top,         C_PURPLE, 1);
        LCD_Draw_Rect(0, coral_top,  240, 20,                C_CORAL,  1);
        LCD_Draw_Rect(0, amber_top,  240, 240 - amber_top,   C_AMBER,  1);
    }
    else if (t_ms < 4500) {
        uint32_t pct = ((t_ms - 3000) * 100) / 1500;
        uint8_t upper = (pct < 50) ? C_PURPLE : C_CORAL;
        LCD_Draw_Rect(0, 0,   240, 80,  upper,   1);
        LCD_Draw_Rect(0, 80,  240, 40,  C_CORAL, 1);
        LCD_Draw_Rect(0, 120, 240, 20,  C_AMBER, 1);
        LCD_Draw_Rect(0, 140, 240, 100, C_SAGE,  1);
    }
    else {
        LCD_Draw_Rect(0, 0,   240, 60,  C_CORAL, 1);
        LCD_Draw_Rect(0, 60,  240, 40,  C_AMBER, 1);
        LCD_Draw_Rect(0, 100, 240, 40,  C_CREAM, 1);
        LCD_Draw_Rect(0, 140, 240, 100, C_SAGE,  1);
    }
}

static void intro_draw_stars_moon(uint32_t t_ms) {
    if (t_ms >= 2800) return;
    uint32_t visible_count = INTRO_NUM_STARS;
    if (t_ms > 2000) {
        uint32_t fade_pct = ((t_ms - 2000) * 100) / 800;
        visible_count = (INTRO_NUM_STARS * (100 - fade_pct)) / 100;
    }
    uint8_t twinkle = (t_ms / 200) % 3;
    for (uint32_t i = 0; i < visible_count; i++) {
        if ((i % 3) == twinkle) continue;
        LCD_Draw_Rect(intro_stars[i][0], intro_stars[i][1], 2, 2, C_CREAM, 1);
    }
    if (t_ms < 2200) {
        LCD_Draw_Circle(196, 36, 10, C_CREAM, 1);
        LCD_Draw_Circle(192, 34, 9,  C_PURPLE, 1);
    }
}

static void intro_draw_mountains(uint32_t t_ms) {
    uint8_t base_col, peak_highlight;
    if (t_ms < 1500) {
        base_col = C_BLACK;
        peak_highlight = C_BLACK;
    } else if (t_ms < 3000) {
        base_col = C_DBROWN;
        peak_highlight = C_CORAL;
    } else {
        base_col = C_TBROWN;
        peak_highlight = 0;
    }

    LCD_Draw_Rect(0,   140, 40, 20, base_col, 1);
    LCD_Draw_Rect(10,  130, 30, 30, base_col, 1);
    LCD_Draw_Rect(20,  120, 20, 40, base_col, 1);
    LCD_Draw_Rect(40,  148, 40, 12, base_col, 1);
    LCD_Draw_Rect(80,  140, 40, 20, base_col, 1);
    LCD_Draw_Rect(100, 128, 24, 32, base_col, 1);
    LCD_Draw_Rect(110, 118, 16, 42, base_col, 1);
    LCD_Draw_Rect(140, 148, 40, 12, base_col, 1);
    LCD_Draw_Rect(150, 136, 24, 24, base_col, 1);
    LCD_Draw_Rect(180, 144, 60, 16, base_col, 1);
    LCD_Draw_Rect(200, 134, 34, 26, base_col, 1);

    if (peak_highlight != 0) {
        LCD_Draw_Rect(20,  120, 20, 2, peak_highlight, 1);
        LCD_Draw_Rect(110, 118, 16, 2, peak_highlight, 1);
        LCD_Draw_Rect(150, 136, 24, 2, peak_highlight, 1);
        LCD_Draw_Rect(200, 134, 34, 2, peak_highlight, 1);
    }
}

static void intro_draw_sun(uint32_t t_ms) {
    if (t_ms < 1500) return;
    int sun_y;
    if (t_ms < 4500) {
        uint32_t pct = ((t_ms - 1500) * 100) / 3000;
        sun_y = 170 - (int)((pct * 110) / 100);
    } else {
        sun_y = 60;
    }
    if (sun_y > -20) {
        LCD_Draw_Circle(200, sun_y, 14, C_AMBER, 1);
        LCD_Draw_Circle(200, sun_y, 10, C_GOLD,  1);
        LCD_Draw_Circle(198, sun_y - 2, 4, C_CREAM, 1);
    }
}

static void intro_draw_villager(uint32_t t_ms) {
    if (t_ms < 3000) return;
    int vx;
    if (t_ms < 6500) {
        uint32_t pct = ((t_ms - 3000) * 100) / 3500;
        vx = -20 + (int)((pct * 150) / 100);
    } else {
        vx = 130;
    }
    int vy = 158;
    uint8_t leg_frame = 0;
    if (t_ms < 6500) leg_frame = (t_ms / 300) & 1;

    LCD_Draw_Rect(vx + 3, vy,     8, 2,  C_CREAM,  1);
    LCD_Draw_Rect(vx + 2, vy + 2, 10, 1, C_CREAM,  1);
    LCD_Draw_Rect(vx + 3, vy + 3, 8, 4,  C_ORANGE, 1);
    LCD_Draw_Rect(vx + 4, vy + 5, 1, 1,  C_DBROWN, 1);
    LCD_Draw_Rect(vx + 9, vy + 5, 1, 1,  C_DBROWN, 1);
    LCD_Draw_Rect(vx + 2, vy + 7, 10, 7, C_SOFT_RED, 1);
    LCD_Draw_Rect(vx + 5, vy + 7, 4, 3,  C_CREAM, 1);
    LCD_Draw_Rect(vx + 1,  vy + 8,  1, 4, C_ORANGE, 1);
    LCD_Draw_Rect(vx + 12, vy + 8,  1, 4, C_ORANGE, 1);

    if (leg_frame == 0) {
        LCD_Draw_Rect(vx + 3, vy + 14, 3, 5, C_DBROWN, 1);
        LCD_Draw_Rect(vx + 8, vy + 14, 3, 5, C_DBROWN, 1);
    } else {
        LCD_Draw_Rect(vx + 2, vy + 14, 3, 5, C_DBROWN, 1);
        LCD_Draw_Rect(vx + 9, vy + 14, 3, 5, C_DBROWN, 1);
    }
}

static void intro_draw_cottage(uint32_t t_ms) {
    if (t_ms < 4500) return;
    int cx;
    if (t_ms < 6500) {
        uint32_t pct = ((t_ms - 4500) * 100) / 2000;
        cx = 280 - (int)((pct * 130) / 100);
    } else {
        cx = 150;
    }
    int cy = 150;

    LCD_Draw_Rect(cx,     cy + 3, 48, 30, C_CREAM, 1);
    LCD_Draw_Rect(cx,     cy + 3, 48, 1,  C_GOLD,  1);
    LCD_Draw_Rect(cx - 2, cy,     52, 3, C_CORAL, 1);
    LCD_Draw_Rect(cx + 2, cy - 3, 44, 3, C_CORAL, 1);
    LCD_Draw_Rect(cx + 6, cy - 6, 36, 3, C_CORAL, 1);
    LCD_Draw_Rect(cx + 12, cy - 9, 24, 3, C_CORAL, 1);
    LCD_Draw_Rect(cx + 18, cy - 12, 12, 3, C_CORAL, 1);
    LCD_Draw_Rect(cx + 36, cy - 14, 6, 8, C_TBROWN, 1);
    LCD_Draw_Rect(cx + 36, cy - 14, 6, 2, C_DBROWN, 1);

    uint32_t smoke_t = (t_ms - 4500) / 150;
    for (int s = 0; s < 3; s++) {
        int phase = s * 6;
        int offset = (smoke_t + phase) % 18;
        int sy = cy - 18 - offset;
        int sx = cx + 39 + ((s & 1) ? 2 : -2);
        if (sy >= 10) LCD_Draw_Circle(sx, sy, 2, C_CREAM, 1);
    }

    LCD_Draw_Rect(cx + 20, cy + 14, 10, 19, C_TBROWN, 1);
    LCD_Draw_Rect(cx + 20, cy + 14, 10, 2,  C_DBROWN, 1);
    LCD_Draw_Rect(cx + 27, cy + 23, 2, 2,   C_GOLD,   1);
    LCD_Draw_Rect(cx + 4, cy + 8, 10, 10, C_AMBER,  1);
    LCD_Draw_Rect(cx + 8, cy + 8, 2, 10,  C_TBROWN, 1);
    LCD_Draw_Rect(cx + 4, cy + 12, 10, 2, C_TBROWN, 1);
    LCD_Draw_Rect(cx + 34, cy + 8, 10, 10, C_AMBER,  1);
    LCD_Draw_Rect(cx + 38, cy + 8, 2, 10,  C_TBROWN, 1);
    LCD_Draw_Rect(cx + 34, cy + 12, 10, 2, C_TBROWN, 1);
}

static void intro_draw_grass_details(uint32_t t_ms) {
    if (t_ms < 3000) return;
    static const uint8_t tufts[][2] = {
        { 22, 150}, { 48, 150}, { 72, 152}, { 92, 150},
        {160, 152}, {210, 150}, { 12, 180}, { 50, 180},
        { 88, 182}, {120, 180}, {162, 182}, {198, 180},
        { 30, 210}, { 70, 212}, {110, 210}, {180, 212},
    };
    for (size_t i = 0; i < sizeof(tufts) / sizeof(tufts[0]); i++) {
        LCD_Draw_Rect(tufts[i][0], tufts[i][1], 2, 2, C_DBROWN, 1);
    }
}

static void intro_draw_title_star(int x, int y, uint8_t colour) {
    LCD_Draw_Rect(x + 2, y,     1, 5, colour, 1);
    LCD_Draw_Rect(x,     y + 2, 5, 1, colour, 1);
}

static void intro_draw_wheat(int x, int y, int dir) {
    LCD_Draw_Rect(x, y, 1, 28, C_GOLD, 1);

    if (dir < 0) {
        LCD_Draw_Rect(x - 4, y + 3,  4, 2, C_AMBER, 1);
        LCD_Draw_Rect(x - 5, y + 8,  5, 2, C_GOLD,  1);
        LCD_Draw_Rect(x - 4, y + 13, 4, 2, C_AMBER, 1);
        LCD_Draw_Rect(x - 5, y + 18, 5, 2, C_GOLD,  1);
        LCD_Draw_Rect(x - 4, y + 23, 4, 2, C_AMBER, 1);
    } else {
        LCD_Draw_Rect(x + 1, y + 3,  4, 2, C_AMBER, 1);
        LCD_Draw_Rect(x + 1, y + 8,  5, 2, C_GOLD,  1);
        LCD_Draw_Rect(x + 1, y + 13, 4, 2, C_AMBER, 1);
        LCD_Draw_Rect(x + 1, y + 18, 5, 2, C_GOLD,  1);
        LCD_Draw_Rect(x + 1, y + 23, 4, 2, C_AMBER, 1);
    }
}

static void intro_draw_title(uint32_t t_ms) {
    if (t_ms < 7400) return;

    uint32_t local_t = t_ms - 7400;
    int board_y;
    int board_h;

    if (local_t < 520) {
        board_y = 88 - (int)((local_t * 26U) / 520U);
        board_h = 70 + (int)((local_t * 48U) / 520U);
    } else {
        board_y = 62;
        board_h = 118;
    }

    /*
       Full-width animated title board.
       The top stays low enough to leave part of the sun visible.
    */
    LCD_Draw_Rect(0, board_y, 240, board_h, C_DBROWN, 1);
    LCD_Draw_Rect(2, board_y + 2, 236, board_h - 4, C_TBROWN, 1);
    LCD_Draw_Rect(6, board_y + 6, 228, board_h - 12, C_PURPLE, 1);

    LCD_Draw_Rect(6,   board_y + 6,             228, 3, C_GOLD, 1);
    LCD_Draw_Rect(6,   board_y + board_h - 9,   228, 3, C_GOLD, 1);
    LCD_Draw_Rect(6,   board_y + 6,             3, board_h - 12, C_GOLD, 1);
    LCD_Draw_Rect(231, board_y + 6,             3, board_h - 12, C_GOLD, 1);

    intro_draw_title_star(26,  board_y + 22, C_CREAM);
    intro_draw_title_star(210, board_y + 22, C_CREAM);
    intro_draw_title_star(26,  board_y + board_h - 30, C_GOLD);
    intro_draw_title_star(210, board_y + board_h - 30, C_GOLD);

    if (local_t < 620) return;

    LCD_printString("COZY",    84, board_y + 16, C_GOLD, 3);
    LCD_printString("VILLAGE", 55, board_y + 43, C_GOLD, 3);

    LCD_printString("A Tiny Adventure", 70, board_y + 75, C_CREAM, 1);
    LCD_printString("A Tiny Adventure", 71, board_y + 75, C_CREAM, 1);

    if (((t_ms - 7400) / 700) & 1) {
        LCD_printString("Press JOY to start", 64, board_y + 94, C_GOLD, 1);
        LCD_printString("Press JOY to start", 65, board_y + 94, C_GOLD, 1);
    }
}

/* Gentle opening melody.
   Frequency 0 means a short rest.
   This replaces the previous three single notes with a softer village tune.
*/
#define INTRO_MUSIC_VOL       1
#define INTRO_MUSIC_START_MS  700

static const uint16_t INTRO_MELODY_FREQS[] = {
    523, 659, 784, 659, 0,
    587, 698, 880, 784, 0,
    659, 784, 1047, 784, 0,
    698, 659, 587, 523, 0,

    523, 659, 784, 659, 587, 523, 0,
    587, 698, 880, 784, 698, 587, 0,
    659, 784, 1047, 988, 880, 784, 0,
    698, 659, 587, 523, 392, 523, 0
};

static const uint16_t INTRO_MELODY_DURS[] = {
    360, 360, 520, 460, 160,
    360, 360, 520, 520, 180,
    360, 360, 620, 520, 180,
    360, 360, 420, 720, 260,

    300, 300, 430, 300, 320, 620, 180,
    300, 300, 430, 300, 320, 620, 200,
    320, 320, 560, 300, 300, 620, 220,
    320, 320, 360, 520, 320, 760, 300
};

#define INTRO_MELODY_LEN \
    ((uint8_t)(sizeof(INTRO_MELODY_FREQS) / sizeof(INTRO_MELODY_FREQS[0])))

static void intro_update_led_and_sound(uint32_t t_ms, uint8_t* note_played, uint32_t* note_off_tick) {
    if (t_ms < 6000) {
        uint8_t duty = (uint8_t)((t_ms * 100) / 6000);
        if (duty > 100) duty = 100;
        PWM_SetDuty(&pwm_cfg, duty);
    } else {
        PWM_SetDuty(&pwm_cfg, 100);
    }

    if (t_ms < INTRO_MUSIC_START_MS) {
        return;
    }

    if (*note_off_tick == 0U) {
        *note_played = 0U;

        if (INTRO_MELODY_FREQS[*note_played] == 0U) {
            buzzer_off(&buzzer_cfg);
        } else {
            buzzer_tone(&buzzer_cfg,
                        INTRO_MELODY_FREQS[*note_played],
                        INTRO_MUSIC_VOL);
        }

        *note_off_tick = HAL_GetTick() + INTRO_MELODY_DURS[*note_played];
        return;
    }

    if (HAL_GetTick() < *note_off_tick) {
        return;
    }

    (*note_played)++;
    if (*note_played >= INTRO_MELODY_LEN) {
        *note_played = 0U;
    }

    if (INTRO_MELODY_FREQS[*note_played] == 0U) {
        buzzer_off(&buzzer_cfg);
    } else {
        buzzer_tone(&buzzer_cfg,
                    INTRO_MELODY_FREQS[*note_played],
                    INTRO_MUSIC_VOL);
    }

    *note_off_tick = HAL_GetTick() + INTRO_MELODY_DURS[*note_played];
}


void Menu_RunIntro(void) {
    LCD_Set_Palette(PALETTE_CUSTOM);
    uint32_t start = HAL_GetTick();
    uint8_t note_played = 0;
    uint32_t note_off_tick = 0;
    PWM_SetDuty(&pwm_cfg, 0);

    while (1) {
        uint32_t frame_start = HAL_GetTick();
        uint32_t t_ms = frame_start - start;

        Input_Read();
        if (current_input.joy_pressed) break;

        intro_draw_sky(t_ms);
        intro_draw_stars_moon(t_ms);
        intro_draw_mountains(t_ms);
        intro_draw_sun(t_ms);
        intro_draw_grass_details(t_ms);
        intro_draw_cottage(t_ms);
        intro_draw_villager(t_ms);
        intro_draw_title(t_ms);
        LCD_Refresh(&cfg0);

        intro_update_led_and_sound(t_ms, &note_played, &note_off_tick);

        uint32_t elapsed = HAL_GetTick() - frame_start;
        if (elapsed < INTRO_FRAME_MS) HAL_Delay(INTRO_FRAME_MS - elapsed);
    }
    buzzer_off(&buzzer_cfg);
    PWM_SetDuty(&pwm_cfg, 100);
}

// =============================================================================
// HOME MENU - 2D Village Map
// =============================================================================

typedef struct {
    int16_t cx;
    int16_t cy;
    int16_t hw;  
    int16_t hh;  
} BuildingHitbox;

static const BuildingHitbox buildings[3] = {
    { 50,   78, 30, 22 },   
    { 185,  78, 32, 22 },   
    { 120, 170, 28, 22 },   
};

static int16_t villager_x = 120;
static int16_t villager_y = 130;
static uint8_t villager_anim = 0;
static uint32_t villager_last_anim_tick = 0;

#define WALK_X_MIN   6
#define WALK_X_MAX   226
#define WALK_Y_MIN   34
#define WALK_Y_MAX   208

static int8_t villager_on_building(int16_t cx, int16_t cy) {
    for (int i = 0; i < 3; i++) {
        int16_t dx = (cx > buildings[i].cx) ? (cx - buildings[i].cx) : (buildings[i].cx - cx);
        int16_t dy = (cy > buildings[i].cy) ? (cy - buildings[i].cy) : (buildings[i].cy - cy);
        if (dx <= buildings[i].hw + 12 && dy <= buildings[i].hh + 12) {
            return (int8_t)i;
        }
    }
    if (cx < 45 && cy > 185) {
        return 3; 
    }
    return -1;
}


static void draw_villager(int16_t cx, int16_t cy, uint8_t anim_frame) {
    int vx = cx - 7;  
    int vy = cy - 10; 
    LCD_Draw_Rect(vx + 3, vy,     8, 2,  C_CREAM, 1);
    LCD_Draw_Rect(vx + 2, vy + 2, 10, 1, C_CREAM, 1);
    LCD_Draw_Rect(vx + 3, vy + 3, 8, 4,  C_ORANGE, 1);
    LCD_Draw_Rect(vx + 4, vy + 5, 1, 1,  C_DBROWN, 1);
    LCD_Draw_Rect(vx + 9, vy + 5, 1, 1,  C_DBROWN, 1);
    LCD_Draw_Rect(vx + 2, vy + 7, 10, 7, C_SOFT_RED, 1);
    LCD_Draw_Rect(vx + 5, vy + 7, 4, 3,  C_CREAM, 1);
    LCD_Draw_Rect(vx + 1,  vy + 8, 1, 4, C_ORANGE, 1);
    LCD_Draw_Rect(vx + 12, vy + 8, 1, 4, C_ORANGE, 1);
    if (anim_frame == 0) {
        LCD_Draw_Rect(vx + 3, vy + 14, 3, 5, C_DBROWN, 1);
        LCD_Draw_Rect(vx + 8, vy + 14, 3, 5, C_DBROWN, 1);
    } else {
        LCD_Draw_Rect(vx + 2, vy + 14, 3, 5, C_DBROWN, 1);
        LCD_Draw_Rect(vx + 9, vy + 14, 3, 5, C_DBROWN, 1);
    }
}

static void draw_farm(void) {
    int bx = 22;  
    int by = 60;  
    LCD_Draw_Rect(bx,      by,      36, 30, C_SOFT_RED, 1);
    LCD_Draw_Rect(bx - 3,  by - 4,  42, 4,  C_DBROWN, 1);
    LCD_Draw_Rect(bx + 2,  by - 7,  32, 3,  C_DBROWN, 1);
    LCD_Draw_Rect(bx + 8,  by - 10, 20, 3,  C_DBROWN, 1);
    LCD_Draw_Rect(bx + 14, by + 14, 8, 16, C_CREAM, 1);
    LCD_Draw_Rect(bx + 20, by + 22, 1, 2,  C_DBROWN, 1);
    LCD_Draw_Rect(bx + 3,  by + 4,  30, 1,  C_CREAM, 1);
    LCD_Draw_Rect(bx + 18, by + 2,  1,  10, C_CREAM, 1);
    LCD_Draw_Rect(bx + 4,  by + 6,  8, 6,  C_AMBER, 1);
    LCD_Draw_Rect(bx + 24, by + 6,  8, 6,  C_AMBER, 1);
    LCD_Draw_Rect(bx + 40, by + 20, 12, 10, C_GOLD, 1);
    LCD_Draw_Rect(bx + 40, by + 22, 12, 1,  C_AMBER, 1);
    LCD_Draw_Rect(bx + 40, by + 26, 12, 1,  C_AMBER, 1);
}

static void draw_lake(uint32_t t_ms) {
    LCD_Draw_Rect(160, 62, 50, 30, C_SKY, 1);
    LCD_Draw_Rect(156, 66, 58, 22, C_SKY, 1);
    LCD_Draw_Rect(158, 64, 54, 26, C_SKY, 1);
    uint8_t wave = (t_ms / 300) & 1;
    LCD_Draw_Rect(162 + wave * 4, 72, 10, 1, C_CREAM, 1);
    LCD_Draw_Rect(180 - wave * 4, 80, 12, 1, C_CREAM, 1);
    LCD_Draw_Rect(200 + wave * 2, 76, 8,  1, C_CREAM, 1);
    LCD_Draw_Rect(148, 76, 16, 6, C_TBROWN, 1);
    LCD_Draw_Rect(148, 76, 16, 1, C_GOLD,   1);
    LCD_Draw_Rect(150, 76, 2, 10, C_DBROWN, 1);
    LCD_Draw_Rect(160, 76, 2, 10, C_DBROWN, 1);
    LCD_Draw_Rect(195, 82, 4, 2, C_CORAL, 1);
    LCD_Draw_Rect(199, 81, 2, 4, C_CORAL, 1);
}

static void draw_mine(uint32_t t_ms) {
    LCD_Draw_Rect(92, 148, 56, 44, C_TBROWN, 1);
    LCD_Draw_Rect(92, 148, 56, 2,  C_GOLD, 1);
    LCD_Draw_Rect(96, 150, 4, 4,  C_DBROWN, 1);
    LCD_Draw_Rect(138, 150, 4, 4, C_DBROWN, 1);
    LCD_Draw_Rect(100, 178, 6, 4, C_DBROWN, 1);
    LCD_Draw_Rect(134, 180, 6, 4, C_DBROWN, 1);
    LCD_Draw_Circle(120, 168, 14, C_BLACK, 1);
    LCD_Draw_Rect(106, 168, 28, 24, C_BLACK, 1);
    LCD_Draw_Rect(104, 168, 2, 24, C_DBROWN, 1);
    LCD_Draw_Rect(134, 168, 2, 24, C_DBROWN, 1);
    LCD_Draw_Rect(104, 166, 32, 2, C_DBROWN, 1);
    uint8_t flicker = ((t_ms / 400) & 1);
    LCD_Draw_Rect(148, 150, 2, 6, C_DBROWN, 1);  
    LCD_Draw_Rect(145, 156, 8, 6, C_TBROWN, 1);  
    LCD_Draw_Rect(146, 157, 6, 4, flicker ? C_AMBER : C_GOLD, 1);
    LCD_Draw_Rect(88, 180, 2, 12, C_TBROWN, 1);
    LCD_Draw_Rect(84, 178, 8, 3,  C_CREAM, 1);
}

static void draw_paths(void) {
    LCD_Draw_Rect(80, 100, 76, 6, C_GOLD, 1);
    LCD_Draw_Rect(80, 101, 76, 1, C_CREAM, 1);
    LCD_Draw_Rect(116, 106, 8, 44, C_GOLD, 1);
    LCD_Draw_Rect(117, 106, 1, 44, C_CREAM, 1);
}

static void draw_grass_texture(void) {
    static const uint8_t tufts[][2] = {
        { 14, 118}, { 72, 128}, {160, 118}, {210, 130},
        { 30, 150}, {170, 155}, { 60, 195}, {200, 195},
        { 14, 195}, {215, 118}, { 94, 136}, {148, 196},
    };
    for (size_t i = 0; i < sizeof(tufts) / sizeof(tufts[0]); i++) {
        LCD_Draw_Rect(tufts[i][0], tufts[i][1], 2, 2, C_DBROWN, 1);
        LCD_Draw_Rect(tufts[i][0] + 2, tufts[i][1] + 1, 1, 1, C_DBROWN, 1);
    }
}

static void menu_print_bold(const char* text, int x, int y, uint8_t colour, uint8_t size) {
    LCD_printString((char*)text, x,     y, colour, size);
    LCD_printString((char*)text, x + 1, y, colour, size);
}

static void draw_coin_icon(int cx, int cy) {
    LCD_Draw_Circle(cx, cy, 6, C_AMBER, 1);
    LCD_Draw_Circle(cx, cy, 6, C_DBROWN, 0);
    LCD_Draw_Circle(cx - 1, cy - 1, 2, C_GOLD, 1);
    LCD_Draw_Rect(cx, cy - 2, 1, 4, C_DBROWN, 1);
}

static void draw_building_label(const char* text, int x, int y, int w) {
    int text_w = 24;

    LCD_Draw_Rect(x + 2, y + 2, w, 13, C_DBROWN, 1);
    LCD_Draw_Rect(x,     y,     w, 13, C_AMBER, 1);
    LCD_Draw_Rect(x,     y,     w, 13, C_TBROWN, 0);
    LCD_Draw_Rect(x + 2, y + 2, w - 4, 1, C_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 11, w - 4, 1, C_TBROWN, 1);

    LCD_printString((char*)text, x + (w - text_w) / 2, y + 3, C_DBROWN, 1);
}

static void draw_building_labels(void) {
    /* Fixed building name plaques are intentionally disabled.
       Building names are shown only when the player walks near a building. */
}

static void draw_down_arrow(int x, int y, uint8_t colour) {
    LCD_Draw_Rect(x,     y,     9, 2, colour, 1);
    LCD_Draw_Rect(x + 2, y + 2, 5, 2, colour, 1);
    LCD_Draw_Rect(x + 4, y + 4, 1, 2, colour, 1);
}

static void draw_building_highlight(int8_t building_idx, uint32_t t_ms) {
    if (building_idx < 0 || building_idx > 2) return;

    const BuildingHitbox* b = &buildings[building_idx];
    int16_t x = b->cx - b->hw - 5;
    int16_t y = b->cy - b->hh - 5;
    int16_t w = (b->hw * 2) + 10;
    int16_t h = (b->hh * 2) + 10;

    uint8_t pulse = ((t_ms / 360) & 1);
    uint8_t frame_col = pulse ? C_GOLD : C_AMBER;

    LCD_Draw_Rect(x - pulse, y - pulse,
                  w + 2 * pulse, h + 2 * pulse,
                  frame_col, 0);
    LCD_Draw_Rect(x + 1 - pulse, y + 1 - pulse,
                  w + 2 * pulse - 2, h + 2 * pulse - 2,
                  frame_col, 0);
    LCD_Draw_Rect(x + 2 - pulse, y + 2 - pulse,
                  w + 2 * pulse - 4, h + 2 * pulse - 4,
                  C_GOLD, 0);

    int arrow_x = b->cx - 4;
    int arrow_y = (b->cy < 120) ? (b->cy - b->hh - 15) : (b->cy - b->hh - 17);
    draw_down_arrow(arrow_x, arrow_y, frame_col);

    const char* label_name = menu_options[building_idx];
    int label_w = 42;
    int label_x = b->cx - label_w / 2;
    int label_y;

    if (building_idx == 2) {
        label_y = b->cy + b->hh + 8;
    } else {
        label_y = b->cy + b->hh + 10;
    }

    draw_building_label(label_name, label_x, label_y, label_w);
}


static void render_home_menu(MenuSystem* menu, int8_t hovered, uint32_t t_ms) {
    LCD_Fill_Buffer(C_SAGE);

    LCD_Draw_Rect(0, 0, 240, 25, C_PURPLE, 1);
    LCD_Draw_Rect(0, 25, 240, 2, C_GOLD, 1);
    LCD_printString("COZY VILLAGE", 50, 8, C_GOLD, 2);

    draw_grass_texture();
    draw_paths();

    draw_farm();
    draw_lake(t_ms);
    draw_mine(t_ms);

    draw_building_highlight(hovered, t_ms);
    draw_villager(villager_x, villager_y, villager_anim);

    LCD_Draw_Rect(0, 216, 240, 24, C_TBROWN, 1);
    LCD_Draw_Rect(0, 216, 240, 1, C_GOLD, 1);
    
    if (hovered == 3) {
        uint8_t pulse = ((t_ms / 200) & 1);
        LCD_Draw_Rect(2 - pulse, 218 - pulse, 60 + 2*pulse, 20 + 2*pulse, C_CORAL, 0);
    }

    draw_coin_icon(12, 228);
    char gold_str[16];
    sprintf(gold_str, "%d", Get_Total_Gold());
    LCD_printString(gold_str, 24, 224, C_GOLD, 1);

    if (hovered == 3) {
        menu_print_bold("[JOY] View Prices", 74, 224, C_AMBER, 1);
    } else if (hovered >= 0) {
        menu_print_bold("[JOY] Enter", 92, 224, C_AMBER, 1);
    } else {
        menu_print_bold("Walk to a building", 82, 224, C_CREAM, 1);
    }

    (void)menu;
    LCD_Refresh(&cfg0);
}

// =============================================================================
// RENDER: Market Price List
// =============================================================================
static void render_price_list(uint32_t t_ms, uint8_t page) {
    // Parchment background
    LCD_Fill_Buffer(C_DBROWN);
    LCD_Draw_Rect(4, 4, 232, 232, C_CREAM, 1);
    LCD_Draw_Rect(4, 4, 232, 232, C_GOLD, 0);
    LCD_Draw_Rect(5, 5, 230, 230, C_GOLD, 0);

    // Title banner
    LCD_Draw_Rect(4, 4, 232, 26, C_PURPLE, 1);
    LCD_Draw_Rect(4, 30, 232, 2, C_GOLD, 1);

    int y = 40;
    int txt_x = 36;  // Left text alignment position
    int val_x = 150; // Right price alignment position

    if (page == 0) {
        // Page 1: income from farm and lake
        LCD_printString("INCOME 1/4", 50, 8, C_GOLD, 2);
        
        // Farm Crop
        LCD_Draw_Rect(12, y+10, 14, 6, C_DBROWN, 1); LCD_Draw_Rect(14, y+4, 10, 6, C_SAGE, 1); 
        LCD_printString("FARM", txt_x, y, C_DBROWN, 2);
        LCD_printString("CROP", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("+10 G", val_x, y+8, C_SAGE, 2);
        y += 44;

        // Blue Fish
        LCD_Draw_Rect(10, y+8, 20, 10, C_SKY, 1); LCD_Draw_Rect(14, y+10, 4, 4, C_BLACK, 1); 
        LCD_printString("BLUE", txt_x, y, C_DBROWN, 2);
        LCD_printString("FISH", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("+30 G", val_x, y+8, C_SAGE, 2);
        y += 44;

        // Red Snapper
        LCD_Draw_Rect(10, y+8, 20, 10, C_SOFT_RED, 1); LCD_Draw_Rect(14, y+10, 4, 4, C_BLACK, 1); 
        LCD_printString("RED", txt_x, y, C_DBROWN, 2);
        LCD_printString("SNAPPER", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("+60 G", val_x, y+8, C_SAGE, 2);
        y += 44;

        // Gold Puffer
        LCD_Draw_Rect(10, y+8, 20, 10, C_AMBER, 1); LCD_Draw_Rect(14, y+10, 4, 4, C_BLACK, 1); 
        LCD_printString("GOLD", txt_x, y, C_DBROWN, 2);
        LCD_printString("PUFFER", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("+150G", val_x, y+8, C_SAGE, 2);

    } else if (page == 1) {
        // Page 2: income from mine
        LCD_printString("INCOME 2/4", 50, 8, C_GOLD, 2);

        // Copper Ore
        LCD_Draw_Rect(14, y+6, 14, 14, C_ORANGE, 1); LCD_Draw_Rect(18, y+10, 4, 4, C_CREAM, 1);
        LCD_printString("COPPER", txt_x, y, C_DBROWN, 2);
        LCD_printString("ORE", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("+10 G", val_x, y+8, C_SAGE, 2);
        y += 44;

        // Gold Ore
        LCD_Draw_Rect(14, y+6, 14, 14, C_GOLD, 1); LCD_Draw_Rect(18, y+10, 4, 4, C_CREAM, 1);
        LCD_printString("GOLD", txt_x, y, C_DBROWN, 2);
        LCD_printString("ORE", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("+30 G", val_x, y+8, C_SAGE, 2);
        y += 44;

        // Diamond text is centred vertically because it fits on one line.
        LCD_Draw_Rect(14, y+6, 14, 14, C_SKY, 1); LCD_Draw_Rect(18, y+10, 4, 4, C_CREAM, 1);
        LCD_printString("DIAMOND", txt_x, y+8, C_DBROWN, 2);
        LCD_printString("+80 G", val_x, y+8, C_SAGE, 2);

    } else if (page == 2) {
        // Page 3: expenses for farm and lake
        LCD_printString("EXPENSE 3/4", 42, 8, C_GOLD, 2);

        // Farm Seed
        LCD_Draw_Rect(16, y+8, 10, 12, C_TBROWN, 1); LCD_Draw_Rect(18, y+10, 4, 4, C_CREAM, 1);
        LCD_printString("FARM", txt_x, y, C_DBROWN, 2);
        LCD_printString("SEED", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("-2 G", val_x, y+8, C_SOFT_RED, 2);
        y += 44;

        // Fish Bait
        LCD_Draw_Rect(14, y+8, 14, 10, C_SOFT_RED, 1); LCD_Draw_Rect(24, y+8, 4, 14, C_CREAM, 1);
        LCD_printString("FISH", txt_x, y, C_DBROWN, 2);
        LCD_printString("BAIT", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("-20 G", val_x, y+8, C_SOFT_RED, 2);
        y += 44;

        // Coffee
        LCD_Draw_Rect(14, y+6, 14, 14, C_SKY, 1); LCD_Draw_Rect(28, y+10, 4, 8, C_SKY, 0);
        LCD_printString("HOT", txt_x, y, C_DBROWN, 2);
        LCD_printString("COFFEE", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("-40 G", val_x, y+8, C_SOFT_RED, 2);
        y += 44;

        // Pro Rod
        LCD_Draw_Line(12, y+20, 28, y+4, C_DBROWN); LCD_Draw_Line(12, y+19, 27, y+4, C_DBROWN);
        LCD_Draw_Rect(16, y+12, 6, 6, C_GOLD, 1);
        LCD_printString("PRO", txt_x, y, C_DBROWN, 2);
        LCD_printString("ROD", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("-500G", val_x, y+8, C_SOFT_RED, 2);

    } else if (page == 3) {
        // Page 4: expenses for mine
        LCD_printString("EXPENSE 4/4", 42, 8, C_GOLD, 2);

        // Torch
        LCD_Draw_Rect(18, y+10, 4, 10, C_DBROWN, 1); LCD_Draw_Rect(16, y+4, 8, 6, C_AMBER, 1);
        LCD_printString("MINE", txt_x, y, C_DBROWN, 2);
        LCD_printString("TORCH", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("-20 G", val_x, y+8, C_SOFT_RED, 2);
        y += 44;

        // Dynamite text is centred vertically because it fits on one line.
        LCD_Draw_Rect(16, y+8, 10, 12, C_SOFT_RED, 1); LCD_Draw_Rect(20, y+4, 2, 4, C_CREAM, 1);
        LCD_printString("DYNAMITE", txt_x, y+8, C_DBROWN, 2);
        LCD_printString("-30 G", val_x, y+8, C_SOFT_RED, 2);
        y += 44;

        // Potion
        LCD_Draw_Rect(18, y+4, 4, 6, C_DBROWN, 1); LCD_Draw_Rect(14, y+10, 12, 10, C_PURPLE, 1);
        LCD_printString("HEALTH", txt_x, y, C_DBROWN, 2);
        LCD_printString("POTION", txt_x, y+16, C_DBROWN, 2);
        LCD_printString("-50 G", val_x, y+8, C_SOFT_RED, 2);
    }

    // Blinking instructions at bottom
    uint8_t pulse = ((t_ms / 300) & 1);
    LCD_printString("[L/R] Flip", 12, 215, C_DBROWN, 1);
    LCD_printString("[JOY] Close", 130, 215, pulse ? C_CORAL : C_DBROWN, 1);

    LCD_Refresh(&cfg0);
}

// =============================================================================
// Public API
// =============================================================================

void Menu_Init(MenuSystem* menu) {
    menu->selected_option = 0;
    villager_x = 120;
    villager_y = 130;
    villager_anim = 0;
}

MenuState Menu_Run(MenuSystem* menu) {
    LCD_Set_Palette(PALETTE_CUSTOM);

    MenuState selected_game = MENU_STATE_HOME;
    uint8_t showing_prices = 0;
    uint8_t price_page = 0; 
    uint32_t last_flip_tick = 0;

    while (1) {
        uint32_t frame_start = HAL_GetTick();

        Input_Read();
        Joystick_Read(&joystick_cfg, &joystick_data);
        
        // Price list screen input handling
        if (showing_prices) {
            if (current_input.joy_pressed) {
                showing_prices = 0;
                HAL_Delay(150); // Debounce delay
            } else {
                Direction dir = joystick_data.direction;
                if ((dir == W) && (frame_start - last_flip_tick > 250)) {
                    price_page = (price_page + 3) % 4; // Previous page with wrap-around
                    last_flip_tick = frame_start;
                } else if ((dir == E) && (frame_start - last_flip_tick > 250)) {
                    price_page = (price_page + 1) % 4; // Next page
                    last_flip_tick = frame_start;
                }
                render_price_list(frame_start, price_page);
            }
            
            // Frame pacing for UI
            uint32_t frame_time = HAL_GetTick() - frame_start;
            if (frame_time < MENU_FRAME_TIME_MS) HAL_Delay(MENU_FRAME_TIME_MS - frame_time);
            continue; 
        }

        // Standard village movement and selection logic
        Direction dir = joystick_data.direction;
        const int speed = 2;
        uint8_t moved = 0;
        if (dir == N || dir == NW || dir == NE) { villager_y -= speed; moved = 1; }
        if (dir == S || dir == SW || dir == SE) { villager_y += speed; moved = 1; }
        if (dir == W || dir == NW || dir == SW) { villager_x -= speed; moved = 1; }
        if (dir == E || dir == NE || dir == SE) { villager_x += speed; moved = 1; }

        if (villager_x < WALK_X_MIN) villager_x = WALK_X_MIN;
        if (villager_x > WALK_X_MAX) villager_x = WALK_X_MAX;
        if (villager_y < WALK_Y_MIN) villager_y = WALK_Y_MIN;
        if (villager_y > WALK_Y_MAX) villager_y = WALK_Y_MAX;

        if (moved && (frame_start - villager_last_anim_tick > 200)) {
            villager_anim ^= 1;
            villager_last_anim_tick = frame_start;
        }

        int8_t hovered = villager_on_building(villager_x, villager_y);

        if (current_input.joy_pressed) {
            if (hovered == 3) {
                // Open the market price list.
                showing_prices = 1;
                price_page = 0; // Always start from page 1
                HAL_Delay(150);
                continue;
            } 
            else if (hovered >= 0) {
                buzzer_tone(&buzzer_cfg, 880, 12);
                HAL_Delay(70);
                buzzer_off(&buzzer_cfg);

                // Enter the selected game.
                if (hovered == 0) selected_game = MENU_STATE_GAME_1;
                else if (hovered == 1) selected_game = MENU_STATE_GAME_2;
                else if (hovered == 2) selected_game = MENU_STATE_GAME_3;
                menu->selected_option = (uint8_t)hovered; 
                break;
            }
        }

        render_home_menu(menu, hovered, frame_start);

        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < MENU_FRAME_TIME_MS) {
            HAL_Delay(MENU_FRAME_TIME_MS - frame_time);
        }
    }

    return selected_game;
}