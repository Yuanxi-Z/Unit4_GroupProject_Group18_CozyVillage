#include "mine_render.h"
#include "mine_ui.h"
#include "mine_palette.h"
#include "mine_sprites.h"
#include "mine_map.h"
#include "mine_player.h"
#include "mine_inventory.h"
#include "LCD.h"
#include "mine_bat.h" 
#include <stdio.h>
#include <stdlib.h>

// Visible window at 28px tiles: 8 cols x 6 rows = 224 x 168 px.
#define VIS_COLS  8
#define VIS_ROWS  6

#define MAP_RIGHT_MARGIN_X  (VIS_COLS * TILE_PX)
#define MAP_RIGHT_MARGIN_W  (240 - VIS_COLS * TILE_PX)

static int16_t cam_x;
static int16_t cam_y;

static void camera_update(void) {
    cam_x = player.x - VIS_COLS / 2;
    cam_y = player.y - VIS_ROWS / 2;
    if (cam_x < 0) cam_x = 0;
    if (cam_y < 0) cam_y = 0;
    if (cam_x > MAP_W - VIS_COLS) cam_x = MAP_W - VIS_COLS;
    if (cam_y > MAP_H - VIS_ROWS) cam_y = MAP_H - VIS_ROWS;
    if (cam_x < 0) cam_x = 0;
    if (cam_y < 0) cam_y = 0;
}

static uint8_t vis_radius_from_torch(int16_t torch) {
    if (torch > 40) return 3;
    if (torch > 20) return 2;
    if (torch >  0) return 1;
    return 0;
}

static uint8_t is_visible(int tile_x, int tile_y, uint8_t radius) {
    int dx = abs(tile_x - player.x);
    int dy = abs(tile_y - player.y);
    int d = dx + dy;
    return (d <= radius);
}

static const uint8_t* pick_wall_sprite(int x, int y) {
    uint8_t variant = ((x * 7) + (y * 3)) & 3;
    switch (variant) {
        case 0: return SPR_WALL_A;
        case 1: return SPR_WALL_B;
        case 2: return SPR_WALL_C;
        default: return SPR_WALL_D;
    }
}

static const uint8_t* pick_floor_sprite(int x, int y) {
    return ((x + y) & 1) ? SPR_FLOOR_A : SPR_FLOOR_B;
}

static inline void draw_tile(int px, int py, const uint8_t* spr) {
    LCD_Draw_Sprite_Scaled(px, py, 14, 14, (uint8_t*)spr, 2);
}

static int16_t prev_cam_x = -99, prev_cam_y = -99;
static int16_t prev_px = -99, prev_py = -99;
static uint8_t prev_radius = 255;
static int prev_hash = -1;
static uint8_t margin_painted = 0;

