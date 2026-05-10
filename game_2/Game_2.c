#include "Game_2.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "Joystick.h" 
#include "stm32l4xx_hal.h"
#include "Economy.h"
#include "fish_hardware.h"
#include "fish_shop.h"
#include "fish_render.h"
#include "fish_audio.h"
#include "fish_collection.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

// External sprites
extern const uint8_t FISH_SPR_GIRL_IDLE[14 * 14];
extern const uint8_t FISH_SPR_GIRL_WALK_A[14 * 14];
extern const uint8_t FISH_SPR_GIRL_CAST[14 * 14];

// Hardware configs
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;
extern ST7789V2_cfg_t cfg0;

// Game states
typedef enum {
    FISH_IDLE,
    FISH_HELP,
    FISH_CASTING,
    FISH_WAITING,
    FISH_BITE,        
    FISH_REELING,
    FISH_RESULT,
    FISH_SHOP,
    FISH_COLLECTION   
} FishingState;

static FishingState current_state = FISH_IDLE;
static uint32_t state_timer = 0; 
static uint32_t wait_duration = 0; 
static int catch_success = 0; 

// Fishing mechanics
static int tension = 50; 
static int fish_hp = 100;

// Player inventory
int inventory_bait = 0; 
int has_pro_rod = 0;    
int inventory_coffee = 0;      

static int using_bait = 0;     
static int coffee_buff_active = 0; 

// Current fish data
static int current_fish_type = 0; 
static int current_fish_reward = 30;
static int fish_max_hp = 100;

// Target frame time (50ms = 20 FPS)
#define GAME2_FRAME_TIME_MS 50 

