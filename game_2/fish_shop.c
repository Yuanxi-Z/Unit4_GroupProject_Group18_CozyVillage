#include "fish_shop.h"
#include "Economy.h"
#include "LCD.h"
#include "fish_hardware.h" 
#include <stdio.h>

#define LCD_Draw_Point LCD_Set_Pixel

// Global inventory state shared with the main game module
extern int inventory_bait;
extern int inventory_coffee;
extern int has_pro_rod;

// Color palette indices
#define CLR_BLACK         0
#define CLR_CREAM         1
#define CLR_SOFT_RED      2
#define CLR_LEAF_GREEN    3
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

// Shopkeeper sprite macro shortcuts
#define __ 255
#define HR CLR_TRUE_BROWN
#define SK CLR_WARM_ORANGE
#define IK CLR_DARK_BROWN
#define MT CLR_SOFT_RED
#define AP CLR_LEAF_GREEN
#define CR CLR_CREAM

static const uint8_t SPR_SHOPKEEPER_IDLE[14 * 14] = {
    __,__,__,HR,HR,HR,HR,HR,HR,HR,HR,__,__,__,
    __,__,HR,HR,HR,HR,HR,HR,HR,HR,HR,HR,__,__,
    __,HR,HR,SK,SK,SK,SK,SK,SK,SK,SK,HR,HR,__,
    __,HR,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,HR,__, 
    __,HR,SK,SK,IK,IK,SK,SK,IK,IK,SK,SK,HR,__,
    __,__,SK,SK,SK,IK,SK,SK,IK,SK,SK,SK,__,__, 
    __,__,SK,SK,SK,SK,SK,SK,SK,SK,SK,SK,__,__, 
    __,__,SK,SK,SK,MT,MT,MT,MT,SK,SK,SK,__,__,
    __,__,__,SK,SK,SK,SK,SK,SK,SK,SK,__,__,__, 
    __,__,__,__,SK,SK,SK,SK,SK,SK,__,__,__,__,
    __,__,__,AP,AP,CR,CR,CR,CR,AP,AP,__,__,__,
    __,__,AP,AP,AP,AP,AP,AP,AP,AP,AP,AP,__,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
    __,AP,AP,AP,AP,AP,CR,CR,AP,AP,AP,AP,AP,__,
};

#undef __
#undef HR
#undef SK
#undef IK
#undef MT
#undef AP
#undef CR

// Shop inventory configuration
typedef struct {
    char name[10];
    uint16_t price;
} ShopItem;

static ShopItem shop_inventory[3] = {
    {"BAIT", 20},
    {"COFFEE", 40},
    {"PRO ROD", 500}
};

static int8_t shop_selected_index = 0; 

void Fish_Shop_Init(void) {
    shop_selected_index = 0;
}

// --- Rendering Helpers ---

