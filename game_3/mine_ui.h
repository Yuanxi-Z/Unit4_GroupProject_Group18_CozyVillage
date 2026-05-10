#ifndef MINE_UI_H
#define MINE_UI_H

#include <stdint.h>

// =============================================================================
// Cozy Miner UI toolkit
// -----------------------------------------------------------------------------
// Reusable Stardew-style widgets used by render, home, shop.
// All functions write to the LCD buffer via LCD primitives - no direct SPI.
// =============================================================================

// ---- Panels ----

/** Draw a wooden Stardew-style panel with border.
 *  Used for dialogs, HUD, popups. Interior is filled parchment-yellow.
 *  @param x,y  top-left corner
 *  @param w,h  width and height in pixels (minimum 16x16)
 */
void ui_panel(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/** Same as ui_panel but with a title strip at the top.
 *  Title bar is darker brown, text centered.
 *  @param title null-terminated string
 */
void ui_panel_titled(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     const char* title);

// ---- Progress bar ----

/** Horizontal progress bar. Outline in ink, fill color depends on value ratio.
 *  Above 50% = green, 25-50% = orange, below 25% = red (flashes if <10%).
 *  @param x,y       top-left of bar
 *  @param w,h       total bar dimensions
 *  @param value     current value
 *  @param max_value full-bar value
 *  @param now_ms    HAL_GetTick() for flash animation
 */
void ui_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     int16_t value, int16_t max_value, uint32_t now_ms);

// ---- HUD row ----

/** Draw the fixed HUD row at screen bottom:
 *    [torch bar] [copper count] [gold count] [diamond count] [coin total]
 *  Uses small icons from mine_sprites.h and gold text.
 *  @param torch_ticks  current torch (for color coding)
 *  @param torch_max    max torch
 *  @param cu,au,di     ore counts
 *  @param coins        coin total
 *  @param now_ms       for torch flash
 */
void ui_hud(int16_t torch_ticks, int16_t torch_max,
            uint8_t cu, uint8_t au, uint8_t di, uint16_t coins,
            uint32_t now_ms);

// ---- Selection highlight ----

/** Draw a yellow highlight rect around a selected item.
 *  Thicker border (2 px) so it reads well even when small.
 */
void ui_select_highlight(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

// ---- Text helpers ----

/** Draw small 5x7 text with shadow for depth. Used for HUD numbers.
 *  colour: palette index 0-15. Shadow is always CLR_INK.
 *  Note: uses LCD_printString internally - scale is always 1.
 */
void ui_text_shadow(const char* s, uint16_t x, uint16_t y, uint8_t colour);

#endif // MINE_UI_H
