#ifndef FISH_RENDER_H
#define FISH_RENDER_H

#include <stdint.h>

// Render the shoreline, water, shop building, gold HUD, and bait count
void Fish_Render_Environment(uint32_t tick, int bait_count);

// Render the fishing rod (color changes if has_pro_rod is 1)
void Fish_Render_Thick_Rod(int x1, int y1, int x2, int y2, int has_pro_rod);

// Render the Stardew Valley style fishing tension bar
void Fish_Render_Fishing_Bar(int x0, int y0, int h, int tension);

// Render a generic progress bar (used for Fish HP)
void Fish_Render_Progress_Bar(int x, int y, int width, int height, int value, int max_val, uint8_t fill_color, const char* label);

// Render the large ASCII art fish during the result screen
void Fish_Render_Big_Fish(int x, int y, int type, uint32_t tick);

extern const uint8_t FISH_SPR_GIRL_IDLE[14 * 14];
extern const uint8_t FISH_SPR_GIRL_WALK_A[14 * 14];
extern const uint8_t FISH_SPR_GIRL_CAST[14 * 14]; // Casting/Reeling action

#endif // FISH_RENDER_H