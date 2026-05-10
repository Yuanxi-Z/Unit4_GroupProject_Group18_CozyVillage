#include "FarmEffects.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"
#include <string.h>
#include <stdio.h>

/*
 * Runtime state for one temporary tile effect.
 * Only one tile effect is active at a time in this module.
 */
typedef struct {
    FarmEffectType type;       /* Current effect type: plant, water, harvest, or none. */
    uint8_t row;               /* Target tile row in the farm grid. */
    uint8_t col;               /* Target tile column in the farm grid. */
    uint8_t active;            /* 1 when the effect is currently visible. */
    uint32_t start_time;       /* HAL tick time when the effect started. */
    uint32_t duration_ms;      /* Effect lifetime in milliseconds. */
} FarmTileEffect;

/*
 * Runtime state for one popup message.
 * The popup is drawn over the current scene for a limited time.
 */
typedef struct {
    uint8_t active;            /* 1 when a popup should be displayed. */
    FarmPopupType type;        /* Popup category used to select title and frame colour. */
    char text[32];             /* Popup body text. */
    uint32_t start_time;       /* HAL tick time when the popup appeared. */
    uint32_t duration_ms;      /* Popup lifetime in milliseconds. */
} FarmPopupMessage;

/*
 * Global effect states.
 * These variables are private to this file and are updated through the
 * FarmEffects public functions.
 */
static FarmTileEffect g_tile_effect;
static FarmPopupMessage g_popup;

/*
 * Start time of the coin bounce animation.
 * If the current time is close enough to this value, the coin icon can be
 * rendered with a small bounce offset.
 */
static uint32_t g_coin_bounce_time = 0;

/*
 * Initialise all farm visual effect states.
 * This should be called when the farm module starts or resets.
 */
void FarmEffects_Init(void)
{
    memset(&g_tile_effect, 0, sizeof(g_tile_effect));
    memset(&g_popup, 0, sizeof(g_popup));
    g_coin_bounce_time = 0;
}

/*
 * Update temporary visual effects.
 * This function clears expired tile effects and popup messages.
 */
void FarmEffects_Update(void)
{
    uint32_t now = HAL_GetTick();

    /*
     * Stop the tile effect after its configured duration.
     */
    if (g_tile_effect.active &&
        (now - g_tile_effect.start_time >= g_tile_effect.duration_ms)) {
        g_tile_effect.active = 0;
        g_tile_effect.type = FARM_EFFECT_NONE;
    }

    /*
     * Hide the popup after its configured duration.
     */
    if (g_popup.active &&
        (now - g_popup.start_time >= g_popup.duration_ms)) {
        g_popup.active = 0;
        g_popup.text[0] = '\0';
    }
}

/*
 * Start a temporary effect on one crop tile.
 * The effect position is stored as farm grid row and column rather than
 * absolute screen coordinates.
 */
void FarmEffects_StartTileEffect(FarmEffectType type,
                                 uint8_t row,
                                 uint8_t col,
                                 uint32_t duration_ms)
{
    g_tile_effect.type = type;
    g_tile_effect.row = row;
    g_tile_effect.col = col;
    g_tile_effect.active = 1;
    g_tile_effect.start_time = HAL_GetTick();
    g_tile_effect.duration_ms = duration_ms;
}

/*
 * Show a temporary popup message.
 * The text is copied into a fixed-size buffer to keep the popup state safe
 * after the original string goes out of scope.
 */
void FarmEffects_ShowPopup(FarmPopupType type,
                           const char *text,
                           uint32_t duration_ms)
{
    g_popup.type = type;
    snprintf(g_popup.text, sizeof(g_popup.text), "%s", text);
    g_popup.active = 1;
    g_popup.start_time = HAL_GetTick();
    g_popup.duration_ms = duration_ms;
}

/*
 * Trigger the coin bounce animation.
 * The renderer can call FarmEffects_IsCoinBounceActive() to decide whether
 * the coin should be drawn with a bounce effect.
 */
void FarmEffects_TriggerCoinBounce(void)
{
    g_coin_bounce_time = HAL_GetTick();
}

/*
 * Check whether the coin bounce animation is still active.
 * The bounce lasts for a short fixed duration after a successful harvest.
 */
uint8_t FarmEffects_IsCoinBounceActive(void)
{
    return ((HAL_GetTick() - g_coin_bounce_time) < 250U) ? 1U : 0U;
}

/*
 * Draw a blinking outline around a mature crop.
 * The glow only appears when mature is true, and it blinks every 250 ms.
 */