void render_map(uint32_t now_ms) {
    camera_update();
    uint8_t radius = vis_radius_from_torch(player.torch_ticks);

    // State hash tracking (includes poison status)
    int current_hash = ores.copper + (ores.gold * 10) + (ores.diamond * 100) + player.torch_ticks + (player.is_poisoned * 777);
    
    for (int i = 0; i < bat_count; i++) {
        if (bats[i].alive) {
            current_hash += (bats[i].x * 13) + (bats[i].y * 17) + (bats[i].anim_frame * 31);
        }
    }

    // Force redraw every 60ms when poisoned for smooth bubble animation without ghosting.
    // 60ms is slower than per-frame (30fps=33ms) but visually smooth enough.
    if (player.is_poisoned) {
        current_hash += (int)(now_ms / 60);
    }

    uint8_t full_redraw = 0;
    if (cam_x != prev_cam_x || cam_y != prev_cam_y ||
        radius != prev_radius || current_hash != prev_hash) {
        full_redraw = 1;
    }

    uint8_t player_moved = (player.x != prev_px || player.y != prev_py);

    if (!margin_painted || full_redraw) {
        LCD_Draw_Rect(MAP_RIGHT_MARGIN_X, MAP_AREA_TOP,
                      MAP_RIGHT_MARGIN_W, MAP_AREA_BOTTOM,
                      CLR_HUD_BG, 1);
        margin_painted = 1;
    }

    if (full_redraw || player_moved) {
        if (full_redraw) {
            LCD_Draw_Rect(0, MAP_AREA_TOP, MAP_RIGHT_MARGIN_X,
                          MAP_AREA_BOTTOM - MAP_AREA_TOP, CLR_BLACK, 1);
        }

        for (int vy = 0; vy < VIS_ROWS; vy++) {
            for (int vx = 0; vx < VIS_COLS; vx++) {
                int tx = cam_x + vx;
                int ty = cam_y + vy;

                if (!full_redraw) {
                    int dx_old = abs(tx - prev_px), dy_old = abs(ty - prev_py);
                    int dx_new = abs(tx - player.x), dy_new = abs(ty - player.y);
                    int d_old = (dx_old > dy_old) ? dx_old : dy_old;
                    int d_new = (dx_new > dy_new) ? dx_new : dy_new;

                    if (d_old > 1 && d_new > 1) {
                        continue;
                    }
                    LCD_Draw_Rect(vx * TILE_PX, MAP_AREA_TOP + vy * TILE_PX,
                                  TILE_PX, TILE_PX, CLR_BLACK, 1);
                }

                if (tx < 0 || tx >= MAP_W || ty < 0 || ty >= MAP_H) continue;
                if (!is_visible(tx, ty, radius)) continue;

                uint8_t tile = map_get(tx, ty);
                int px = vx * TILE_PX;
                int py = MAP_AREA_TOP + vy * TILE_PX;

                switch (tile) {
                    case TILE_WALL:
                        draw_tile(px, py, pick_wall_sprite(tx, ty));
                        // Floor 2: Cover ~55% of the tile with large purple blocks
                        // instead of scattered dots. Large blocks are easier to identify
                        // as "a purple wall", distinguishing it from Floor 1's brown walls.
                        if (player.depth == FLOOR_DEEP) {
                            // Top bar (y 0-8)
                            LCD_Draw_Rect(px+0,  py+0,  12, 8, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+16, py+0,  12, 6, CLR_DUSK_PURPLE, 1);
                            // Middle bar (y 10-18)
                            LCD_Draw_Rect(px+4,  py+10, 8, 8, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+14, py+12, 6, 6, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+22, py+8,  6, 10, CLR_DUSK_PURPLE, 1);
                            // Bottom bar (y 20-28)
                            LCD_Draw_Rect(px+0,  py+20, 10, 8, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+12, py+22, 8, 6, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+22, py+22, 6, 6, CLR_DUSK_PURPLE, 1);
                        }
                        break;

                    case TILE_GEODE:
                        draw_tile(px, py, pick_wall_sprite(tx, ty));
                        // Floor 2 geode: Large purple blocks at corners, leaving the center for flashing gold
                        if (player.depth == FLOOR_DEEP) {
                            LCD_Draw_Rect(px+0,  py+0,  10, 8, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+20, py+0,  8, 8, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+0,  py+20, 8, 8,  CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+20, py+20, 8, 8,  CLR_DUSK_PURPLE, 1);
                        }
                        {
                            uint8_t sparkle = ((now_ms / 350) & 3);
                            if (sparkle == 0 || sparkle == 1) {
                                LCD_Draw_Rect(px+12, py+12, 4, 4, CLR_HONEY_GOLD, 1);
                                LCD_Draw_Rect(px+10, py+14, 2, 2, CLR_AMBER, 1);
                                LCD_Draw_Rect(px+16, py+14, 2, 2, CLR_AMBER, 1);
                                LCD_Draw_Rect(px+14, py+10, 2, 2, CLR_AMBER, 1);
                                LCD_Draw_Rect(px+14, py+16, 2, 2, CLR_AMBER, 1);
                            } else if (sparkle == 2) {
                                LCD_Draw_Rect(px+12, py+14, 4, 2, CLR_HONEY_GOLD, 1);
                                LCD_Draw_Rect(px+14, py+12, 2, 4, CLR_HONEY_GOLD, 1);
                            }
                        }
                        break;

                    case TILE_FLOOR:
                        draw_tile(px, py, pick_floor_sprite(tx, ty));
                        // Floor 2 floor: Large purple spots
                        // to make the ground look distinctly different
                        if (player.depth == FLOOR_DEEP) {
                            LCD_Draw_Rect(px+2,  py+4,  8, 4, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+14, py+10, 6, 5, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+4,  py+18, 7, 4, CLR_DUSK_PURPLE, 1);
                            LCD_Draw_Rect(px+18, py+22, 6, 4, CLR_DUSK_PURPLE, 1);
                        }
                        break;

                    case TILE_ORE_CU:
                        draw_tile(px, py, pick_floor_sprite(tx, ty));
                        draw_tile(px, py, SPR_ORE_COPPER);
                        break;

                    case TILE_ORE_AU:
                        draw_tile(px, py, pick_floor_sprite(tx, ty));
                        draw_tile(px, py, SPR_ORE_GOLD);
                        break;

                    case TILE_ITEM_DIAMOND:
                        draw_tile(px, py, pick_floor_sprite(tx, ty));
                        draw_tile(px, py, SPR_ORE_DIAMOND);
                        {
                            uint8_t s = ((now_ms / 250) & 3);
                            if (s == 0) {
                                LCD_Draw_Rect(px+4,  py+4,  2, 2, CLR_CREAM, 1);
                                LCD_Draw_Rect(px+22, py+8,  2, 2, CLR_CREAM, 1);
                            } else if (s == 1) {
                                LCD_Draw_Rect(px+20, py+20, 2, 2, CLR_CREAM, 1);
                                LCD_Draw_Rect(px+6,  py+18, 2, 2, CLR_CREAM, 1);
                            } else if (s == 2) {
                                LCD_Draw_Rect(px+12, py+2,  2, 2, CLR_CREAM, 1);
                            }
                        }
                        break;

                    case TILE_EXIT:
                        draw_tile(px, py, SPR_EXIT_STAIRS);
                        break;
                }
            }
        }

        for (int i = 0; i < bat_count; i++) {
            if (bats[i].alive) {
                if (is_visible(bats[i].x, bats[i].y, radius)) {
                    int bvx = bats[i].x - cam_x;
                    int bvy = bats[i].y - cam_y;
                    
                    if (bvx >= 0 && bvx < VIS_COLS && bvy >= 0 && bvy < VIS_ROWS) {
                        int bpx = bvx * TILE_PX;
                        int bpy = MAP_AREA_TOP + bvy * TILE_PX;
                        const uint8_t* bat_spr = (bats[i].anim_frame == 0) ? SPR_BAT_A : SPR_BAT_B;
                        draw_tile(bpx, bpy, bat_spr);
                    }
                }
            }
        }
    }

    int player_vx = player.x - cam_x;
    int player_vy = player.y - cam_y;
    int ppx = player_vx * TILE_PX;
    int ppy = MAP_AREA_TOP + player_vy * TILE_PX;

    const uint8_t* player_sprite;
    if (player_is_mining(now_ms)) {
        player_sprite = SPR_GIRL_MINE;
    } else if (player.anim_frame == 0) {
        player_sprite = SPR_GIRL_IDLE;
    } else {
        player_sprite = SPR_GIRL_WALK_A;
    }
    LCD_Draw_Sprite_Scaled(ppx, ppy, 14, 14, (uint8_t*)player_sprite, 2);

    // If poisoned, draw a warning exclamation mark above the head
    if (player.is_poisoned) {
        // =====================================================================
        // Poison Visuals v3:
        //   1) Player has a 3px thick purple outline
        //   2) 3 giant bubbles above the head, r=4..6 (clearly visible spheres)
        //   3) Screen edges pulse with a 4px thick purple border
        //   4) Large red "!" mark with gold border above the head (eye-catching)
        // =====================================================================

        // --- 1. Player outline: 3 px thick purple halo ---
        LCD_Draw_Rect(ppx - 3, ppy - 3,      TILE_PX + 6, 3, CLR_DUSK_PURPLE, 1);
        LCD_Draw_Rect(ppx - 3, ppy + TILE_PX, TILE_PX + 6, 3, CLR_DUSK_PURPLE, 1);
        LCD_Draw_Rect(ppx - 3, ppy,          3, TILE_PX,     CLR_DUSK_PURPLE, 1);
        LCD_Draw_Rect(ppx + TILE_PX, ppy,    3, TILE_PX,     CLR_DUSK_PURPLE, 1);

        // --- 2. LARGE floating poison bubbles (r=4..6) ---
        // 3 large bubbles, different phases, looping upwards from 0 to 30.
        uint32_t t = now_ms / 80;
        static const int8_t bubble_offsets[3][2] = {
            // x relative to sprite left, radius
            { 4, 4},
            {14, 6},  // The middle one is the largest and most prominent
            {24, 4},
        };
        for (int b = 0; b < 3; b++) {
            int phase = b * 10;
            int bubble_y_offset = (int)((t + phase) % 30);
            int by = ppy - 10 - bubble_y_offset;
            int bx = ppx + bubble_offsets[b][0];
            int r  = bubble_offsets[b][1];

            if (by >= r && by < MAP_AREA_BOTTOM) {
                // outer dusk purple body
                LCD_Draw_Circle(bx, by, r, CLR_DUSK_PURPLE, 1);
                // darker purple outline ring - makes bubble edge crisp
                LCD_Draw_Circle(bx, by, r, CLR_DARK_BROWN, 0);
                // cream highlight for glossy bubble look (top-left of bubble)
                LCD_Draw_Rect(bx - r + 1, by - r + 1, 2, 2, CLR_CREAM, 1);
            }
        }

        // --- 3. Thick screen-edge vignette pulse (4px, every 400ms) ---
        uint8_t pulse = ((now_ms / 200) & 1) ? 4 : 3;
        LCD_Draw_Rect(0, MAP_AREA_TOP, 240, pulse, CLR_DUSK_PURPLE, 1);
        LCD_Draw_Rect(0, MAP_AREA_BOTTOM - pulse, 240, pulse, CLR_DUSK_PURPLE, 1);
        LCD_Draw_Rect(0, MAP_AREA_TOP, pulse, MAP_AREA_BOTTOM - MAP_AREA_TOP,
                      CLR_DUSK_PURPLE, 1);
        LCD_Draw_Rect(240 - pulse, MAP_AREA_TOP, pulse,
                      MAP_AREA_BOTTOM - MAP_AREA_TOP, CLR_DUSK_PURPLE, 1);

        // --- 4. Big red "!" mark above the player's head ---
        // Positioned above the head, the gold outline makes it stand out on any background.
        // Mark = 12 wide, 20 tall.
        int mx = ppx + 8;    // left edge of the ! mark
        int my = ppy - 26;   // top of the ! mark

        // Golden glow/border (2px bigger than the mark in every direction)
        LCD_Draw_Rect(mx - 2, my - 2, 16, 24, CLR_HONEY_GOLD, 1);
        // Red body background
        LCD_Draw_Rect(mx,     my,     12, 20, CLR_DANGER, 1);
        // The "!" character itself in cream
        // Stem (4 wide, 12 tall, centered)
        LCD_Draw_Rect(mx + 4, my + 2, 4, 12, CLR_CREAM, 1);
        // Dot (4x4 near bottom)
        LCD_Draw_Rect(mx + 4, my + 15, 4, 4, CLR_CREAM, 1);
    }

    prev_cam_x = cam_x;
    prev_cam_y = cam_y;
    prev_px = player.x;
    prev_py = player.y;
    prev_radius = radius;
    prev_hash = current_hash;
}

