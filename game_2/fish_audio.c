#include "fish_audio.h"
#include "stm32l4xx_hal.h"

extern TIM_HandleTypeDef htim2; 

// --- Core Buzzer Hardware Control ---

static void beep(uint16_t freq, uint16_t duration) {
    // Protect against invalid frequencies and division by zero
    if (freq < 20 || freq > 20000) return; 

    // Timer clock frequency (1MHz based on current prescaler config)
    uint32_t timer_clock = 1000000; 
    uint32_t arr_value = (timer_clock / freq) - 1;

    // Update PWM frequency
    __HAL_TIM_SET_AUTORELOAD(&htim2, arr_value);
    
    // Set 50% duty cycle for maximum volume
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, arr_value / 2); 

    // Start the PWM output strictly only when we need to make a sound
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

    HAL_Delay(duration);

    // Mute buzzer after the duration ends
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);

    // Completely stop the hardware PWM output to eliminate electrical noise
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
}

// --- Audio System API ---

void Fish_Audio_Init(void) {
    // Ensure the timer channel is completely stopped at initialization
    // This prevents residual PWM noise from bleeding into the main menu
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3); 
}

void Fish_Audio_Play_Cast(void) {
    // Pitch slide down effect
    for(int i = 1000; i > 200; i -= 100) {
        beep(i, 10);
    }
}

void Fish_Audio_Play_Bite(void) {
    // Double high-pitch beep alert
    beep(2500, 100);
    HAL_Delay(50);   
    beep(2500, 250); 
}

void Fish_Audio_Play_Reeling(int tension, uint32_t tick) {
    // Periodic sound update
    if ((tick % 200) < 50) {
        if (tension > 80 || tension < 20) {
            beep(150, 50); // Low frequency warning
        } else {
            beep(400 + tension * 10, 20); // Dynamic pitch tracking the tension
        }
    }
}

void Fish_Audio_Play_Success(void) {
    // Simple victory arpeggio (C-E-G-C)
    beep(523, 100); 
    beep(659, 100); 
    beep(784, 150); 
    beep(1046, 200); 
}

void Fish_Audio_Play_Fail(void) {
    // Sad low pitch effect
    beep(300, 200);
    beep(250, 300);
}