void FarmEffects_DrawMatureGlow(int x, int y, int tile_size,
                                uint8_t mature, uint8_t glow_colour)
{
    if (!mature) {
        return;
    }

    uint32_t now = HAL_GetTick();

    /*
     * Toggle the glow on and off to make mature crops easier to notice.
     */
    if (((now / 250U) % 2U) == 0U) {
        LCD_Draw_Rect(x - 1, y - 1, tile_size + 2, tile_size + 2, glow_colour, 0);
    }
}

/*
 * Draw the currently active tile effect.
 * The effect is converted from grid coordinates into screen coordinates.
 */
void FarmEffects_DrawTileEffect(int tile_start_x,
                                int tile_start_y,
                                int tile_step,
                                int tile_size,
                                uint8_t leaf_colour,
                                uint8_t water_colour,
                                uint8_t gold_colour)
{
    if (!g_tile_effect.active) {
        return;
    }

    /*
     * Calculate the selected tile origin and centre point.
     */
    int tx = tile_start_x + g_tile_effect.col * tile_step;
    int ty = tile_start_y + g_tile_effect.row * tile_step;
    int cx = tx + tile_size / 2;
    int cy = ty + tile_size / 2;

    switch (g_tile_effect.type) {
        case FARM_EFFECT_PLANT:
            /*
             * Plant effect: small sprout shape.
             * It gives immediate visual feedback after a seed is planted.
             */
            LCD_Draw_Rect(cx - 1, cy + 2, 2, 4, leaf_colour, 1);
            LCD_Draw_Rect(cx - 4, cy, 3, 2, leaf_colour, 1);
            LCD_Draw_Rect(cx + 1, cy, 3, 2, leaf_colour, 1);
            break;

        case FARM_EFFECT_WATER:
            /*
             * Water effect: falling water droplets.
             * It appears briefly when the player waters a crop.
             */
            LCD_Draw_Rect(cx - 5, cy - 3, 2, 3, water_colour, 1);
            LCD_Draw_Rect(cx,     cy - 1, 2, 3, water_colour, 1);
            LCD_Draw_Rect(cx + 4, cy - 4, 2, 3, water_colour, 1);
            break;

        case FARM_EFFECT_HARVEST:
            /*
             * Harvest effect: bright cross sparkle.
             * It is used after collecting a mature crop.
             */
            LCD_Draw_Rect(cx - 1, cy - 5, 2, 10, gold_colour, 1);
            LCD_Draw_Rect(cx - 5, cy - 1, 10, 2, gold_colour, 1);
            LCD_Draw_Rect(cx - 3, cy - 3, 6, 6, gold_colour, 0);
            break;

        case FARM_EFFECT_NONE:
        default:
            break;
    }
}

/*
 * Split popup text into two short display lines.
 * The function first tries to split at the last space character.
 * If no space exists, it cuts the text near the middle.
 */
static void FarmEffects_SplitPopupText(const char *src,
                                       char *line1,
                                       char *line2,
                                       int max1,
                                       int max2)
{
    int len = (int)strlen(src);
    int split = -1;

    line1[0] = '\0';
    line2[0] = '\0';

    /*
     * Find the last space in the source text.
     * Splitting at a space keeps the popup message easier to read.
     */
    for (int i = 0; i < len; i++) {
        if (src[i] == ' ') {
            split = i;
        }
    }

    if (split > 0) {
        int left_len = split;
        int right_len = len - split - 1;

        /*
         * Limit both lines so that the fixed-size buffers are not overrun.
         */
        if (left_len >= max1) {
            left_len = max1 - 1;
        }

        if (right_len >= max2) {
            right_len = max2 - 1;
        }

        strncpy(line1, src, left_len);
        line1[left_len] = '\0';

        strncpy(line2, src + split + 1, right_len);
        line2[right_len] = '\0';
    } else {
        /*
         * If the message has no space, split it into two approximate halves.
         */
        int cut = len / 2;

        if (cut >= max1) {
            cut = max1 - 1;
        }

        strncpy(line1, src, cut);
        line1[cut] = '\0';

        {
            int rest = len - cut;

            if (rest >= max2) {
                rest = max2 - 1;
            }

            strncpy(line2, src + cut, rest);
            line2[rest] = '\0';
        }
    }
}

/*
 * Draw centred text with a simple bold effect.
 * The text is printed twice with a one-pixel horizontal offset.
 */
static void FarmEffects_DrawBoldCenteredText(const char *text,
                                             int box_x,
                                             int box_y,
                                             int box_w,
                                             uint8_t colour,
                                             uint8_t size)
{
    int len = 0;

    while (text[len] != '\0') {
        len++;
    }

    /*
     * Estimate text width according to the font scale used by LCD_printString().
     */
    int char_w = (size == 2) ? 12 : 6;
    int text_w = len * char_w;
    int x = box_x + (box_w - text_w) / 2;

    /*
     * Keep the text inside the popup box even if the message is slightly long.
     */
    if (x < box_x + 4) {
        x = box_x + 4;
    }

    LCD_printString(text, x + 1, box_y, colour, size);
    LCD_printString(text, x,     box_y, colour, size);
}