static void render_torch_bar(uint32_t now_ms) {
    LCD_Draw_Rect(0, TORCH_BAR_TOP, 240, 24, CLR_HUD_BG, 1);

    LCD_printString("TORCH", 4, TORCH_BAR_TOP + 5, CLR_HUD_TEXT, 2);

    ui_progress_bar(70, TORCH_BAR_TOP + 4, 130, 16,
                    player.torch_ticks, TORCH_MAX, now_ms);

    char buf[8];
    snprintf(buf, sizeof(buf), "%d", (int)player.torch_ticks);
    uint8_t col = (player.torch_ticks <= 10 && ((now_ms / 300) & 1))
                  ? CLR_DANGER : CLR_HUD_TEXT;
    LCD_printString(buf, 206, TORCH_BAR_TOP + 5, col, 2);
}

void render_hud(uint32_t now_ms) {
    render_torch_bar(now_ms);
    ui_hud(player.torch_ticks, TORCH_MAX, ores.copper, ores.gold, ores.diamond, coins, now_ms);
}

void render_items_drawer(uint8_t selected_index, uint32_t now_ms) {
    LCD_Draw_Rect(0, 130, 240, 110, CLR_WOOD_DARK, 1);
    ui_panel_titled(10, 135, 220, 100, "ITEMS");

    static const int slot_x[3] = {20, 90, 160};
    static const uint8_t* icons[3] = { ICO_ITEM_TORCH, ICO_ITEM_DYNAMITE, ICO_ITEM_POTION };
    static const char* names[3] = {"TORCH", "BOOM", "POTION"};
    uint8_t qtys[3] = {items.torch, items.dynamite, items.potion};

    for (int i = 0; i < 3; i++) {
        int sx = slot_x[i];
        int sy = 155;

        LCD_Draw_Rect(sx, sy, 60, 65, CLR_WOOD_LIGHT, 1);
        LCD_Draw_Rect(sx, sy, 60, 65, CLR_WOOD_DARK, 0);
        
        LCD_Draw_Sprite_Scaled(sx + 16, sy + 5, 14, 14, (uint8_t*)icons[i], 2);

        LCD_printString(names[i], sx + 5, sy + 40, CLR_INK, 1);
        char buf[8];
        snprintf(buf, sizeof(buf), "x%d", qtys[i]);
        LCD_printString(buf, sx + 40, sy + 52, CLR_INK, 1);

        if (i == selected_index) {
            uint8_t pulse = ((now_ms / 400) & 1);
            ui_select_highlight(sx - 2 - pulse, sy - 2 - pulse, 64 + 2*pulse, 69 + 2*pulse);
        }
    }
    LCD_printString("[BT2]Use  [BT3]Close", 40, 225, CLR_WOOD_LIGHT, 1);
}

void render_explosion_flash(void) {
    LCD_Fill_Buffer(CLR_CREAM);
}

void render_floor_transition_dialog(uint32_t now_ms) {
    LCD_Draw_Rect(30, 40, 180, 90, CLR_HUD_BG, 1);
    LCD_Draw_Rect(30, 40, 180, 90, CLR_HUD_BORDER, 0);
    LCD_Draw_Rect(32, 42, 176, 86, CLR_HUD_BG, 1);

    LCD_printString("STAIRS DOWN", 62, 50, CLR_HUD_TEXT, 2);
    LCD_Draw_Rect(40, 70, 160, 1, CLR_HUD_BORDER, 1);

    LCD_printString("[BT2] Go deeper", 45, 80,  CLR_CREAM, 1);
    LCD_printString("[BT3] Return home", 45, 100, CLR_CREAM, 1);

    uint8_t blink = ((now_ms / 500) & 1);
    if (blink) {
        LCD_printString("Choose wisely...", 60, 117, CLR_CORAL, 1);
    }
}