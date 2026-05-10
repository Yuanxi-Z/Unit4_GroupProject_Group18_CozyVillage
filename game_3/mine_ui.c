#include "mine_ui.h"
#include "mine_palette.h"
#include "mine_sprites.h"
#include "LCD.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// Panel - Stardew-style wooden frame
// =============================================================================

void ui_panel(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    LCD_Draw_Rect(x, y, w, h, CLR_WOOD_DARK, 1);
    LCD_Draw_Rect(x + 2, y + 2, w - 4, h - 4, CLR_PARCH, 1);
    LCD_Draw_Rect(x + 2, y + 2, w - 4, 1, CLR_WOOD_LIGHT, 1);
    LCD_Draw_Rect(x + 2, y + 2, 1, h - 4, CLR_WOOD_LIGHT, 1);
}

void ui_panel_titled(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     const char* title) {
    ui_panel(x, y, w, h);
    LCD_Draw_Rect(x + 2, y + 2, w - 4, 10, CLR_WOOD_MID, 1);

    int char_w = 6;
    int title_len = 0;
    while (title[title_len] && title_len < 24) title_len++;
    int total_w = title_len * char_w - 1;
    int tx = x + (w - total_w) / 2;
    int ty = y + 4;
    LCD_printString(title, tx, ty, CLR_CREAM, 1);
}

// =============================================================================
// Progress bar
// =============================================================================

void ui_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     int16_t value, int16_t max_value, uint32_t now_ms) {
    LCD_Draw_Rect(x, y, w, h, CLR_INK, 0);
    LCD_Draw_Rect(x + 1, y + 1, w - 2, h - 2, CLR_WOOD_DARK, 1);

    if (value <= 0 || max_value <= 0) return;
    if (value > max_value) value = max_value;

    int fill_w = ((int)value * (w - 2)) / max_value;
    if (fill_w < 1) fill_w = 1;

    uint8_t fill_color;
    int ratio_pct = (value * 100) / max_value;
    if (ratio_pct > 50) {
        fill_color = CLR_OK;
    } else if (ratio_pct > 25) {
        fill_color = CLR_COPPER;
    } else {
        if (ratio_pct <= 10 && ((now_ms / 250) & 1)) {
            fill_color = CLR_WOOD_DARK;
        } else {
            fill_color = CLR_DANGER;
        }
    }

    LCD_Draw_Rect(x + 1, y + 1, fill_w, h - 2, fill_color, 1);
}

// =============================================================================
// Text with shadow
// =============================================================================

void ui_text_shadow(const char* s, uint16_t x, uint16_t y, uint8_t colour) {
    LCD_printString(s, x + 1, y + 1, CLR_INK, 1);
    LCD_printString(s, x, y, colour, 1);
}

// =============================================================================
// Selection highlight
// =============================================================================

void ui_select_highlight(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    LCD_Draw_Rect(x, y, w, h, CLR_SELECT_EDGE, 0);
    LCD_Draw_Rect(x + 1, y + 1, w - 2, h - 2, CLR_SELECT_BG, 0);
}

// =============================================================================
// HUD row - bottom status bar (48 px tall, y=192..239)
// -----------------------------------------------------------------------------
//   - Icons: 10x10 source drawn at scale 2 -> 20x20 on screen
//   - Numbers: scale 2 (~10x14 per char)
//   - 5 slots, 48 px pitch across 240 px
//
//   NO gold/orange top border line - torch bar and HUD share CLR_HUD_BG
//   so they read as one smooth dark-blue status strip.
// =============================================================================

void ui_hud(int16_t torch_ticks, int16_t torch_max,
            uint8_t cu, uint8_t au, uint8_t di, uint16_t coins,
            uint32_t now_ms) {
    // Background bar (y=192..239)
    LCD_Draw_Rect(0, 192, 240, 48, CLR_HUD_BG, 1);

    char buf[8];
    static const int slot_x[5] = {4, 64, 102, 140, 178};
    static const uint8_t* slot_ico[5] = {
        ICO_TORCH, ICO_COPPER, ICO_GOLD, ICO_DIAMOND, ICO_COIN
    };
    uint16_t slot_val[5] = {
        (uint16_t)torch_ticks, cu, au, di, coins
    };

    uint16_t icon_y = 206;  // 20x20 icons centered in the 48px bar
    uint16_t text_y = 212;  // scale-2 text (~14px tall) centered

    for (int i = 0; i < 5; i++) {
        // 10x10 source icon, scale 2 -> 20x20 on screen
        LCD_Draw_Sprite_Scaled(slot_x[i], icon_y, 10, 10,
                               (uint8_t*)slot_ico[i], 2);

        snprintf(buf, sizeof(buf), "%d", slot_val[i]);

        uint8_t col = CLR_HUD_TEXT;
        if (i == 0 && torch_ticks <= 10 && ((now_ms / 300) & 1)) {
            col = CLR_DANGER;
        }

        LCD_printString(buf, slot_x[i] + 22, text_y, col, 2);
    }

    (void)torch_max;
}