MenuState Game2_Run(void) {
    current_state = FISH_IDLE;
    MenuState exit_state = MENU_STATE_HOME; 
    
    // Init hardware and custom palette
    LCD_Set_Palette(PALETTE_CUSTOM);
    Fish_Hardware_Init();
    Fish_Sensor_Init();
    Fish_Shop_Init();
    Fish_Audio_Init(); 
    
    while (1) {
        uint32_t frame_start = HAL_GetTick();
        
        Input_Read(); 
        Joystick_Read(&joystick_cfg, &joystick_data); 
        uint32_t now = frame_start;
        
        // Render background if not in UI modes
        if (current_state != FISH_SHOP && current_state != FISH_COLLECTION) {
            Fish_Render_Environment(now, inventory_bait);
            
            if (inventory_coffee > 0) {
                char cof_str[16];
                sprintf(cof_str, "COF: %d", inventory_coffee);
                LCD_printString(cof_str, 200, 3, 4, 1); 
            }
        }

        switch (current_state) {
            
            case FISH_IDLE:
                Fish_RGB_Set(0, 0, 1); 
                LCD_Draw_Sprite_Scaled(35, 25, 14, 14, (uint8_t*)FISH_SPR_GIRL_IDLE, 2);
                
                LCD_Draw_Rect(4, 72, 115, 42, 0, 1); 
                LCD_Draw_Rect(4, 72, 115, 42, 10, 0); 
                LCD_printString("BT2: SHOP", 8, 75, 1, 1);  
                LCD_printString("DOWN: BOOK", 8, 87, 1, 1); 
                LCD_printString("UP: HOW TO", 8, 99, 1, 1);
                
                // Exit game
                if (current_input.btn3_pressed) {
                    Fish_RGB_Set(0, 0, 0); 
                    exit_state = MENU_STATE_HOME;
                    goto end_game_loop; 
                }
                
                if (current_input.btn2_pressed) {
                    current_state = FISH_SHOP; 
                }
                if (joystick_data.direction == S) {
                    current_state = FISH_COLLECTION; 
                    HAL_Delay(200); 
                }
                if (joystick_data.direction == N) { 
                    current_state = FISH_HELP;
                    HAL_Delay(200); 
                }
                // Detect physical rod swing via accelerometer
                if (Fish_Detect_Rod_Swing()) {
                    current_state = FISH_CASTING;
                }
                break;

            case FISH_HELP:
                LCD_Fill_Buffer(1); 
                
                // Draw UI header
                LCD_printString("HOW TO PLAY", 60, 20, 9, 2);
                LCD_Draw_Line(10, 45, 230, 45, 9);
                
                // Instruction 1: Links to Accelerometer gesture
                LCD_printString("1. SWING the console", 10, 60, 4, 1);
                LCD_printString("   to cast your line.", 10, 75, 0, 1);
                
                // Instruction 2: Alert for fish bite event
                LCD_printString("2. WAIT for the '!'", 10, 100, 4, 1);
                LCD_printString("   and a beep sound.", 10, 115, 0, 1);
                
                // Instruction 3: Links to Pressure Sensor input
                LCD_printString("3. PRESS the sensor", 10, 140, 2, 1);
                LCD_printString("   to reel in the fish.", 10, 155, 0, 1);
                
                // Main gameplay objective: Tension bar control
                LCD_printString("KEEP TENSION IN GREEN!", 25, 190, 3, 1);
                
                LCD_printString("Press BT3 to Exit", 65, 220, 10, 1);
                
                // Transition back to idle state
                if (current_input.btn3_pressed) {
                    current_state = FISH_IDLE;
                    HAL_Delay(200); // Simple debounce
                }
                break;
                
            case FISH_CASTING:
                Fish_RGB_Set(0, 0, 1);
                
                LCD_Draw_Sprite_Scaled(35, 25, 14, 14, (uint8_t*)FISH_SPR_GIRL_CAST, 2); 
                LCD_printString("SWING ROD!", 130, 80, 2, 2); 
                
                Fish_Render_Thick_Rod(45, 42, 110, 30, has_pro_rod);
                LCD_Draw_Line(110, 30, 160, 110, 1); 
                LCD_Draw_Rect(158, 108, 4, 6, 2, 1); 

                // Wait for the swing gesture to complete
                if (Fish_Detect_Rod_Swing()) { 
                    Fish_Audio_Play_Cast(); 
                    current_state = FISH_WAITING;
                    state_timer = now;
                    
                    // Consume bait to reduce wait time and increase rarity odds
                    if (inventory_bait > 0) {
                        inventory_bait--; 
                        using_bait = 1;
                        wait_duration = 300 + (rand() % 500); 
                    } else {
                        using_bait = 0;
                        wait_duration = 1000 + (rand() % 1500);
                    }
                }
                break;
                
            case FISH_WAITING:
                Fish_RGB_Set(0, 0, 1);
                LCD_Draw_Sprite_Scaled(35, 25, 14, 14, (uint8_t*)FISH_SPR_GIRL_WALK_A, 2);
                Fish_Render_Thick_Rod(45, 42, 110, 80, has_pro_rod); 
                
                // Bobber sine wave animation
                int float_y = 190 + (int)(sin(now / 200.0) * 6.0);
                LCD_Draw_Line(110, 80, 130, float_y, 1); 
                LCD_Draw_Circle(130, float_y, 4, 2, 1); 
                LCD_printString(" WAITING... ", 80, 10, 1, 2);
                
                // Fish bites
                if (now - state_timer > wait_duration) {
                    Fish_Audio_Play_Bite(); 
                    state_timer = now;
                    current_state = FISH_BITE; 
                    
                    // Roll for fish rarity and set HP
                    int roll = (rand() % 100) + (using_bait ? 30 : 0);
                    if (roll < 60) { 
                        current_fish_type = 0; fish_max_hp = 300; current_fish_reward = 30; 
                    } else if (roll < 90) { 
                        current_fish_type = 1; fish_max_hp = 720; current_fish_reward = 60; 
                    } else { 
                        current_fish_type = 2; fish_max_hp = 1500; current_fish_reward = 150; 
                    } 
                    fish_hp = fish_max_hp; 
                }
                break;
                
            case FISH_BITE: 
                Fish_RGB_Set(1, 0, 0); // Red warning

                LCD_Draw_Sprite_Scaled(35, 25, 14, 14, (uint8_t*)FISH_SPR_GIRL_WALK_A, 2);
                Fish_Render_Thick_Rod(45, 42, 110, 80, has_pro_rod);
                
                LCD_Draw_Rect(44, 2, 8, 14, 2, 1); 
                LCD_Draw_Rect(44, 18, 8, 6, 2, 1); 
                
                // 1.2s reaction window before reeling starts
                if (now - state_timer > 1200) {
                    tension = 50; 
                    
                    if (inventory_coffee > 0) {
                        inventory_coffee--;
                        coffee_buff_active = 1;
                    } else {
                        coffee_buff_active = 0;
                    }
                    
                    current_state = FISH_REELING;
                }
                break;
                
            case FISH_REELING:
            {
                LCD_Draw_Sprite_Scaled(33, 25, 14, 14, (uint8_t*)FISH_SPR_GIRL_CAST, 2); 
                Fish_Render_Thick_Rod(43, 42, 90, 100, has_pro_rod); 
                LCD_Draw_Line(90, 100, 140, 200, 1); 
                LCD_printString("PRESS SENSOR!", 25, 115, 2, 2);
                
                if (coffee_buff_active) {
                    LCD_printString("COFFEE RUSH!", 140, 130, 6, 1); 
                }
                
                int raw_adc = (int)Fish_Read_Pressure(); 
                
                // Apply deadzone to ignore baseline sensor noise (<550)
                if (raw_adc < 550) {
                    raw_adc = 0;
                } else {
                    raw_adc = raw_adc - 550; 
                }

                // Low-pass filter to smooth out finger jitter
                static int smooth_pressure = 0;
                smooth_pressure = (smooth_pressure * 3 + raw_adc) / 4;
                
                // Map to 0-100 tension (2500 is approx max effective pressure)
                int base_tension = (smooth_pressure * 100) / 2500; 
                if (base_tension > 100) {
                    base_tension = 100;
                }
                
                // Fish struggle logic: Random tension offset based on rarity
                static uint32_t drift_timer = 0;
                static int fish_drift = 0; 
                
                if (now - drift_timer > 400 + (rand() % 400)) {
                    drift_timer = now;
                    int struggle = (current_fish_type == 2) ? 60 : ((current_fish_type == 1) ? 40 : 24);
                    fish_drift = (rand() % struggle) - (struggle / 2);
                }
                
                tension = base_tension + fish_drift;
                if (tension < 0) tension = 0;
                if (tension > 100) tension = 100;
                
                // Debug info on screen
                char debug_str[32];
                sprintf(debug_str, "ADC:%4d T:%3d", raw_adc, tension);
                LCD_printString(debug_str, 5, 220, 1, 1);

                Fish_Audio_Play_Reeling(tension, now); 
                
                // Damage calculation (Safe zone: 30-70)
                if (tension > 70 || tension < 30) {
                    // Flash red, fish heals
                    if ((now / 100) % 2 == 0) {
                        Fish_RGB_Set(1, 0, 0); 
                    } else {
                        Fish_RGB_Set(0, 0, 0);
                    }
                    
                    fish_hp += (current_fish_type + 1) * 4; 
                    if (fish_hp > fish_max_hp) {
                        fish_hp = fish_max_hp;
                    }
                } else {
                    // Green, deal damage
                    Fish_RGB_Set(0, 1, 0); 
                    
                    int damage = 3; 
                    if (has_pro_rod) damage += 4; 
                    if (coffee_buff_active) damage += 3; 
                    
                    fish_hp -= damage; 
                }
                
                Fish_Render_Fishing_Bar(200, 60, 160, tension);
                Fish_Render_Progress_Bar(20, 150, 150, 16, fish_hp, fish_max_hp, 10, "Fish Energy:");
                
                // Check win/loss
                if (fish_hp <= 0) { 
                    catch_success = 1; 
                    Add_Gold(current_fish_reward); 
                    current_state = FISH_RESULT; 
                    state_timer = 0; 
                } else if (tension >= 100 || tension <= 0) { 
                    catch_success = 0; 
                    current_state = FISH_RESULT; 
                    state_timer = 0;
                }
                break;
            }
                
            case FISH_RESULT:
                if (catch_success) {
                    if (state_timer == 0) { 
                        Fish_Audio_Play_Success(); 
                        Fish_Collection_Add(current_fish_type); 
                        state_timer = 1; 
                    }
                    Fish_RGB_Set(0, 1, 0); 
                    LCD_printString(" GOT ONE! ", 60, 60, 0, 2);
                    Fish_Render_Big_Fish(100, 100, current_fish_type, now);
                    
                    char reward_str[16];
                    sprintf(reward_str, "+ %d GOLD", current_fish_reward);
                    LCD_printString(reward_str, 80, 145, 10, 1);
                } else {
                    if (state_timer == 0) {
                        Fish_Audio_Play_Fail(); 
                        state_timer = 1;
                    }
                    Fish_RGB_Set(1, 0, 0); 
                    LCD_printString(" ESCAPED ", 60, 100, 1, 2);
                }
                
                LCD_printString(" Press BT2 ", 75, 185, 0, 1); 
                
                if (current_input.btn2_pressed) {
                    current_state = FISH_IDLE;
                }
                break;

            case FISH_SHOP:
                Fish_Shop_Draw_UI(now);
                if (Fish_Shop_Handle_Input(current_input.btn2_pressed, current_input.btn3_pressed, joystick_data.direction, now)) {
                    current_state = FISH_IDLE;
                }
                break;
                
            case FISH_COLLECTION: 
                Fish_Collection_Draw_UI(now);
                if (current_input.btn3_pressed) {
                    current_state = FISH_IDLE;
                    HAL_Delay(100); 
                }
                break;
        }

        LCD_Refresh(&cfg0); 
        
        // Keep stable frame rate
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < GAME2_FRAME_TIME_MS) {
            HAL_Delay(GAME2_FRAME_TIME_MS - frame_time);
        }
    }

end_game_loop:
    LCD_Set_Palette(PALETTE_DEFAULT);
    return exit_state;
}