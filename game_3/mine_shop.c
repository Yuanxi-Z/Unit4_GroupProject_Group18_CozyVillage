#include "mine_shop.h"
#include "mine_inventory.h"
#include "mine_audio.h"
#include "mine_palette.h"
#include "mine_sprites.h"
#include "LCD.h"
#include <stdio.h>

// =============================================================================
// PIERRE'S STORE - redesigned cozy Stardew-style interior
// =============================================================================

typedef enum {
    SHOP_BROWSING,
    SHOP_FEEDBACK_OK,
    SHOP_FEEDBACK_NO,
} ShopSubState;

static ShopSubState shop_state;
static uint8_t selected_item; // 0=Torch, 1=Dynamite, 2=Potion
static uint32_t last_feedback_tick;
static uint32_t last_input_tick;
static uint8_t just_sold;

void shop_init(void) {
    shop_state = SHOP_BROWSING;
    selected_item = 0;
    last_feedback_tick = 0;
    last_input_tick = 0;
    just_sold = 0;
}

ShopResult shop_update(uint8_t bt2, uint8_t bt3, Direction dir, uint32_t now_ms) {
    if (shop_state != SHOP_BROWSING && (now_ms - last_feedback_tick > 1000)) {
        shop_state = SHOP_BROWSING;
        just_sold = 0;
    }

    if (now_ms - last_input_tick > 200) {
        if (dir == W && selected_item > 0) {
            selected_item--;
            last_input_tick = now_ms;
        } else if (dir == E && selected_item < 2) {
            selected_item++;
            last_input_tick = now_ms;
        } else if (dir == N) {
            uint16_t earned = ore_sell_all();
            if (earned > 0) {
                audio_buy();
                shop_state = SHOP_FEEDBACK_OK;
                last_feedback_tick = now_ms;
                just_sold = 1;
            }
            last_input_tick = now_ms;
        }
    }

    if (bt2 && (now_ms - last_input_tick > 200)) {
        if (item_buy(selected_item)) {
            audio_buy();
            shop_state = SHOP_FEEDBACK_OK;
        } else {
            shop_state = SHOP_FEEDBACK_NO;
        }
        last_feedback_tick = now_ms;
        last_input_tick = now_ms;
    }

    if (bt3) {
        return SHOP_RESULT_LEAVE;
    }

    return SHOP_RESULT_STAY;
}

// -----------------------------------------------------------------------------
// Helper: draw a single framed product card
// -----------------------------------------------------------------------------
static void draw_product_card(int x, int y, const char* name, const uint8_t* icon,
                              const char* price) {
    // shadow (2px offset down-right) gives the card some lift
    LCD_Draw_Rect(x + 2, y + 2, 56, 66, CLR_DARK_BROWN, 1);
    // parchment body
    LCD_Draw_Rect(x, y, 56, 66, CLR_CREAM, 1);
    // dark wooden frame (2px border, 4 sides)
    LCD_Draw_Rect(x, y,       56, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y + 64,  56, 2, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y,        2, 66, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 54, y,   2, 66, CLR_DARK_BROWN, 1);

    // name strip
    LCD_Draw_Rect(x + 2, y + 2, 52, 10, CLR_DARK_BROWN, 1);
    int name_len = 0;
    while (name[name_len] && name_len < 10) name_len++;
    int name_w = name_len * 6 - 1;
    int nx = x + (56 - name_w) / 2;
    LCD_printString(name, nx, y + 4, CLR_CREAM, 1);

    // icon at center (14x14 scaled x2 = 28x28)
    // perfectly centered in the 56px wide card -> x + 14
    LCD_Draw_Sprite_Scaled(x + 14, y + 18, 14, 14, (uint8_t*)icon, 2);

    // price tag at bottom
    LCD_Draw_Rect(x + 2, y + 52, 52, 12, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 52, 52, 1, CLR_CREAM, 1);
    int price_len = 0;
    while (price[price_len] && price_len < 6) price_len++;
    int price_w = price_len * 6 - 1;
    int px = x + (56 - price_w) / 2;
    LCD_printString(price, px, y + 55, CLR_DARK_BROWN, 1);
}

