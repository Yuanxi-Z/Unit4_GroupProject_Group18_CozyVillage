#include "FarmShop.h"
#include "../game_3/mine_palette.h"
#include "../game_3/mine_sprites.h"
#include "LCD.h"
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

/*
 * Farm shop module.
 * The shop currently sells one item: a seed pack.
 * The visual style is based on the mine shop scene.
 */

extern Buzzer_cfg_t buzzer_cfg;

/*
 * Internal shop state.
 * Browsing is the normal state; OK and NO are short feedback states after buying.
 */
typedef enum {
    SHOP_BROWSING,
    SHOP_FEEDBACK_OK,
    SHOP_FEEDBACK_NO,
} FarmShopSubState;

static FarmShopSubState shop_state;
static uint32_t last_feedback_tick;
static uint32_t last_input_tick;

/*
 * Non-blocking sound sequence state for shop feedback sounds.
 */
static const uint16_t *shop_seq_freqs = NULL;
static const uint16_t *shop_seq_durs  = NULL;
static uint8_t shop_seq_len = 0;
static uint8_t shop_seq_idx = 0;
static uint8_t shop_seq_active = 0;
static uint8_t shop_seq_volume = 0;
static uint32_t shop_sound_off_time = 0;
static uint8_t shop_sound_playing = 0;

/*
 * Seed pack item settings.
 */
#define SEED_PACK_PRICE    2
#define SEED_PACK_AMOUNT   1

/*
 * Buy success melody: short upward two-note sound.
 */
static const uint16_t SHOP_SOUND_BUY_FREQS[]  = {880, 1100};
static const uint16_t SHOP_SOUND_BUY_DURS[]   = {45, 70};

/*
 * Buy fail melody: clear downward three-note sound.
 */
static const uint16_t SHOP_SOUND_FAIL_FREQS[] = {880, 620, 420};
static const uint16_t SHOP_SOUND_FAIL_DURS[]  = {50, 60, 110};

/*
 * Stop all active shop sound output and clear the sequence state.
 */
static void shop_stop_sound(void)
{
    buzzer_off(&buzzer_cfg);

    shop_sound_playing = 0;
    shop_sound_off_time = 0;

    shop_seq_active = 0;
    shop_seq_idx = 0;
    shop_seq_len = 0;
    shop_seq_freqs = NULL;
    shop_seq_durs = NULL;
    shop_seq_volume = 0;
}

/*
 * Start a non-blocking buzzer sequence.
 * Each note duration is updated later by shop_update_sound().
 */
static void shop_start_sequence(const uint16_t *freqs,
                                const uint16_t *durs,
                                uint8_t length,
                                uint8_t volume)
{
    if (freqs == NULL || durs == NULL || length == 0) {
        return;
    }

    shop_seq_freqs = freqs;
    shop_seq_durs = durs;
    shop_seq_len = length;
    shop_seq_idx = 0;
    shop_seq_active = 1;
    shop_seq_volume = volume;

    buzzer_tone(&buzzer_cfg, shop_seq_freqs[0], shop_seq_volume);

    shop_sound_off_time = HAL_GetTick() + shop_seq_durs[0];
    shop_sound_playing = 1;
}

/*
 * Update the current shop sound sequence.
 * This keeps the shop logic non-blocking.
 */
static void shop_update_sound(uint32_t now_ms)
{
    if (!shop_seq_active) {
        if (shop_sound_playing && now_ms >= shop_sound_off_time) {
            buzzer_off(&buzzer_cfg);
            shop_sound_playing = 0;
        }
        return;
    }

    if (now_ms < shop_sound_off_time) {
        return;
    }

    shop_seq_idx++;

    if (shop_seq_idx >= shop_seq_len) {
        shop_stop_sound();
        return;
    }

    buzzer_tone(&buzzer_cfg, shop_seq_freqs[shop_seq_idx], shop_seq_volume);

    shop_sound_off_time = now_ms + shop_seq_durs[shop_seq_idx];
    shop_sound_playing = 1;
}

/*
 * Initialise shop state when entering the shop scene.
 */
void farm_shop_init(void)
{
    shop_state = SHOP_BROWSING;
    last_feedback_tick = 0;
    last_input_tick = 0;

    shop_stop_sound();
}

/*
 * Draw a small coin icon.
 */
