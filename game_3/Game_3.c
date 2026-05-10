// Game_3.c — Cozy Torchlight Miner
#include "Game_3.h"
#include "mine_map.h"
#include "mine_player.h"
#include "mine_inventory.h"
#include "mine_audio.h"
#include "mine_render.h"
#include "mine_shop.h" 
#include "mine_home.h" 
#include "mine_bat.h" 
#include "mine_palette.h"
#include "mine_serial.h"   // UART terminal logger + ASCII map

#include "InputHandler.h"
#include "LCD.h"
#include "Joystick.h"
#include "PWM.h"
#include "stm32l4xx_hal.h"
#include "Economy.h"
#include <stdio.h>

extern uint16_t coins;
extern TIM_HandleTypeDef htim7;
extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern volatile uint32_t g_tim7_ticks;

uint16_t g_faint_ores_value_lost = 0;  
uint16_t g_faint_coins_saved = 0;      
uint32_t g_faint_start_tick = 0;       

typedef enum {
    ST_EXPLORING,
    ST_ITEMS,
    ST_TRADING,
    ST_FLOOR_TRANSITION,  
    ST_FAINTED,
} MineState;

#define FRAME_MS 33

MenuState Game3_Run(void) {
    LCD_Set_Palette(PALETTE_CUSTOM); 

    map_generate(FLOOR_ENTRY);  
    player_init();
    inv_reset();
    coins = Get_Total_Gold();
    audio_init();
    bat_init_for_floor(FLOOR_ENTRY);   

    serial_init();
    serial_log_event("START", "entering mine, floor=1");

    MineState state = ST_EXPLORING;
    uint8_t item_selected = 0;

    g_tim7_ticks = 0;
    uint32_t last_tim7 = 0;
    HAL_TIM_Base_Start_IT(&htim7);
    PWM_SetDuty(&pwm_cfg, 100);

    MenuState exit_state = MENU_STATE_HOME;

    Input_Read();
    HAL_Delay(100);
    Input_Read();

    while (1) {
        uint32_t now = HAL_GetTick();
        uint32_t frame_start = now;

        uint32_t t7 = g_tim7_ticks;
        if (t7 != last_tim7) {
            int elapsed = (int)(t7 - last_tim7);
            last_tim7 = t7;
            if (state == ST_EXPLORING) {
                // Torch depletes even when poisoned
                player.torch_ticks -= elapsed;
                if (player.torch_ticks < 0) player.torch_ticks = 0;
            }
        }

        if (player.torch_ticks == 0 && state == ST_EXPLORING) {
            uint16_t pre_value = ores.copper * 10 + ores.gold * 30 + ores.diamond * 80;
            ore_lose_half();
            uint16_t after_value = ores.copper * 10 + ores.gold * 30 + ores.diamond * 80;
            
            g_faint_ores_value_lost = pre_value - after_value;
            g_faint_coins_saved = ore_sell_all();  
            g_faint_start_tick = HAL_GetTick();

            char fdetail[64];
            snprintf(fdetail, sizeof(fdetail),
                     "torch=0, lost=%dc, saved=%dc",
                     g_faint_ores_value_lost, g_faint_coins_saved);
            serial_log_warn("FAINT", fdetail);

            state = ST_FAINTED;
            audio_faint();
            PWM_SetDuty(&pwm_cfg, 0);
        }

        if (state != ST_FAINTED) {
            audio_torch_led(player.torch_ticks);
        }

        Input_Read();
        Joystick_Read(&joystick_cfg, &joystick_data);
        now = HAL_GetTick();

        uint8_t bt2 = current_input.btn2_pressed;
        uint8_t bt3 = current_input.btn3_pressed;

        if (state == ST_EXPLORING) {
            
            // Player can only move if not poisoned
            if (!player.is_poisoned) {
                uint8_t move_result = player_move(joystick_data.direction, now);
                char detail[48];
                if (move_result == 2) {
                    audio_pickup_copper();
                    snprintf(detail, sizeof(detail), "copper at (%d,%d) -> Cu=%d",
                             player.x, player.y, ores.copper);
                    serial_log_event("PICKUP", detail);
                }
                if (move_result == 3) {
                    audio_pickup_gold();
                    snprintf(detail, sizeof(detail), "gold at (%d,%d) -> Au=%d",
                             player.x, player.y, ores.gold);
                    serial_log_event("PICKUP", detail);
                }
                if (move_result == 4) {
                    audio_pickup_diamond();
                    snprintf(detail, sizeof(detail), "DIAMOND at (%d,%d) -> Di=%d",
                             player.x, player.y, ores.diamond);
                    serial_log_ok("PICKUP", detail);
                }
                
                // Moving into a bat causes poisoning
                if (move_result == 5) {
                    player.is_poisoned = 1;
                    audio_explode();
                    char bdetail[48];
                    snprintf(bdetail, sizeof(bdetail),
                             "stepped onto bat at (%d,%d)",
                             player.x, player.y);
                    serial_log_warn("POISON", bdetail);
                }

                if (map_get(player.x, player.y) == TILE_EXIT) {
                    if (player.depth == FLOOR_DEEP) {
                        state = ST_TRADING;
                        shop_init();
                    } else {
                        state = ST_FLOOR_TRANSITION;
                    }
                }
            }

            // Player can only mine if not poisoned
            if (bt2 && !player.is_poisoned) {
                uint8_t result = player_mine(now);
                switch (result) {
                    case 0: audio_pickup_copper();
                            serial_log_event("MINE", "copper vein cracked");
                            break;
                    case 1: audio_pickup_gold();
                            serial_log_event("MINE", "gold vein cracked");
                            break;
                    case 2: audio_pickup_diamond();
                            serial_log_ok("MINE", "diamond extracted!");
                            break;
                    case 3: audio_mine_wall();
                            serial_log_event("MINE", "wall broken, torch -5");
                            break;
                }
            }

            // Inventory remains accessible to use potions when poisoned
            if (bt3) {
                state = ST_ITEMS;
                item_selected = 0;
            }

            static uint32_t last_detect = 0;
            if (now - last_detect > 200) {
                last_detect = now;
                int dist = map_nearest_geode_dist(player.x, player.y);
                audio_ping(dist);
            }

            // Periodic terminal frame render - once every 2 seconds.
            // Keeps UART from flooding while still animating in PuTTY/Tera Term.
            static uint32_t last_serial_frame = 0;
            if (now - last_serial_frame > 2000) {
                last_serial_frame = now;
                serial_render_frame();
            }

            // Bat attacking the player causes poisoning
            uint8_t bat_bit = bat_update(now, player.x, player.y);
            if (bat_bit && !player.is_poisoned) {
                player.is_poisoned = 1;
                audio_explode();
                serial_log_warn("POISON", "bat bit the player!");
            }
        }
        else if (state == ST_ITEMS) {
            static uint32_t last_nav = 0;
            if (now - last_nav > 200) {
                Direction dir = joystick_data.direction;
                if ((dir == N) && item_selected > 0) { item_selected--; last_nav = now; }
                if ((dir == S) && item_selected < 2) { item_selected++; last_nav = now; }
            }

            if (bt2) {
                if (item_selected == 0 && items.torch > 0) {
                    items.torch--;
                    player.torch_ticks += TORCH_MAX;
                    if (player.torch_ticks > TORCH_MAX) player.torch_ticks = TORCH_MAX;
                    audio_buy();
                    state = ST_EXPLORING;
                }
                else if (item_selected == 1 && items.dynamite > 0) {
                    items.dynamite--;
                    uint8_t dia = 0;
                    map_explode(player.x, player.y, &dia);
                    render_explosion_flash();
                    audio_explode();
                    state = ST_EXPLORING;
                }
                else if (item_selected == 2 && items.potion > 0) { 
                    // Use potion to cure poison
                    if (item_use_potion()) {
                        player.is_poisoned = 0; // Cure status
                        audio_buy(); // Recovery sound
                        char pdetail[32];
                        snprintf(pdetail, sizeof(pdetail),
                                 "cured, potions left=%d", items.potion);
                        serial_log_ok("POTION", pdetail);
                    }
                    state = ST_EXPLORING;
                }
            }
            if (bt3) state = ST_EXPLORING;
        }
        else if (state == ST_FLOOR_TRANSITION) {
            if (bt2) {
                map_generate(FLOOR_DEEP);
                player_init_on_floor(FLOOR_DEEP);
                bat_init_for_floor(FLOOR_DEEP);   
                audio_buy();  
                serial_log_event("DESCEND", "entering Floor 2 (The Deep)");
                state = ST_EXPLORING;
            }
            if (bt3) {
                state = ST_TRADING;
                shop_init();
            }
        }
        else if (state == ST_TRADING) {
            ShopResult r = shop_update(bt2, bt3, joystick_data.direction, now);
            if (r == SHOP_RESULT_LEAVE) {
                serial_log_event("EXIT", "leaving shop -> returning home");
                exit_state = MENU_STATE_HOME;
                break;
            }
        }
        else if (state == ST_FAINTED) {
            uint32_t since_faint = now - g_faint_start_tick;
            if (bt3) {
                if (since_faint < 2500) {
                    g_faint_start_tick = now - 2500;
                } else {
                    exit_state = MENU_STATE_HOME;
                    break;
                }
            }
        }

        audio_update(now);

        static MineState last_state = (MineState)-1;
        if (state != last_state) {
            LCD_Fill_Buffer(0);
            last_state = state;
        }

        switch (state) {
            case ST_EXPLORING:
                render_map(now);
                render_hud(now);
                break;
            case ST_ITEMS:
                render_map(now);
                render_items_drawer(item_selected, now);
                break;
            case ST_FLOOR_TRANSITION:     
                render_map(now);
                render_floor_transition_dialog(now);
                break;
            case ST_TRADING:
                shop_render(now);
                break;
            case ST_FAINTED:
                home_render(now);
                break;
        }

        LCD_Refresh(&cfg0);

        uint32_t elapsed = HAL_GetTick() - frame_start;
        if (elapsed < FRAME_MS) HAL_Delay(FRAME_MS - elapsed);
    }

    LCD_Set_Palette(PALETTE_DEFAULT);
    HAL_TIM_Base_Stop_IT(&htim7);
    PWM_SetDuty(&pwm_cfg, 50);
    return exit_state;
}