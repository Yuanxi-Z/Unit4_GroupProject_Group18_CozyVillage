#include "fish_render.h"
#include "Economy.h"
#include "LCD.h"
#include <stdio.h>
#include <math.h>

// Color palette indices
#define CLR_BLACK         0
#define CLR_CREAM         1
#define CLR_SOFT_RED      2
#define CLR_LEAF_GREEN    3
#define CLR_DENIM_BLUE    4
#define CLR_WARM_ORANGE   5
#define CLR_AMBER         6
#define CLR_SOFT_PINK     7
#define CLR_DARK_BROWN    9
#define CLR_HONEY_GOLD    10
#define CLR_TRUE_BROWN    12
#define CLR_STONE_GREY    13
#define CLR_SKY_CYAN      14

// --- Player Sprites ---

// Macro shortcuts for drawing the sprite frames
#define __ 255
#define HY 6   // Hat
#define HB 5   // Brim
#define HR 12  // Hair
#define SK 5   // Skin
#define IK 9   // Outline
#define BU 7   // Blush
#define MT 2   // Mouth
#define OV 2   // Overalls
#define UN 1   // Shirt
#define OH 6   // Highlight
#define DN 4   // Pants
#define BT 9   // Boots
#define WH 13  // Metal
#define WM 12  // Wood

