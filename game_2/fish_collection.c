#include "fish_collection.h"
#include "fish_render.h"
#include "LCD.h"
#include <stdio.h>

// Shared color palette indices
#define CLR_BLACK         0
#define CLR_CREAM         1
#define CLR_SOFT_RED      2
#define CLR_DENIM_BLUE    4
#define CLR_WARM_ORANGE   5
#define CLR_AMBER         6
#define CLR_DARK_BROWN    9
#define CLR_HONEY_GOLD    10
#define CLR_SAGE          11
#define CLR_TRUE_BROWN    12
#define CLR_STONE_GREY    13
#define CLR_SKY_CYAN      14
#define CLR_CORAL         15

// Global array tracking the number of each fish caught
int fish_caught_count[3] = {0, 0, 0};

static const char* fish_names[3] = {
    "BLUE SARDINE",
    "RED SNAPPER",
    "GOLD PUFFER"
};

// Increments the catch counter for a specific fish type
void Fish_Collection_Add(int fish_type) {
    if (fish_type >= 0 && fish_type < 3) {
        fish_caught_count[fish_type]++;
    }
}

// --- UI Rendering ---

// Draws an individual fish collection card
static void draw_fish_card(int x, int y, int index, uint32_t tick) {
    // Card background and drop shadow
    LCD_Draw_Rect(x + 2, y + 2, 66, 86, CLR_DARK_BROWN, 1); 
    LCD_Draw_Rect(x, y, 66, 86, CLR_CREAM, 1);              
    
    // Dark borders
    LCD_Draw_Rect(x, y, 66, 2, CLR_DARK_BROWN, 1);          
    LCD_Draw_Rect(x, y + 84, 66, 2, CLR_DARK_BROWN, 1);     
    LCD_Draw_Rect(x, y, 2, 86, CLR_DARK_BROWN, 1);          
    LCD_Draw_Rect(x + 64, y, 2, 86, CLR_DARK_BROWN, 1);     

    // Title bar
    LCD_Draw_Rect(x + 2, y + 2, 62, 12, CLR_DARK_BROWN, 1);
    
    if (fish_caught_count[index] > 0) {
        // Unlocked state: Show fish name, sprite, and catch count
        LCD_printString((char*)fish_names[index], x + 4, y + 4, CLR_CREAM, 1);
        
        // Add a slight bobbing animation to the fish
        int float_y = (tick / 200) % 3;
        Fish_Render_Big_Fish(x + 3, y + 18 + float_y, index, tick);
        
        // Bottom stats bar
        LCD_Draw_Rect(x + 2, y + 70, 62, 14, CLR_HONEY_GOLD, 1);
        LCD_Draw_Rect(x + 2, y + 70, 62, 1, CLR_CREAM, 1); 
        
        char count_str[10];
        sprintf(count_str, "C: %d", fish_caught_count[index]);
        LCD_printString(count_str, x + 15, y + 73, CLR_DARK_BROWN, 1);
    } else {
        // Locked state: Greyed out with question marks
        LCD_printString("???", x + 25, y + 4, CLR_STONE_GREY, 1);
        LCD_printString("?", x + 26, y + 35, CLR_STONE_GREY, 2);
        
        LCD_Draw_Rect(x + 2, y + 70, 62, 14, CLR_STONE_GREY, 1);
        LCD_printString("LOCKED", x + 15, y + 73, CLR_CREAM, 1);
    }
}

// Renders the main collection book interface
void Fish_Collection_Draw_UI(uint32_t tick) {
    // Dot pattern background
    LCD_Fill_Buffer(CLR_CREAM);
    for (int y = 40; y < 240; y += 20) {
        for (int x = 10; x < 240; x += 30) {
            LCD_Set_Pixel(x, y, CLR_HONEY_GOLD);
        }
    }

    // Top banner
    LCD_Draw_Rect(0, 0, 240, 26, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, 0, 240, 3,  CLR_DARK_BROWN, 1);   
    LCD_Draw_Rect(0, 23, 240, 3, CLR_DARK_BROWN, 1);   
    LCD_printString("COLLECTION BOOK", 50, 7, CLR_CREAM, 2);

    // Draw the three fish cards side-by-side
    draw_fish_card(10,  40, 0, tick); 
    draw_fish_card(87,  40, 1, tick); 
    draw_fish_card(164, 40, 2, tick); 

    // Achievement summary box
    LCD_Draw_Rect(10, 140, 220, 40, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(10, 140, 220, 2,  CLR_DARK_BROWN, 1);
    
    int total_types = 0;
    int total_caught = 0;
    for(int i = 0; i < 3; i++) {
        if(fish_caught_count[i] > 0) total_types++;
        total_caught += fish_caught_count[i];
    }
    
    char summary[40];
    sprintf(summary, "Types Discovered: %d/3", total_types);
    LCD_printString(summary, 20, 148, CLR_HONEY_GOLD, 1);
    
    sprintf(summary, "Total Fish Caught: %d", total_caught);
    LCD_printString(summary, 20, 162, CLR_CREAM, 1);

    // Footer navigation
    LCD_Draw_Rect(0, 200, 240, 40, CLR_SAGE, 1);
    LCD_Draw_Rect(0, 200, 240, 2,  CLR_DARK_BROWN, 1);
    LCD_printString("[BT3] Back to Fishing", 40, 215, CLR_DARK_BROWN, 1);
}