static void draw_coin_icon(int cx, int cy) {
    LCD_Draw_Circle(cx, cy, 6, CLR_AMBER, 1);
    LCD_Draw_Circle(cx, cy, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Circle(cx - 1, cy - 1, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(cx, cy - 2, 1, 4, CLR_DARK_BROWN, 1);
}

static void draw_coin_jar(int x, int y) {
    LCD_Draw_Rect(x,     y + 4, 18, 20, CLR_SKY_CYAN, 1);
    LCD_Draw_Rect(x - 1, y + 2, 20, 2,  CLR_DENIM_BLUE, 1);
    LCD_Draw_Rect(x + 1, y,     16, 2,  CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 6, 2, 10,  CLR_CREAM, 1);
    LCD_Draw_Circle(x + 5,  y + 17, 2, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 10, y + 17, 2, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 14, y + 17, 2, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 7,  y + 13, 2, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 12, y + 13, 2, CLR_AMBER, 1);
    LCD_Draw_Rect(x + 1, y + 21, 16, 2, CLR_DENIM_BLUE, 1);
}

static void draw_real_coffee_icon(int x, int y, uint32_t tick) {
    int cx = x + 6; int cy = y + 8;
    LCD_Draw_Rect(cx, cy, 14, 14, CLR_DENIM_BLUE, 1);
    LCD_Draw_Rect(cx + 14, cy + 3, 5, 8, CLR_DENIM_BLUE, 0); 
    LCD_Draw_Rect(cx + 14, cy + 4, 4, 6, CLR_DENIM_BLUE, 0); 
    LCD_Draw_Rect(cx, cy, 14, 2, CLR_DARK_BROWN, 1);
    
    // Animate steam based on system tick
    if ((tick / 300) % 2) {
        LCD_Draw_Line(cx + 3, cy - 3, cx + 3, cy - 7, CLR_STONE_GREY);
        LCD_Draw_Line(cx + 9, cy - 2, cx + 9, cy - 8, CLR_STONE_GREY);
    }
}

static void draw_real_bait_icon(int x, int y, uint32_t tick) {
    int cx = x + 4; int cy = y + 10;
    LCD_Draw_Rect(cx, cy, 16, 8, CLR_SOFT_RED, 1);
    LCD_Set_Pixel(x + 11, y + 21, CLR_STONE_GREY); 
    LCD_Draw_Line(cx + 8, cy + 8, cx + 8, cy + 16, CLR_STONE_GREY);
    LCD_Draw_Line(cx + 8, cy + 16, cx + 16, cy + 16, CLR_STONE_GREY);
    LCD_Draw_Point(cx + 16, cy + 14, CLR_STONE_GREY);
}

static void draw_real_pro_rod_icon(int x, int y, uint32_t tick) {
    int cx = x + 4; int cy = y + 4;
    LCD_Draw_Line(cx, cy + 20, cx + 20, cy, CLR_TRUE_BROWN);
    LCD_Draw_Line(cx+1, cy + 20, cx + 21, cy, CLR_TRUE_BROWN);
    LCD_Draw_Rect(cx + 4, cy + 14, 6, 6, CLR_STONE_GREY, 0);
    LCD_Draw_Rect(cx + 6, cy + 16, 2, 2, CLR_CREAM, 1);
}

static void draw_product_card(int x, int y, const char* name, int item_index, uint32_t tick, const char* price) {
    // Draw card background and borders
    LCD_Draw_Rect(x + 2, y + 2, 56, 66, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y, 56, 66, CLR_CREAM, 1);
    LCD_Draw_Rect(x, y,       56, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y + 64,  56, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y,        2, 66, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 54, y,   2, 66, CLR_DARK_BROWN, 1);

    LCD_Draw_Rect(x + 2, y + 2, 52, 10, CLR_DARK_BROWN, 1);
    
    // Center the item name text
    int name_len = 0;
    while (name[name_len] && name_len < 10) name_len++;
    int name_w = name_len * 6 - 1;
    int nx = x + (56 - name_w) / 2;
    LCD_printString(name, nx, y + 4, CLR_CREAM, 1);

    // Draw corresponding item icon
    int icon_x = x + 14;
    int icon_y = y + 18;
    switch (item_index) {
        case 0: draw_real_bait_icon(icon_x, icon_y, tick); break;
        case 1: draw_real_coffee_icon(icon_x, icon_y, tick); break;
        case 2: draw_real_pro_rod_icon(icon_x, icon_y, tick); break;
    }

    LCD_Draw_Rect(x + 2, y + 52, 52, 12, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 52, 52, 1, CLR_CREAM, 1);
    
    // Center the price text
    int price_len = 0;
    while (price[price_len] && price_len < 6) price_len++;
    int price_w = price_len * 6 - 1;
    int px = x + (56 - price_w) / 2;
    LCD_printString(price, px, y + 55, CLR_DARK_BROWN, 1);
}

// --- Main UI Routine ---

void Fish_Shop_Draw_UI(uint32_t tick) {
    LCD_Fill_Buffer(CLR_CREAM);
    
    // Background wallpaper pattern
    for (int y = 34; y < 102; y += 12) {
        for (int x = 8; x < 240; x += 18) {
            LCD_Draw_Rect(x, y, 2, 2, CLR_HONEY_GOLD, 1);
        }
    }

    // Top banner
    LCD_Draw_Rect(0, 0, 240, 26, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, 0, 240, 3,  CLR_DARK_BROWN, 1);   
    LCD_Draw_Rect(0, 23, 240, 3, CLR_DARK_BROWN, 1);   
    for (int x = 20; x < 240; x += 40) {
        LCD_Draw_Rect(x, 3, 1, 20, CLR_DARK_BROWN, 1);
    }
    LCD_Draw_Rect(8,   10, 3, 3, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(229, 10, 3, 3, CLR_HONEY_GOLD, 1);
    LCD_printString("WILLY'S SHOP", 55, 7, CLR_CREAM, 2);

    // Shelf graphic
    LCD_Draw_Rect(6, 104, 228, 5, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(6, 108, 228, 2, CLR_DARK_BROWN, 1);

    // Render items on the shelf
    static const int card_x[3] = {10, 92, 174};
    for (int i = 0; i < 3; i++) {
        char price_str[10];
        sprintf(price_str, "%dG", shop_inventory[i].price);
        draw_product_card(card_x[i], 34, shop_inventory[i].name, i, tick, price_str);
    }

    // Draw selection highlight with pulsating effect
    {
        uint8_t pulse = ((tick / 300) & 1);
        int sx = card_x[shop_selected_index];
        int sy = 34;
        LCD_Draw_Rect(sx - 3 - pulse, sy - 3 - pulse, 62 + 2 * pulse, 72 + 2 * pulse, CLR_CORAL, 0);
        LCD_Draw_Rect(sx - 2 - pulse, sy - 2 - pulse, 60 + 2 * pulse, 70 + 2 * pulse, CLR_AMBER, 0);
    }

    // Counter desk
    LCD_Draw_Rect(0, 125, 240, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 128, 240, 29, CLR_TRUE_BROWN, 1); 
    for (int x = 60; x < 240; x += 60) {
        LCD_Draw_Rect(x, 128, 1, 29, CLR_DARK_BROWN, 1);
    }
    LCD_Draw_Rect(8,   135, 48, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(130, 132, 54, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(195, 142, 40, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 155, 240, 2, CLR_DARK_BROWN, 1);

    draw_coin_jar(18, 126);
    LCD_Draw_Sprite_Scaled(99, 85, 14, 14, (uint8_t*)SPR_SHOPKEEPER_IDLE, 3);

    // Bottom floor and HUD area
    LCD_Draw_Rect(0, 157, 240, 3,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 160, 240, 80, CLR_SAGE, 1);
    LCD_Draw_Rect(0, 160, 240, 1,  CLR_CREAM, 1);

    draw_coin_icon(14, 178);
    char buf[32];
    snprintf(buf, sizeof(buf), "COINS: %d", Get_Total_Gold());
    LCD_printString(buf, 28, 172, CLR_CREAM, 2);

    LCD_Draw_Rect(10, 193, 220, 1, CLR_DARK_BROWN, 1);
    LCD_printString("[L/R] Select  [BT2] Buy", 8, 204, CLR_CREAM, 1);
    LCD_printString("[BT3] Leave", 8, 220, CLR_HONEY_GOLD, 1);
}

// --- Input Handling ---

bool Fish_Shop_Handle_Input(uint8_t bt2, uint8_t bt3, Direction dir, uint32_t tick) {
    static uint32_t last_nav = 0;
    
    // Joystick navigation with a 200ms debounce
    if (tick - last_nav > 200) {
        if (dir == W && shop_selected_index > 0) { shop_selected_index--; last_nav = tick; }
        if (dir == E && shop_selected_index < 2) { shop_selected_index++; last_nav = tick; }
    }
    
    // Purchase action handling
    if (bt2 && (tick - last_nav > 200)) {
        // Prevent buying multiple pro rods
        if (shop_selected_index == 2 && has_pro_rod) {
            Fish_RGB_Set(1, 0, 0); 
            HAL_Delay(100); 
            Fish_RGB_Set(0, 0, 0);
        } else if (Spend_Gold(shop_inventory[shop_selected_index].price)) {
            // Process successful purchase
            if (shop_selected_index == 0) inventory_bait++;
            if (shop_selected_index == 1) inventory_coffee++;
            if (shop_selected_index == 2) has_pro_rod = 1;
            
            Fish_RGB_Set(0, 1, 0); 
            HAL_Delay(100); 
            Fish_RGB_Set(0, 0, 0);
        } else {
            // Insufficient funds
            Fish_RGB_Set(1, 0, 0); 
            HAL_Delay(100); 
            Fish_RGB_Set(0, 0, 0);
        }
        last_nav = tick;
    }
    
    // Exit shop
    if (bt3) {
        HAL_Delay(100); 
        return true; 
    }
    return false;
}