// Idle frame
const uint8_t FISH_SPR_GIRL_IDLE[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,__,__,__,
    __,__,__,HY,HY,HY,HY,HY,HY,HY,HY,__,__,__, 
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

// Walk animation frame
const uint8_t FISH_SPR_GIRL_WALK_A[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,__,__,__,
    __,__,__,HY,HY,HY,HY,HY,HY,HY,HY,__,__,__, 
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

// Casting frame (rod extended)
const uint8_t FISH_SPR_GIRL_CAST[14 * 14] = {
    __,__,__,__,HY,HY,HY,HY,HY,HY,__,__,WM,__, 
    __,__,__,HY,HY,HY,HY,HY,HY,HY,HY,WM,__,__, 
    __,__,HB,HB,HB,HB,HB,HB,HB,HB,HB,HB,WM,__, 
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

// Clean up macros
#undef __
#undef HY
#undef HB
#undef HR
#undef SK
#undef IK
#undef BU
#undef MT
#undef OV
#undef UN
#undef OH
#undef DN
#undef BT
#undef WH
#undef WM

// --- Internal Rendering Functions ---

static void draw_coin_icon(int cx, int cy) {
    LCD_Draw_Circle(cx, cy, 6, CLR_AMBER, 1);
    LCD_Draw_Circle(cx, cy, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Circle(cx - 1, cy - 1, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(cx, cy - 2, 1, 4, CLR_DARK_BROWN, 1);
}

// Draws the total gold counter in the top right
static void draw_gold_hud(void) {
    LCD_Draw_Rect(156, 2, 82, 16, CLR_BLACK, 1); 
    LCD_Draw_Rect(156, 2, 82, 16, CLR_TRUE_BROWN, 0); 
    draw_coin_icon(166, 10); 
    char gold_hud[16];
    sprintf(gold_hud, "%d G", Get_Total_Gold());
    LCD_printString(gold_hud, 178, 6, CLR_HONEY_GOLD, 1); 
}

// Hardcoded shop building sprite
static void draw_small_shop_building(int x, int y, uint32_t tick) {
    LCD_Draw_Rect(x + 2, y + 10, 36, 24, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 10, 36, 24, CLR_DARK_BROWN, 0); 
    LCD_Draw_Rect(x + 8, y + 15, 24, 12, CLR_BLACK, 1);
    LCD_Draw_Rect(x, y, 40, 10, CLR_SOFT_RED, 1);
    for(int i = 0; i < 40; i += 8) {
        LCD_Draw_Rect(x + i, y, 4, 10, CLR_CREAM, 1); 
    }
    LCD_Draw_Rect(x, y, 40, 10, CLR_DARK_BROWN, 0); 
    LCD_Draw_Rect(x + 4, y + 27, 32, 4, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 4, y + 27, 32, 4, CLR_DARK_BROWN, 0);
    LCD_Draw_Rect(x + 6, y - 6, 28, 10, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 6, y - 6, 28, 10, CLR_DARK_BROWN, 0);
    LCD_printString("SHOP", x + 8, y - 4, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 10, y + 23, 4, 4, CLR_DENIM_BLUE, 1);
    
    // Animate shop sign indicator
    if ((tick / 400) % 2) {
        LCD_Set_Pixel(x + 11, y + 21, CLR_STONE_GREY); 
    }
}

// Parallax wave effect for the water
static void Draw_Dynamic_Waves(uint32_t tick) {
    int offset1 = (tick / 50) % 240;
    LCD_Draw_Line(offset1, 100, offset1 + 30, 100, CLR_SKY_CYAN);
    LCD_Draw_Line(240 - ((tick / 70) % 240), 150, 280 - ((tick / 70) % 240), 150, CLR_SKY_CYAN);
}

// --- Public API ---

void Fish_Render_Environment(uint32_t tick, int bait_count) {
    LCD_Fill_Buffer(CLR_DENIM_BLUE); 
    
    // Draw wavy shoreline using a sine function
    for(int x = 0; x < 240; x++) {
        uint16_t shore_y = 55 + (int)(sin(x / 20.0) * 12.0); 
        LCD_Draw_Line(x, 0, x, shore_y, CLR_LEAF_GREEN); 
        LCD_Set_Pixel(x, shore_y, CLR_BLACK); 
    }
    
    // Draw dock and shop
    LCD_Draw_Rect(0, 50, 60, 20, CLR_TRUE_BROWN, 1); 
    LCD_Draw_Rect(0, 50, 60, 2, CLR_HONEY_GOLD, 1);  
    draw_small_shop_building(8, 16, tick);
    
    Draw_Dynamic_Waves(tick); 
    draw_gold_hud(); 
    
    // Draw bait inventory icon
    if(bait_count > 0) {
        LCD_Draw_Rect(2, 2, 8, 8, CLR_SOFT_RED, 1);
        char b_str[16]; 
        sprintf(b_str, "x%d", bait_count);
        LCD_printString(b_str, 12, 3, CLR_CREAM, 1);
    }
}

void Fish_Render_Thick_Rod(int x1, int y1, int x2, int y2, int has_pro_rod) {
    uint16_t rod_color = has_pro_rod ? CLR_AMBER : CLR_TRUE_BROWN;
    uint16_t rod_shadow = has_pro_rod ? CLR_WARM_ORANGE : CLR_DARK_BROWN;
    
    LCD_Draw_Line(x1, y1 - 1, x2, y2 - 1, CLR_HONEY_GOLD); 
    LCD_Draw_Line(x1, y1,     x2, y2,     rod_color); 
    LCD_Draw_Line(x1, y1 + 1, x2, y2 + 1, rod_shadow); 

    // Draw the reel
    int reel_x = x1 + (x2 - x1) / 5;
    int reel_y = y1 + (y2 - y1) / 5;
    LCD_Draw_Rect(reel_x - 3, reel_y + 2, 6, 6, CLR_STONE_GREY, 1); 
    LCD_Draw_Rect(reel_x - 1, reel_y + 4, 2, 2, CLR_CREAM, 1);      
    LCD_Draw_Line(reel_x, reel_y + 6, reel_x + 3, reel_y + 10, CLR_STONE_GREY); 
}

void Fish_Render_Progress_Bar(int x, int y, int width, int height, int value, int max_val, uint8_t fill_color, const char* label) {
    LCD_printString((char*)label, x, y - 12, CLR_CREAM, 1); 
    LCD_Draw_Rect(x, y, width, height, CLR_CREAM, 0); 
    
    int fill_w = (value * (width - 2)) / max_val;
    if(fill_w < 0) fill_w = 0;
    if(fill_w > width - 2) fill_w = width - 2;
    
    if (fill_w > 0) {
        LCD_Draw_Rect(x + 1, y + 1, fill_w, height - 2, fill_color, 1); 
    }
}

// Renders the vertical tension minigame bar
void Fish_Render_Fishing_Bar(int x0, int y0, int h, int tension) {
    int w = 18; 
    
    LCD_Draw_Rect(x0 - 2, y0 - 2, w + 4, h + 4, CLR_CREAM, 0); 
    LCD_Draw_Rect(x0, y0, w, h, CLR_BLACK, 1);         
    
    // Draw tick marks
    for(int i = 10; i < 100; i += 20) {
        int tick_y = y0 + h - (i * h / 100);
        LCD_Draw_Line(x0 - 4, tick_y, x0 - 1, tick_y, CLR_CREAM);
    }
    
    // Draw the green safe zone (30% to 70% bounds)
    int safe_h = h * 60 / 100; 
    int safe_y = y0 + h - (80 * h / 100); 
    LCD_Draw_Rect(x0 + 2, safe_y, w - 4, safe_h, CLR_LEAF_GREEN, 1);
    
    // Draw the moving tension cursor
    int cursor_h = 8;
    int tension_y_mapped = (y0 + h - cursor_h) - (tension * (h - cursor_h) / 100);
    LCD_Draw_Rect(x0 + 1, tension_y_mapped, w - 2, cursor_h, CLR_CREAM, 1);
    LCD_Draw_Line(x0 + 1, tension_y_mapped + cursor_h / 2, x0 + w - 2, tension_y_mapped + cursor_h / 2, CLR_BLACK); 
}

// Draws the caught fish sprite using string arrays as pixel maps
void Fish_Render_Big_Fish(int x, int y, int type, uint32_t tick) {
    int float_y = (tick / 150) % 4; // Floating animation offset
    y += float_y;
    int scale = 3; 

    const char** pattern;
    int rows;

    static const char* pat_blue[] = {
        "          DDD       ",
        "    DDDDDDCCCDD    D",
        "  DDCCCCCCCCCCCD  DD",
        " DCCKWCCCCCCCCCCDDD ",
        " DCCCCCCCCCCCCCCDDD ",
        " DWWDDWWWWWCCCDCDDD ",
        "  DWWWWWWWWWWWD    D",
        "    DDDDDDDDDD      "
    };
    
    static const char* pat_red[] = {
        "       A A A A      ",
        "     AAR R R R A   A",
        "   AARRRRRRRRRRRA AA",
        "  ARRRKWWRRRRRRRRA A",
        " ARRRRRWWRRRRRRRRRAA",
        " ARRRRRRRRWWWWWWWRAA",
        "  AWWAAWWWWWWWWWA  A",
        "   AA  AWWWWWWWAA   ",
        "         AAAA       "
    };

    static const char* pat_gold[] = {
        "       B B B        ",
        "     BGGGGGGB       ",
        "   BBGGWGGWGGGBB    ",
        "  B GGWKWGWKWGG B O ",
        " B  GGWWWGWWWGG  OOO",
        " B  GGGGGRGGGGG  OOO",
        "  B GPPGGGGGPPG B O ",
        "   BBGWWWWWWWGBB    ",
        "     BWWWWWWWB      ",
        "       B B B        "
    };

    if (type == 0) { pattern = pat_blue; rows = 8; }
    else if (type == 1) { pattern = pat_red; rows = 9; }
    else { pattern = pat_gold; rows = 10; }

    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < 20; c++) {
            char ch = pattern[r][c];
            if (ch == ' ') continue; 
            
            uint16_t col = CLR_BLACK;
            switch(ch) {
                case 'D': col = CLR_DENIM_BLUE; break;
                case 'C': col = CLR_SKY_CYAN; break;
                case 'W': col = CLR_CREAM; break;
                case 'K': col = CLR_BLACK; break;
                case 'R': col = CLR_SOFT_RED; break;
                case 'A': col = CLR_AMBER; break;
                case 'G': col = CLR_HONEY_GOLD; break;
                case 'B': col = CLR_DARK_BROWN; break;
                case 'O': col = CLR_WARM_ORANGE; break;
                case 'P': col = CLR_SOFT_PINK; break;
            }
            LCD_Draw_Rect(x + c * scale, y + r * scale, scale, scale, col, 1);
        }
    }
}