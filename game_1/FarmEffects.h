#ifndef FARM_EFFECTS_H
#define FARM_EFFECTS_H

#include <stdint.h>

/*
 * Tile effect type.
 * These effects are drawn temporarily over a selected crop tile to give
 * immediate visual feedback after a farm action.
 */
typedef enum {
    FARM_EFFECT_NONE = 0,  /* No active tile effect. */
    FARM_EFFECT_PLANT,     /* Small sprout effect after planting. */
    FARM_EFFECT_WATER,     /* Water droplet effect after watering. */
    FARM_EFFECT_HARVEST    /* Sparkle effect after harvesting. */
} FarmEffectType;

/*
 * Popup message type.
 * The popup type controls the title and visual style of the message box.
 */
typedef enum {
    FARM_POPUP_NOTICE = 0, /* General information message. */
    FARM_POPUP_WARNING,    /* Warning message for invalid or risky actions. */
    FARM_POPUP_REST,       /* Night or resting message. */
    FARM_POPUP_WEATHER     /* Temperature or weather-related message. */
} FarmPopupType;

/*
 * Initialise the farm effects module.
 * This clears all temporary tile effects, popup messages, and HUD animations.
 */
void FarmEffects_Init(void);

/*
 * Update active farm effects.
 * Expired tile effects and popup messages are automatically disabled here.
 */
void FarmEffects_Update(void);

/*
 * Start a temporary visual effect on one crop tile.
 *
 * Parameters:
 *   type        - effect type to display.
 *   row         - target tile row in the farm grid.
 *   col         - target tile column in the farm grid.
 *   duration_ms - effect duration in milliseconds.
 */
void FarmEffects_StartTileEffect(FarmEffectType type,
                                 uint8_t row,
                                 uint8_t col,
                                 uint32_t duration_ms);

/*
 * Show a temporary popup message on screen.
 *
 * Parameters:
 *   type        - popup category used to select title and style.
 *   text        - popup body text.
 *   duration_ms - popup display duration in milliseconds.
 */
void FarmEffects_ShowPopup(FarmPopupType type,
                           const char *text,
                           uint32_t duration_ms);

/*
 * Trigger the HUD coin bounce animation.
 * This is usually called after a successful harvest or coin gain.
 */
void FarmEffects_TriggerCoinBounce(void);

/*
 * Check whether the coin bounce animation is currently active.
 *
 * Return:
 *   1 if the bounce effect should still be displayed, otherwise 0.
 */
uint8_t FarmEffects_IsCoinBounceActive(void);

/*
 * Draw a blinking border around a mature crop tile.
 * This helps the player notice crops that are ready to harvest.
 *
 * Parameters:
 *   x           - tile top-left X coordinate.
 *   y           - tile top-left Y coordinate.
 *   tile_size   - tile width and height in pixels.
 *   mature      - non-zero when the tile is mature.
 *   glow_colour - palette index used for the glow border.
 */
void FarmEffects_DrawMatureGlow(int x, int y, int tile_size,
                                uint8_t mature, uint8_t glow_colour);

/*
 * Draw the active tile effect overlay.
 * The stored tile row and column are converted into screen coordinates
 * using the tile layout parameters.
 *
 * Parameters:
 *   tile_start_x - X coordinate of the first tile.
 *   tile_start_y - Y coordinate of the first tile.
 *   tile_step    - distance between neighbouring tile origins.
 *   tile_size    - tile width and height in pixels.
 *   leaf_colour  - palette index used for planting effects.
 *   water_colour - palette index used for watering effects.
 *   gold_colour  - palette index used for harvest effects.
 */
void FarmEffects_DrawTileEffect(int tile_start_x,
                                int tile_start_y,
                                int tile_step,
                                int tile_size,
                                uint8_t leaf_colour,
                                uint8_t water_colour,
                                uint8_t gold_colour);

/*
 * Draw the active popup message.
 * If no popup is active, this function returns immediately.
 *
 * Parameters:
 *   screen_w      - screen width in pixels.
 *   popup_y       - reserved popup Y parameter for interface compatibility.
 *   bg_colour     - reserved background colour parameter.
 *   border_colour - reserved border colour parameter.
 *   text_colour   - reserved text colour parameter.
 */
void FarmEffects_DrawPopup(int screen_w,
                           int popup_y,
                           uint8_t bg_colour,
                           uint8_t border_colour,
                           uint8_t text_colour);

#endif