/*
 * Draw the active popup message.
 * The popup uses a small pixel-style frame and different titles/colours
 * depending on the popup type.
 */
void FarmEffects_DrawPopup(int screen_w,
                           int popup_y,
                           uint8_t bg_colour,
                           uint8_t border_colour,
                           uint8_t text_colour)
{
    /*
     * These parameters are kept for interface compatibility.
     * The current popup design uses a fixed centred layout and internal colours.
     */
    (void)popup_y;
    (void)bg_colour;
    (void)border_colour;
    (void)text_colour;

    if (!g_popup.active) {
        return;
    }

    /*
     * Fixed popup size and centred screen position.
     * The display is 240 pixels high in this project.
     */
    int w = 156;
    int h = 82;
    int x = (screen_w - w) / 2;
    int y = (240 - h) / 2;

    char line1[20];
    char line2[20];
    const char *title = "NOTICE";

    /*
     * Default popup colours.
     * These are palette indices from the unified farm palette.
     */
    uint8_t body_colour = 1;       /* Cream body background. */
    uint8_t frame_colour = 6;      /* Brown default frame. */
    uint8_t title_text_colour = 1; /* Cream title text. */
    uint8_t body_text_colour = 6;  /* Brown body text. */

    FarmEffects_SplitPopupText(g_popup.text, line1, line2, sizeof(line1), sizeof(line2));

    /*
     * Select the popup title and frame colour according to message type.
     */
    switch (g_popup.type) {
        case FARM_POPUP_WARNING:
            title = "WARNING";
            frame_colour = 8;      /* Honey gold frame for warnings. */
            body_text_colour = 6;  /* Brown body text. */
            break;

        case FARM_POPUP_REST:
            title = "REST";
            frame_colour = 12;     /* Night blue frame for night/rest messages. */
            body_text_colour = 12; /* Night blue body text. */
            break;

        case FARM_POPUP_WEATHER:
            title = "WEATHER";
            frame_colour = 4;      /* Denim blue frame for weather messages. */
            body_text_colour = 6;  /* Brown body text. */
            break;

        case FARM_POPUP_NOTICE:
        default:
            title = "NOTICE";
            frame_colour = 6;      /* Brown frame for general messages. */
            body_text_colour = 6;  /* Brown body text. */
            break;
    }

    /*
     * Draw the popup drop shadow and main background panel.
     */
    LCD_Draw_Rect(x + 4, y + 4, w, h, frame_colour, 1);
    LCD_Draw_Rect(x, y, w, h, body_colour, 1);

    /*
     * Draw the outer frame.
     */
    LCD_Draw_Rect(x,         y,         w, 3, frame_colour, 1);
    LCD_Draw_Rect(x,         y + h - 3, w, 3, frame_colour, 1);
    LCD_Draw_Rect(x,         y,         3, h, frame_colour, 1);
    LCD_Draw_Rect(x + w - 3, y,         3, h, frame_colour, 1);

    /*
     * Draw the title bar.
     */
    LCD_Draw_Rect(x + 3, y + 3, w - 6, 16, frame_colour, 1);
    FarmEffects_DrawBoldCenteredText(title, x + 3, y + 7, w - 6, title_text_colour, 1);

    /*
     * Draw the inner text box.
     */
    LCD_Draw_Rect(x + 8, y + 25, w - 16, h - 33, 1, 1);
    LCD_Draw_Rect(x + 8, y + 25,  w - 16, 2,      frame_colour, 1);
    LCD_Draw_Rect(x + 8, y + h - 10, w - 16, 2,   frame_colour, 1);
    LCD_Draw_Rect(x + 8, y + 25,  2,      h - 33, frame_colour, 1);
    LCD_Draw_Rect(x + w - 10, y + 25, 2, h - 33, frame_colour, 1);

    /*
     * Draw small pixel-style corner decorations inside the popup.
     */
    LCD_Draw_Rect(x + 12,     y + 29,     4, 4, frame_colour, 1);
    LCD_Draw_Rect(x + w - 16, y + 29,     4, 4, frame_colour, 1);
    LCD_Draw_Rect(x + 12,     y + h - 18, 4, 4, frame_colour, 1);
    LCD_Draw_Rect(x + w - 16, y + h - 18, 4, 4, frame_colour, 1);

    /*
     * Draw the two-line popup message.
     */
    FarmEffects_DrawBoldCenteredText(line1, x + 12, y + 38, w - 24, body_text_colour, 1);
    FarmEffects_DrawBoldCenteredText(line2, x + 12, y + 52, w - 24, body_text_colour, 1);
}