// -----------------------------------------------------------------------------
// Helper: draw coin jar
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Helper: draw coin icon
// -----------------------------------------------------------------------------
static void draw_coin_icon(int cx, int cy) {
    LCD_Draw_Circle(cx, cy, 6, CLR_AMBER, 1);
    LCD_Draw_Circle(cx, cy, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Circle(cx - 1, cy - 1, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(cx, cy - 2, 1, 4, CLR_DARK_BROWN, 1);
}

void shop_render(uint32_t now_ms) {
    // ---- 1. WALL: warm cream background with dotted wallpaper ----
    LCD_Fill_Buffer(CLR_CREAM);
    for (int y = 34; y < 102; y += 12) {
        for (int x = 8; x < 240; x += 18) {
            LCD_Draw_Rect(x, y, 2, 2, CLR_HONEY_GOLD, 1);
        }
    }

    // ---- 2. TOP SIGN ----
    LCD_Draw_Rect(0, 0, 240, 26, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, 0, 240, 3,  CLR_DARK_BROWN, 1);   
    LCD_Draw_Rect(0, 23, 240, 3, CLR_DARK_BROWN, 1);   
    for (int x = 20; x < 240; x += 40) {
        LCD_Draw_Rect(x, 3, 1, 20, CLR_DARK_BROWN, 1);
    }
    LCD_Draw_Rect(8,   10, 3, 3, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(229, 10, 3, 3, CLR_HONEY_GOLD, 1);
    LCD_printString("PIERRE SHOP", 62, 7, CLR_CREAM, 2);

    // ---- 3. SHELF LINE under product cards ----
    LCD_Draw_Rect(6, 104, 228, 5, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(6, 108, 228, 2, CLR_DARK_BROWN, 1);

    // ---- 4. THREE PRODUCT CARDS ----
    static const int card_x[3] = {10, 92, 174};
    static const char* names[3]  = {"TORCH", "BOOM", "POTION"};
    static const uint8_t* icons[3] = { ICO_ITEM_TORCH, ICO_ITEM_DYNAMITE, ICO_ITEM_POTION };
    static const char* prices[3] = {"20c", "30c", "50c"};

    for (int i = 0; i < 3; i++) {
        draw_product_card(card_x[i], 34, names[i], icons[i], prices[i]);
    }

    // ---- 5. SELECTION HIGHLIGHT ----
    {
        uint8_t pulse = ((now_ms / 300) & 1);
        int sx = card_x[selected_item];
        int sy = 34;
        LCD_Draw_Rect(sx - 3 - pulse, sy - 3 - pulse,
                      62 + 2 * pulse, 72 + 2 * pulse,
                      CLR_CORAL, 0);
        LCD_Draw_Rect(sx - 2 - pulse, sy - 2 - pulse,
                      60 + 2 * pulse, 70 + 2 * pulse,
                      CLR_AMBER, 0);
    }

    // ---- 6. COMPRESSED COUNTER (Lowered to y=125) ----
    LCD_Draw_Rect(0, 125, 240, 3, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 128, 240, 29, CLR_TRUE_BROWN, 1); // height: 29px
    for (int x = 60; x < 240; x += 60) {
        LCD_Draw_Rect(x, 128, 1, 29, CLR_DARK_BROWN, 1);
    }
    LCD_Draw_Rect(8,   135, 48, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(130, 132, 54, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(195, 142, 40, 1, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 155, 240, 2, CLR_DARK_BROWN, 1);

    // ---- 7. COIN JAR ----
    draw_coin_jar(18, 126);

    // ---- 8. SHOPKEEPER (42x42 Sprite) ----
    const uint8_t* keeper_spr = SPR_SHOPKEEPER_IDLE;
    if (shop_state == SHOP_FEEDBACK_OK) keeper_spr = SPR_SHOPKEEPER_HAPPY;
    else if (shop_state == SHOP_FEEDBACK_NO) keeper_spr = SPR_SHOPKEEPER_WORRY;
    
    // If scale 3 is too large, change '3' to '2' and adjust coordinates to 106, 97 to reposition.
    LCD_Draw_Sprite_Scaled(99, 85, 14, 14, (uint8_t*)keeper_spr, 3);

    // "$$$" floaty text
    if (just_sold) {
        LCD_printString("$$$", 150, 90, CLR_HONEY_GOLD, 2);
    }

    // ---- 9. BOTTOM INFO PANEL ----
    LCD_Draw_Rect(0, 157, 240, 3,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 160, 240, 80, CLR_SAGE, 1);
    LCD_Draw_Rect(0, 160, 240, 1,  CLR_CREAM, 1);

    draw_coin_icon(14, 178);
    char buf[32];
    snprintf(buf, sizeof(buf), "COINS: %d", coins);
    LCD_printString(buf, 28, 172, CLR_CREAM, 2);

    LCD_Draw_Rect(10, 193, 220, 1, CLR_DARK_BROWN, 1);

    LCD_printString("[UP] Sell ores", 8, 200, CLR_CREAM, 1);
    LCD_printString("[L/R] Select  [BT2] Buy", 8, 213, CLR_CREAM, 1);
    LCD_printString("[BT3] Leave", 8, 226, CLR_HONEY_GOLD, 1);
}