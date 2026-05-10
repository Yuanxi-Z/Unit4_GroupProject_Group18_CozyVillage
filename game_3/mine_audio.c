#include "mine_audio.h"
#include "Buzzer.h"
#include "PWM.h"
#include "stm32l4xx_hal.h"

extern Buzzer_cfg_t buzzer_cfg;
extern PWM_cfg_t pwm_cfg;

#define TORCH_MAX 60

static uint32_t buzzer_off_at;
static uint32_t last_ping_at;

// Diamond melody state
static uint8_t  melody_step;
static uint32_t melody_next_at;
static const uint16_t melody_freq[3] = {523, 659, 784}; // C5 E5 G5

void audio_init(void) {
    buzzer_off_at = 0;
    last_ping_at = 0;
    melody_step = 0;
}

void audio_update(uint32_t now) {
    // Diamond melody sequencer
    if (melody_step > 0 && now >= melody_next_at) {
        if (melody_step <= 3) {
            buzzer_tone(&buzzer_cfg, melody_freq[melody_step - 1], 25);
            melody_next_at = now + 100;
            melody_step++;
        } else {
            buzzer_off(&buzzer_cfg);
            melody_step = 0;
        }
        return;
    }

    // Auto buzzer off
    if (buzzer_off_at && now >= buzzer_off_at) {
        buzzer_off(&buzzer_cfg);
        buzzer_off_at = 0;
    }
}

void audio_ping(int distance) {
    // Buzzer only sounds when very close to a geode (<= 2 tiles).
    // Audible only when adjacent to the tile.
    if (distance > 2 || distance <= 0) return;
    uint32_t now = HAL_GetTick();
    // distance=1 -> 300ms interval, 2 -> 450ms interval (closer = faster ping)
    uint32_t interval = 150 + (distance * 150);
    if (now - last_ping_at >= interval) {
        // distance=1 -> 900Hz, 2 -> 800Hz (closer = higher pitch)
        uint32_t freq = 700 + (3 - distance) * 100;
        buzzer_tone(&buzzer_cfg, freq, 20);
        buzzer_off_at = now + 40;
        last_ping_at = now;
    }
}

void audio_pickup_copper(void) {
    buzzer_tone(&buzzer_cfg, 1400, 25);
    buzzer_off_at = HAL_GetTick() + 60;
}

void audio_pickup_gold(void) {
    buzzer_tone(&buzzer_cfg, 1600, 25);
    buzzer_off_at = HAL_GetTick() + 60;
}

void audio_pickup_diamond(void) {
    melody_step = 1;
    melody_next_at = HAL_GetTick();
}

void audio_mine_wall(void) {
    buzzer_tone(&buzzer_cfg, 300, 30);
    buzzer_off_at = HAL_GetTick() + 80;
}

void audio_explode(void) {
    buzzer_tone(&buzzer_cfg, 150, 60);
    buzzer_off_at = HAL_GetTick() + 250;
}

void audio_faint(void) {
    buzzer_tone(&buzzer_cfg, 150, 50);
    buzzer_off_at = HAL_GetTick() + 500;
}

void audio_buy(void) {
    buzzer_tone(&buzzer_cfg, 1200, 15);
    buzzer_off_at = HAL_GetTick() + 50;
}

void audio_torch_led(int16_t ticks) {
    if (ticks > 15) {
        uint8_t duty = (uint8_t)((ticks * 100) / TORCH_MAX);
        if (duty > 100) duty = 100;
        PWM_SetDuty(&pwm_cfg, duty);
    } else if (ticks > 0) {
        uint8_t base = (uint8_t)((ticks * 100) / TORCH_MAX);
        uint8_t flash = ((HAL_GetTick() / 300) % 2) ? base : 0;
        PWM_SetDuty(&pwm_cfg, flash);
    } else {
        PWM_SetDuty(&pwm_cfg, 0);
    }
}