static void draw_coin_icon(int cx, int cy)
{
    LCD_Draw_Circle(cx, cy, 6, CLR_AMBER, 1);
    LCD_Draw_Circle(cx, cy, 6, CLR_DARK_BROWN, 0);
    LCD_Draw_Circle(cx - 1, cy - 1, 2, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(cx, cy - 2, 1, 4, CLR_DARK_BROWN, 1);
}

/*
 * Draw a decorative coin jar on the shop counter.
 */
static void draw_coin_jar(int x, int y)
{
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

/*
 * Draw the seed pack item icon.
 */
static void draw_seed_bag_icon(int x, int y)
{
    LCD_Draw_Rect(x + 2, y + 2, 24, 22, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(x + 2, y + 2, 24, 22, CLR_DARK_BROWN, 0);

    LCD_Draw_Rect(x + 4, y, 20, 5, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 4, y, 20, 1, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 7, y + 5, 14, 2, CLR_DARK_BROWN, 1);

    LCD_Draw_Rect(x + 7, y + 8, 14, 9, CLR_CREAM, 1);
    LCD_Draw_Rect(x + 7, y + 8, 14, 9, CLR_DARK_BROWN, 0);

    LCD_Draw_Rect(x + 13, y + 11, 2, 4, CLR_LEAF_GREEN, 1);
    LCD_Draw_Rect(x + 10, y + 10, 3, 2, CLR_LEAF_GREEN, 1);
    LCD_Draw_Rect(x + 15, y + 10, 3, 2, CLR_LEAF_GREEN, 1);

    LCD_Draw_Circle(x + 9,  y + 20, 1, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 14, y + 21, 1, CLR_AMBER, 1);
    LCD_Draw_Circle(x + 18, y + 19, 1, CLR_AMBER, 1);
}

/*
 * Draw the product card for the seed pack.
 */
static void draw_product_card(int x, int y, const char* name, const char* price)
{
    LCD_Draw_Rect(x + 2, y + 2, 56, 66, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y, 56, 66, CLR_CREAM, 1);

    LCD_Draw_Rect(x, y,      56, 2,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y + 64, 56, 2,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x, y,       2, 66, CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(x + 54, y,  2, 66, CLR_DARK_BROWN, 1);

    LCD_Draw_Rect(x + 2, y + 2, 52, 10, CLR_DARK_BROWN, 1);

    /*
     * Centre the product name inside the card header.
     */
    int name_len = 0;
    while (name[name_len] && name_len < 12) {
        name_len++;
    }

    int name_w = name_len * 6 - 1;
    int nx = x + (56 - name_w) / 2;
    LCD_printString(name, nx, y + 4, CLR_CREAM, 1);

    draw_seed_bag_icon(x + 14, y + 16);

    LCD_Draw_Rect(x + 2, y + 52, 52, 12, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(x + 2, y + 52, 52, 1,  CLR_CREAM, 1);

    /*
     * Centre the price text in the bottom price area.
     */
    int price_len = 0;
    while (price[price_len] && price_len < 8) {
        price_len++;
    }

    int price_w = price_len * 6 - 1;
    int px = x + (56 - price_w) / 2;
    LCD_printString(price, px, y + 55, CLR_DARK_BROWN, 1);
}

/*
 * Update shop input and purchase logic.
 * BT2 buys one seed pack, while BT3 leaves the shop.
 */
FarmShopResult farm_shop_update(uint8_t bt2,
                                uint8_t bt3,
                                Direction dir,
                                uint32_t now_ms,
                                uint16_t *coins,
                                uint8_t *seeds)
{
    /*
     * Direction is reserved for future multi-item shop navigation.
     */
    (void)dir;

    shop_update_sound(now_ms);

    /*
     * Feedback expressions return to browsing after a short delay.
     */
    if (shop_state != SHOP_BROWSING && (now_ms - last_feedback_tick > 1000)) {
        shop_state = SHOP_BROWSING;
    }

    /*
     * BT2 purchase action with a simple debounce interval.
     */
    if (bt2 && (now_ms - last_input_tick > 200)) {
        if (*coins >= SEED_PACK_PRICE) {
            *coins -= SEED_PACK_PRICE;
            *seeds += SEED_PACK_AMOUNT;

            shop_state = SHOP_FEEDBACK_OK;

            shop_start_sequence(SHOP_SOUND_BUY_FREQS,
                                SHOP_SOUND_BUY_DURS,
                                2,
                                30);
        } else {
            shop_state = SHOP_FEEDBACK_NO;

            shop_start_sequence(SHOP_SOUND_FAIL_FREQS,
                                SHOP_SOUND_FAIL_DURS,
                                3,
                                38);
        }

        last_feedback_tick = now_ms;
        last_input_tick = now_ms;
    }

    /*
     * BT3 exits the shop scene.
     */
    if (bt3) {
        return FARM_SHOP_RESULT_LEAVE;
    }

    return FARM_SHOP_RESULT_STAY;
}

/*
 * Render the full seed shop scene.
 */
void farm_shop_render(uint32_t now_ms, uint16_t coins, uint8_t seeds)
{
    LCD_Set_Palette(PALETTE_CUSTOM);

    /*
     * Background wallpaper.
     */
    LCD_Fill_Buffer(CLR_CREAM);

    for (int y = 34; y < 102; y += 12) {
        for (int x = 8; x < 240; x += 18) {
            LCD_Draw_Rect(x, y, 2, 2, CLR_HONEY_GOLD, 1);
        }
    }

    /*
     * Top wooden title board.
     */
    LCD_Draw_Rect(0, 0, 240, 26, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(0, 0, 240, 3,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 23, 240, 3, CLR_DARK_BROWN, 1);

    for (int x = 20; x < 240; x += 40) {
        LCD_Draw_Rect(x, 3, 1, 20, CLR_DARK_BROWN, 1);
    }

    LCD_Draw_Rect(8,   10, 3, 3, CLR_HONEY_GOLD, 1);
    LCD_Draw_Rect(229, 10, 3, 3, CLR_HONEY_GOLD, 1);

    LCD_printString("SEED SHOP", 74, 7, CLR_CREAM, 2);

    /*
     * Shelf below the product area.
     */
    LCD_Draw_Rect(6, 104, 228, 5, CLR_TRUE_BROWN, 1);
    LCD_Draw_Rect(6, 108, 228, 2, CLR_DARK_BROWN, 1);

    /*
     * Main product card.
     */
    draw_product_card(92, 34, "SEED PACK", "2c");

    /*
     * Pulsing selection frame around the product card.
     */
    {
        uint8_t pulse = ((now_ms / 300) & 1);
        int sx = 92;
        int sy = 34;

        LCD_Draw_Rect(sx - 3 - pulse, sy - 3 - pulse,
                      62 + 2 * pulse, 72 + 2 * pulse,
                      CLR_CORAL, 0);

        LCD_Draw_Rect(sx - 2 - pulse, sy - 2 - pulse,
                      60 + 2 * pulse, 70 + 2 * pulse,
                      CLR_AMBER, 0);
    }

    /*
     * Counter area and shopkeeper position.
     */
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

    /*
     * Choose shopkeeper expression according to the latest purchase result.
     */
    const uint8_t* keeper_spr = SPR_SHOPKEEPER_IDLE;

    if (shop_state == SHOP_FEEDBACK_OK) {
        keeper_spr = SPR_SHOPKEEPER_HAPPY;
    } else if (shop_state == SHOP_FEEDBACK_NO) {
        keeper_spr = SPR_SHOPKEEPER_WORRY;
    }

    LCD_Draw_Sprite_Scaled(99, 85, 14, 14, (uint8_t*)keeper_spr, 3);

    /*
     * Bottom status area.
     */
    LCD_Draw_Rect(0, 157, 240, 3,  CLR_DARK_BROWN, 1);
    LCD_Draw_Rect(0, 160, 240, 80, CLR_SAGE, 1);
    LCD_Draw_Rect(0, 160, 240, 1,  CLR_CREAM, 1);

    draw_coin_icon(14, 176);

    {
        char buf[32];

        snprintf(buf, sizeof(buf), "COINS: %d", coins);
        LCD_printString(buf, 28, 170, CLR_CREAM, 2);
    }

    {
        char buf[32];

        snprintf(buf, sizeof(buf), "SEEDS: %d", seeds);
        LCD_printString(buf, 28, 188, CLR_CREAM, 2);
    }

    LCD_Draw_Rect(10, 206, 220, 1, CLR_DARK_BROWN, 1);

    /*
     * Control hint text.
     */
    LCD_printString("Buy +1 seed", 9, 212, CLR_DARK_BROWN, 1);
    LCD_printString("Buy +1 seed", 8, 212, CLR_DARK_BROWN, 1);

    LCD_printString("[BT2] Buy   [BT3] Leave", 9, 225, CLR_DARK_BROWN, 1);
    LCD_printString("[BT2] Buy   [BT3] Leave", 8, 225, CLR_DARK_BROWN, 1);
}