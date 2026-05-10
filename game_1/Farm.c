#include "Farm.h"
#include "FarmHome.h"
#include "FarmShop.h"
#include "FarmState.h"
#include "FarmConfig.h"
#include "FarmRender.h"
#include "FarmEffects.h"
#include "farm_palette.h"
#include "InputHandler.h"
#include "LCD.h"
#include "Joystick.h"
#include "FarmSprites.h"
#include "Buzzer.h"
#include "Economy.h"
#include "adc.h"
#include "ds18b20.h"
#include "sevenseg.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include <string.h>
#include <stdio.h>

/*
 * External hardware and driver objects generated or configured elsewhere.
 * These objects are shared with the farm module for LCD, joystick, buzzer,
 * ADC, timer, and peripheral control.
 */
extern ST7789V2_cfg_t   cfg0;
extern Joystick_cfg_t   joystick_cfg;
extern Joystick_t       joystick_data;
extern Buzzer_cfg_t     buzzer_cfg;
extern ADC_HandleTypeDef hadc1;

/*
 * TIM6 software tick counter.
 * It is used as the crop growth timing base so that crop updates are
 * independent from the main rendering frame rate.
 */
extern volatile uint32_t g_tim6_ticks;

/*
 * Main farm game state.
 * All gameplay values, scene states, crop states, sensor values, and animation
 * states are stored inside this structure.
 */
static FarmState farm_state;

/*
 * Return time used by the home sleeping animation.
 * When the sleep sequence finishes, the scene automatically returns to the field.
 */
static uint32_t farm_sleep_return_time = 0;


/* -------------------------------------------------------------------------- */
/* Shared economy bridge                                                       */
/* -------------------------------------------------------------------------- */

static uint16_t Farm_GetSharedGoldForDisplay(void)
{
    int gold = Get_Total_Gold();

    if (gold < 0) {
        return 0U;
    }

    if (gold > 65535) {
        return 65535U;
    }

    return (uint16_t)gold;
}

static void Farm_SyncCoinsFromSharedEconomy(void)
{
    farm_state.game.coins = Farm_GetSharedGoldForDisplay();
}

static void Farm_CommitCoinDeltaToSharedEconomy(uint16_t before, uint16_t after)
{
    if (after > before) {
        Add_Gold((int)(after - before));
    } else if (before > after) {
        Spend_Gold((int)(before - after));
    }

    Farm_SyncCoinsFromSharedEconomy();
}

/* -------------------------------------------------------------------------- */
/* Buzzer sequence state                                                       */
/* -------------------------------------------------------------------------- */

/*
 * These variables implement a simple non-blocking melody player.
 * The main loop updates the current note according to HAL_GetTick().
 */
static const uint16_t *farm_buzzer_seq_freqs = NULL;
static const uint16_t *farm_buzzer_seq_durs  = NULL;
static uint8_t farm_buzzer_seq_len = 0;
static uint8_t farm_buzzer_seq_idx = 0;
static uint8_t farm_buzzer_seq_active = 0;
static uint8_t farm_buzzer_seq_volume = 0;
static uint8_t farm_buzzer_seq_loop = 0;

/* -------------------------------------------------------------------------- */
/* Short feedback sound presets                                                */
/* -------------------------------------------------------------------------- */

/*
 * Single-tone feedback sounds.
 * These are used for quick player actions such as planting, watering,
 * clearing dead crops, and changing mode.
 */
#define FARM_SOUND_PLANT_FREQ         960
#define FARM_SOUND_PLANT_VOL          1
#define FARM_SOUND_PLANT_MS           35

#define FARM_SOUND_WATER_FREQ         840
#define FARM_SOUND_WATER_VOL          1
#define FARM_SOUND_WATER_MS           35

#define FARM_SOUND_CLEAR_FREQ         720
#define FARM_SOUND_CLEAR_VOL          1
#define FARM_SOUND_CLEAR_MS           35

#define FARM_SOUND_MODE_FREQ          900
#define FARM_SOUND_MODE_VOL           1
#define FARM_SOUND_MODE_MS            25

/*
 * Failure sound sequence.
 * Used when the player tries an invalid action, such as planting without seeds
 * or acting with no energy.
 */
static const uint16_t FARM_SOUND_FAIL_FREQS[]  = {880, 620, 420};
static const uint16_t FARM_SOUND_FAIL_DURS[]   = {50, 60, 110};

/*
 * Crop death sound sequence.
 * Used when a crop dies because of overwatering or drought.
 */
static const uint16_t FARM_SOUND_DEATH_FREQS[] = {700, 500, 320};
static const uint16_t FARM_SOUND_DEATH_DURS[]  = {70, 80, 120};

/*
 * Night transition sound.
 * A short gentle falling sound is played when night begins.
 */
static const uint16_t FARM_SOUND_NIGHT_FREQS[] = {659, 523, 440};
static const uint16_t FARM_SOUND_NIGHT_DURS[]  = {160, 220, 320};

#define FARM_SOUND_NIGHT_LEN \
    ((uint8_t)(sizeof(FARM_SOUND_NIGHT_FREQS) / sizeof(FARM_SOUND_NIGHT_FREQS[0])))

/*
 * Overwatering warning sound.
 * This is separate from the crop death sound to make the cause clearer.
 */
static const uint16_t FARM_SOUND_OVERWATER_FREQS[] = {740, 0, 520};
static const uint16_t FARM_SOUND_OVERWATER_DURS[]  = {90, 60, 220};

#define FARM_SOUND_OVERWATER_LEN \
    ((uint8_t)(sizeof(FARM_SOUND_OVERWATER_FREQS) / sizeof(FARM_SOUND_OVERWATER_FREQS[0])))

/*
 * Harvest sound sequence.
 * This rising melody gives positive feedback when a mature crop is harvested.
 */
static const uint16_t FARM_SOUND_HARVEST_FREQS[] = {988, 1175, 1319};
static const uint16_t FARM_SOUND_HARVEST_DURS[]  = {70, 70, 120};

/*
 * Sleep transition melody.
 * The longer note durations create a calm night-time feeling before the next day.
 * A frequency value of 0 represents silence.
 */
static const uint16_t FARM_SOUND_SLEEP_FREQS[] = {
    659, 0,   587, 0,   523, 0,   440, 0,
    392, 440, 523, 0
};

static const uint16_t FARM_SOUND_SLEEP_DURS[] = {
    240, 80,  280, 80,  340, 100, 420, 140,
    520, 420, 620, 220
};

/*
 * Number of notes in the sleep melody.
 */
#define FARM_SOUND_SLEEP_LEN \
    ((uint8_t)(sizeof(FARM_SOUND_SLEEP_FREQS) / sizeof(FARM_SOUND_SLEEP_FREQS[0])))

/* -------------------------------------------------------------------------- */
/* Intro background music                                                      */
/* -------------------------------------------------------------------------- */

/*
 * Low-volume looping intro melody.
 * It is used on the rules / introduction page to create a warm farm atmosphere.
 *
 * A frequency value of 0 represents a rest.
 * The very low volume keeps the music gentle and avoids making the buzzer harsh.
 */
#define FARM_SOUND_INTRO_VOL  1

static const uint16_t FARM_SOUND_INTRO_FREQS[] = {
    523, 659, 784, 0,   784, 880, 784, 659,
    587, 0,   659, 587, 523, 0,   0,

    523, 587, 659, 0,   659, 784, 659, 587,
    523, 0,   392, 440, 523, 0,   0,

    523, 659, 784, 0,   880, 784, 659, 587,
    659, 0,   587, 523, 440, 0,   0,

    392, 440, 523, 0,   587, 659, 587, 523,
    440, 0,   392, 440, 523, 0,   0
};

static const uint16_t FARM_SOUND_INTRO_DURS[] = {
    220, 220, 340, 70,  180, 180, 280, 260,
    340, 80,  240, 240, 420, 100, 180,

    220, 220, 340, 70,  220, 300, 240, 260,
    380, 90,  220, 220, 440, 120, 180,

    220, 220, 320, 70,  240, 260, 240, 260,
    360, 90,  240, 240, 420, 120, 180,

    220, 220, 340, 70,  220, 260, 240, 260,
    360, 90,  220, 220, 520, 140, 220
};

/*
 * Number of notes in the intro melody.
 */
#define FARM_SOUND_INTRO_LEN \
    ((uint8_t)(sizeof(FARM_SOUND_INTRO_FREQS) / sizeof(FARM_SOUND_INTRO_FREQS[0])))

/*
 * Play one note from the current buzzer sequence.
 * If the frequency is 0, the note is treated as a rest and the buzzer is turned off.
 */
static void Farm_PlaySequenceNote(uint8_t idx)
{
    if (farm_buzzer_seq_freqs == NULL ||
        farm_buzzer_seq_durs == NULL ||
        idx >= farm_buzzer_seq_len) {
        return;
    }

    if (farm_buzzer_seq_freqs[idx] == 0U) {
        buzzer_off(&buzzer_cfg);
    } else {
        buzzer_tone(&buzzer_cfg,
                    farm_buzzer_seq_freqs[idx],
                    farm_buzzer_seq_volume);
    }
}

/*
 * Stop all active sound output.
 * This function clears both single-tone feedback and multi-note sequences.
 */
static void Farm_StopAllSound(void)
{
    buzzer_off(&buzzer_cfg);

    farm_state.sound_playing = 0;
    farm_state.sound_off_time = 0;

    farm_buzzer_seq_active = 0;
    farm_buzzer_seq_idx = 0;
    farm_buzzer_seq_len = 0;
    farm_buzzer_seq_freqs = NULL;
    farm_buzzer_seq_durs = NULL;
    farm_buzzer_seq_volume = 0;
    farm_buzzer_seq_loop = 0;
}

/*
 * Play a short single feedback tone.
 * This is used for immediate action feedback where a full melody is unnecessary.
 */
static void Farm_PlayFeedbackTone(uint16_t freq, uint8_t volume, uint16_t duration_ms)
{
    /*
     * Stop any active sequence first so that short action feedback does not
     * overlap with another buzzer pattern.
     */
    farm_buzzer_seq_active = 0;
    farm_buzzer_seq_idx = 0;
    farm_buzzer_seq_len = 0;
    farm_buzzer_seq_freqs = NULL;
    farm_buzzer_seq_durs = NULL;
    farm_buzzer_seq_volume = 0;
    farm_buzzer_seq_loop = 0;

    buzzer_tone(&buzzer_cfg, freq, volume);

    farm_state.sound_off_time = HAL_GetTick() + duration_ms;
    farm_state.sound_playing = 1;
}

/*
 * Start a non-looping tone sequence.
 * The sequence is updated later by Farm_UpdateToneSequence().
 */
static void Farm_StartToneSequence(const uint16_t *freqs,
                                   const uint16_t *durs,
                                   uint8_t length,
                                   uint8_t volume)
{
    if (freqs == NULL || durs == NULL || length == 0) {
        return;
    }

    farm_buzzer_seq_freqs = freqs;
    farm_buzzer_seq_durs = durs;
    farm_buzzer_seq_len = length;
    farm_buzzer_seq_idx = 0;
    farm_buzzer_seq_active = 1;
    farm_buzzer_seq_volume = volume;
    farm_buzzer_seq_loop = 0;

    Farm_PlaySequenceNote(0);

    farm_state.sound_off_time = HAL_GetTick() + farm_buzzer_seq_durs[0];
    farm_state.sound_playing = 1;
}

/*
 * Start a looping tone sequence.
 * This is mainly used for intro background music.
 */
static void Farm_StartLoopingToneSequence(const uint16_t *freqs,
                                          const uint16_t *durs,
                                          uint8_t length,
                                          uint8_t volume)
{
    if (freqs == NULL || durs == NULL || length == 0) {
        return;
    }

    farm_buzzer_seq_freqs = freqs;
    farm_buzzer_seq_durs = durs;
    farm_buzzer_seq_len = length;
    farm_buzzer_seq_idx = 0;
    farm_buzzer_seq_active = 1;
    farm_buzzer_seq_volume = volume;
    farm_buzzer_seq_loop = 1;

    Farm_PlaySequenceNote(0);

    farm_state.sound_off_time = HAL_GetTick() + farm_buzzer_seq_durs[0];
    farm_state.sound_playing = 1;
}

/*
 * Update the currently active tone sequence.
 * This function is non-blocking and should be called repeatedly from Farm_Update().
 */
static void Farm_UpdateToneSequence(uint32_t now)
{
    if (!farm_buzzer_seq_active) {
        return;
    }

    /*
     * Keep playing the current note until its duration has elapsed.
     */
    if (now < farm_state.sound_off_time) {
        return;
    }

    farm_buzzer_seq_idx++;

    /*
     * If the sequence has ended, either restart it or stop all sequence state.
     */
    if (farm_buzzer_seq_idx >= farm_buzzer_seq_len) {
        if (farm_buzzer_seq_loop) {
            farm_buzzer_seq_idx = 0;
            Farm_PlaySequenceNote(farm_buzzer_seq_idx);
            farm_state.sound_off_time = now + farm_buzzer_seq_durs[farm_buzzer_seq_idx];
            farm_state.sound_playing = 1;
            return;
        }

        buzzer_off(&buzzer_cfg);

        farm_state.sound_playing = 0;
        farm_buzzer_seq_active = 0;
        farm_buzzer_seq_idx = 0;
        farm_buzzer_seq_len = 0;
        farm_buzzer_seq_freqs = NULL;
        farm_buzzer_seq_durs = NULL;
        farm_buzzer_seq_volume = 0;
        farm_buzzer_seq_loop = 0;
        return;
    }

    /*
     * Play the next note and schedule the next update time.
     */
    Farm_PlaySequenceNote(farm_buzzer_seq_idx);
    farm_state.sound_off_time = now + farm_buzzer_seq_durs[farm_buzzer_seq_idx];
    farm_state.sound_playing = 1;
}

/*
 * Start the intro music only when no other sequence is active.
 * This prevents restarting the melody every frame.
 */
static void Farm_StartIntroMusicIfNeeded(void)
{
    if (!farm_buzzer_seq_active) {
        Farm_StartLoopingToneSequence(FARM_SOUND_INTRO_FREQS,
                                      FARM_SOUND_INTRO_DURS,
                                      FARM_SOUND_INTRO_LEN,
                                      FARM_SOUND_INTRO_VOL);
    }
}

/*
 * Set the RGB LED colour using three logical colour channels.
 * The pin mapping follows the actual board wiring used in this project.
 */
static void Farm_RGB_Set(uint8_t r, uint8_t g, uint8_t b)
{
    HAL_GPIO_WritePin(RGB_R_GPIO_Port, RGB_R_Pin, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_G_GPIO_Port, RGB_G_Pin, g ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_B_GPIO_Port, RGB_B_Pin, r ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/*
 * Update the RGB LED according to the current temperature state.
 * Red indicates hot, blue indicates cold, and green indicates normal temperature.
 */
static void Farm_RGB_UpdateByTemperature(void)
{
    if (farm_state.temp_state == TEMP_STATE_HOT) {
        Farm_RGB_Set(1, 0, 0);
    } else if (farm_state.temp_state == TEMP_STATE_COLD) {
        Farm_RGB_Set(0, 0, 1);
    } else {
        Farm_RGB_Set(0, 1, 0);
    }
}

/*
 * Reset one crop tile to an empty state.
 * This is used after harvesting or clearing a dead crop.
 */
static void Farm_ClearTile(CropTile *tile)
{
    tile->state        = TILE_EMPTY;
    tile->water_level  = 0;
    tile->growth_ticks = 0;
    tile->dry_ticks    = 0;
    tile->planted      = false;
}

/*
 * Read one ADC channel using ADC1.
 * This helper is shared by the light sensor and potentiometer.
 */
static uint16_t Farm_ReadADCChannel(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    /*
     * Reconfigure the regular ADC channel before each reading.
     * This allows the same ADC peripheral to read multiple inputs.
     */
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);

    {
        uint16_t value = (uint16_t)HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        return value;
    }
}

/*
 * Read the light sensor ADC value.
 * The result is used to switch between daytime and night-time behaviour.
 */
static uint16_t Farm_ReadLightADC(void)
{
    return Farm_ReadADCChannel(LIGHT_ADC_CHANNEL);
}

/*
 * Read the potentiometer ADC value.
 * The result is used to control the watering amount in watering mode.
 */
static uint16_t Farm_ReadPotADC(void)
{
    return Farm_ReadADCChannel(POT_ADC_CHANNEL);
}

/*
 * Convert the potentiometer ADC value into a water amount.
 * The output is clamped by the configured minimum and maximum water range.
 */
static uint8_t Farm_GetWaterAmountFromPot(void)
{
    uint16_t pot_value = Farm_ReadPotADC();

    uint32_t amount = POT_WATER_MIN +
        ((uint32_t)pot_value * (POT_WATER_MAX - POT_WATER_MIN)) / 4095U;

    return (uint8_t)amount;
}

/*
 * Update the day/night state using the light sensor.
 * Crops stop growing at night, and a popup is shown when the scene changes to night.
 */
static void Farm_UpdateDayNight(void)
{
    static uint8_t prev_daytime = 1;

    uint16_t light_value = Farm_ReadLightADC();

    farm_state.is_daytime = (light_value >= LIGHT_DAY_THRESHOLD) ? 1U : 0U;

    /*
     * Show the rest message only when the light state changes.
     * This avoids repeatedly displaying the same popup every frame.
     */
    if (prev_daytime != farm_state.is_daytime) {
        if (!farm_state.is_daytime) {
            FarmEffects_ShowPopup(FARM_POPUP_REST,
                                  "Night - crops rest",
                                  1200);

            Farm_StartToneSequence(FARM_SOUND_NIGHT_FREQS,
                                   FARM_SOUND_NIGHT_DURS,
                                   FARM_SOUND_NIGHT_LEN,
                                   1);
        }

        prev_daytime = farm_state.is_daytime;
    }
}

/*
 * Update the temperature state using the DS18B20 sensor.
 * Temperature conversion is handled in a non-blocking way:
 * first start conversion, then read the result after completion or timeout.
 */
static void Farm_UpdateTemperatureEvent(void)
{
    static FarmTempState prev_temp_state = TEMP_STATE_NORMAL;
    uint32_t now = HAL_GetTick();

    /*
     * Start a new temperature conversion after the sampling interval.
     */
    if (!farm_state.temp_conversion_active) {
        if ((now - farm_state.temp_last_update_time) >= TEMP_SAMPLE_INTERVAL_MS) {
            if (DS18B20_StartConversion()) {
                farm_state.temp_conversion_active = 1;
                farm_state.temp_request_time = now;
            }
        }

        return;
    }

    /*
     * Read the temperature when conversion is complete.
     * A timeout is also used to prevent the game from waiting forever.
     */
    if (DS18B20_IsConversionDone() ||
        (now - farm_state.temp_request_time) >= TEMP_CONVERSION_TIMEOUT_MS) {
        float temp_c = 25.0f;

        if (DS18B20_ReadTemperatureC(&temp_c)) {
            farm_state.temp_c = temp_c + TEMP_CALIB_OFFSET_C;

            /*
             * Convert the measured temperature into a gameplay temperature state.
             */
            if (farm_state.temp_c <= TEMP_COLD_THRESHOLD_C) {
                farm_state.temp_state = TEMP_STATE_COLD;
            } else if (farm_state.temp_c >= TEMP_HOT_THRESHOLD_C) {
                farm_state.temp_state = TEMP_STATE_HOT;
            } else {
                farm_state.temp_state = TEMP_STATE_NORMAL;
            }

            /*
             * Show a warning popup only when the temperature state changes.
             */
            if (farm_state.temp_state != prev_temp_state) {
                if (farm_state.temp_state == TEMP_STATE_COLD) {
                    FarmEffects_ShowPopup(FARM_POPUP_WEATHER,
                                          "Too cold - growth stops",
                                          1200);
                } else if (farm_state.temp_state == TEMP_STATE_HOT) {
                    FarmEffects_ShowPopup(FARM_POPUP_WEATHER,
                                          "Too hot - water more",
                                          1200);
                }

                prev_temp_state = farm_state.temp_state;
            }
        }

        farm_state.temp_conversion_active = 0;
        farm_state.temp_last_update_time = now;
    }
}

/*
 * Reset daily gameplay values after sleeping.
 * The day number increases, energy is restored, and temporary crop dry states
 * are cleared for living crops.
 */
static void Farm_ResetForNextDay(void)
{
    int r, c;

    farm_state.game.day++;
    farm_state.game.energy = farm_state.game.max_energy;
    farm_state.is_daytime = 1;
    farm_state.temp_state = TEMP_STATE_NORMAL;

    Farm_RGB_UpdateByTemperature();

    /*
     * Living crops get a fresh start for the new day.
     * Dead and mature crops are not modified here.
     */
    for (r = 0; r < FARM_ROWS; r++) {
        for (c = 0; c < FARM_COLS; c++) {
            if (farm_state.game.tiles[r][c].state == TILE_SEEDED ||
                farm_state.game.tiles[r][c].state == TILE_GROWING) {
                farm_state.game.tiles[r][c].dry_ticks = 0;
            }
        }
    }
}

/*
 * Set the farm girl expression for a limited time.
 * A happy mood also starts a short spin animation.
 */
static void Farm_SetGirlMood(FarmGirlMood mood, uint32_t duration_ms)
{
    farm_state.girl_mood = mood;
    farm_state.girl_mood_until = HAL_GetTick() + duration_ms;

    if (mood == GIRL_MOOD_HAPPY) {
        farm_state.girl_spin_active = 1;
        farm_state.girl_spin_start  = HAL_GetTick();
    }
}

/*
 * Update the farm girl position, movement animation, and temporary mood state.
 * The girl moves toward the selected home/shop target when one is selected.
 */
static void Farm_UpdateGirlState(uint32_t now)
{
    int target_x = GIRL_X;
    int target_y = GIRL_Y;
    int dx, dy;
    int step = 3;

    /*
     * Return the girl to the idle mood after the mood duration expires.
     */
    if (farm_state.girl_mood != GIRL_MOOD_IDLE &&
        farm_state.girl_mood_until != 0 &&
        now >= farm_state.girl_mood_until) {
        farm_state.girl_mood = GIRL_MOOD_IDLE;
        farm_state.girl_mood_until = 0;
    }

    /*
     * Choose the target position according to the selected interactive building.
     */
    if (farm_state.selected_target == FARM_SELECT_HOME) {
        target_x = HOUSE_X + 2;
        target_y = GIRL_Y - 4;
    } else if (farm_state.selected_target == FARM_SELECT_SHOP) {
        target_x = SHOP_X + 14;
        target_y = GIRL_Y - 4;
    }

    dx = target_x - (int)farm_state.girl_x;
    dy = target_y - (int)farm_state.girl_y;

    /*
     * Smoothly move the character towards the target position.
     */
    if (dx > step)       farm_state.girl_x += step;
    else if (dx < -step) farm_state.girl_x -= step;
    else                 farm_state.girl_x = (int16_t)target_x;

    if (dy > step)       farm_state.girl_y += step;
    else if (dy < -step) farm_state.girl_y -= step;
    else                 farm_state.girl_y = (int16_t)target_y;

    farm_state.girl_is_moving =
        (farm_state.girl_x != target_x || farm_state.girl_y != target_y) ? 1U : 0U;

    /*
     * Advance the walking sprite frame only while the girl is moving.
     */
    if (farm_state.girl_is_moving) {
        if ((now - farm_state.girl_walk_tick) >= 120U) {
            farm_state.girl_walk_frame = (farm_state.girl_walk_frame + 1U) % 4U;
            farm_state.girl_walk_tick  = now;
        }
    } else {
        farm_state.girl_walk_frame = 0;
    }

    /*
     * Stop the short spin animation after its fixed duration.
     */
    if (farm_state.girl_spin_active &&
        (now - farm_state.girl_spin_start) >= 800U) {
        farm_state.girl_spin_active = 0;
    }
}

/*
 * Initialise the complete farm game module.
 * This function resets gameplay state, hardware display state, sensors,
 * effects, sounds, and all crop tiles.
 */
void Farm_Init(void)
{
    int r, c;

    /*
     * Clear the whole game state structure before assigning default values.
     */
    memset(&farm_state, 0, sizeof(farm_state));

    /*
     * Initialise core gameplay values.
     */
    farm_state.game.cursor_row = 0;
    farm_state.game.cursor_col = 0;
    farm_state.game.mode = MODE_PLANT;
    Farm_SyncCoinsFromSharedEconomy();

    farm_state.game.day = 1;
    farm_state.game.max_energy = FARM_MAX_ENERGY;
    farm_state.game.energy = farm_state.game.max_energy;
    farm_state.game.seeds = FARM_START_SEEDS;

    /*
     * Start from the rules screen.
     */
    farm_state.game.scene = FARM_SCENE_RULES;
    farm_state.game.selecting_home = 0;
    farm_state.game.selecting_shop = 0;
    farm_state.game.home_sleeping_message = 0;

    farm_state.harvest_count = 0;
    farm_state.selected_target = FARM_SELECT_NONE;

    /*
     * Initialise sound and environmental states.
     */
    farm_state.sound_off_time = 0;
    farm_state.sound_playing = 0;
    farm_state.is_daytime = 1;
    farm_state.water_preview_amount = POT_WATER_MIN;

    /*
     * Initialise temperature sensor state.
     */
    farm_state.temp_state = TEMP_STATE_NORMAL;
    farm_state.temp_c = 25.0f;
    farm_state.temp_conversion_active = 0;
    farm_state.temp_request_time = 0;
    farm_state.temp_last_update_time = 0;

    /*
     * Initialise farm girl animation and mood state.
     */
    farm_state.girl_mood = GIRL_MOOD_IDLE;
    farm_state.girl_mood_until = 0;
    farm_state.girl_x = GIRL_X;
    farm_state.girl_y = GIRL_Y;
    farm_state.girl_is_moving = 0;
    farm_state.girl_walk_frame = 0;
    farm_state.girl_walk_tick = 0;
    farm_state.girl_spin_active = 0;
    farm_state.girl_spin_start = 0;

    farm_sleep_return_time = 0;

    /*
     * Ensure no old buzzer output remains active.
     */
    Farm_StopAllSound();

    /*
     * Initialise visual effects and external display modules.
     */
    FarmEffects_Init();

    SevenSeg_Init();
    SevenSeg_DisplayDigit(farm_state.harvest_count % 10);

    /*
     * Start the first temperature conversion.
     * The result will be read later in Farm_UpdateTemperatureEvent().
     */
    DS18B20_Init();
    DS18B20_StartConversion();

    farm_state.temp_conversion_active = 1;
    farm_state.temp_request_time = HAL_GetTick();

    /*
     * Initialise all 4 x 4 farm tiles as empty soil.
     */
    for (r = 0; r < FARM_ROWS; r++) {
        for (c = 0; c < FARM_COLS; c++) {
            farm_state.game.tiles[r][c].state        = TILE_EMPTY;
            farm_state.game.tiles[r][c].water_level  = 0;
            farm_state.game.tiles[r][c].growth_ticks = 0;
            farm_state.game.tiles[r][c].dry_ticks    = 0;
            farm_state.game.tiles[r][c].planted      = false;
        }
    }

    Farm_RGB_UpdateByTemperature();
}

/*
 * Move the crop grid cursor by one step.
 * The cursor is clamped inside the 4 x 4 farm grid.
 */
static void Farm_MoveCursor(int8_t d_row, int8_t d_col)
{
    int nr = (int)farm_state.game.cursor_row + d_row;
    int nc = (int)farm_state.game.cursor_col + d_col;

    if (nr >= 0 && nr < FARM_ROWS) {
        farm_state.game.cursor_row = (uint8_t)nr;
    }

    if (nc >= 0 && nc < FARM_COLS) {
        farm_state.game.cursor_col = (uint8_t)nc;
    }
}

/*
 * Perform the current action on the selected tile.
 * The selected mode determines whether the action is planting, watering,
 * harvesting, or clearing.
 */
static void Farm_PerformAction(void)
{
    CropTile *tile = &farm_state.game.tiles[farm_state.game.cursor_row][farm_state.game.cursor_col];
    uint8_t action_done = 0;

    /*
     * Every valid farm action consumes energy.
     * If there is no energy left, the action is rejected.
     */
    if (farm_state.game.energy == 0) {
        FarmEffects_ShowPopup(FARM_POPUP_WARNING,
                              "Out of energy!",
                              1000);

        Farm_StartToneSequence(FARM_SOUND_FAIL_FREQS,
                               FARM_SOUND_FAIL_DURS,
                               3,
                               1);
        return;
    }

    switch (farm_state.game.mode) {
        case MODE_PLANT:
            /*
             * Planting requires at least one seed and an empty tile.
             */
            if (farm_state.game.seeds == 0) {
                FarmEffects_ShowPopup(FARM_POPUP_WARNING,
                                      "Out of seeds!",
                                      1000);

                Farm_StartToneSequence(FARM_SOUND_FAIL_FREQS,
                                       FARM_SOUND_FAIL_DURS,
                                       3,
                                       1);
                return;
            }

            if (tile->state == TILE_EMPTY) {
                tile->state        = TILE_SEEDED;
                tile->water_level  = 0;
                tile->growth_ticks = 0;
                tile->dry_ticks    = 0;
                tile->planted      = true;

                farm_state.game.seeds--;

                FarmEffects_StartTileEffect(FARM_EFFECT_PLANT,
                                            farm_state.game.cursor_row,
                                            farm_state.game.cursor_col,
                                            350);

                action_done = 1;
            }
            break;

        case MODE_WATER:
            /*
             * Water can only be added to seeded or growing crops.
             * The amount is controlled by the potentiometer.
             */
            if (tile->state == TILE_SEEDED || tile->state == TILE_GROWING) {
                uint8_t water_add = farm_state.water_preview_amount;

                /*
                 * Overwatering kills the crop immediately.
                 */
                if ((tile->water_level + water_add) > OVERWATER_DEATH_LEVEL) {
                    tile->state       = TILE_DEAD;
                    tile->water_level = 0;
                    tile->dry_ticks   = 0;

                    Farm_SetGirlMood(GIRL_MOOD_SAD, 1000);

                    FarmEffects_ShowPopup(FARM_POPUP_WARNING,
                                          "Too much water!",
                                          1000);

                    Farm_StartToneSequence(FARM_SOUND_OVERWATER_FREQS,
                                           FARM_SOUND_OVERWATER_DURS,
                                           FARM_SOUND_OVERWATER_LEN,
                                           1);

                    action_done = 1;
                } else {
                    tile->water_level += water_add;
                    tile->dry_ticks = 0;

                    FarmEffects_StartTileEffect(FARM_EFFECT_WATER,
                                                farm_state.game.cursor_row,
                                                farm_state.game.cursor_col,
                                                350);

                    action_done = 1;
                }
            }
            break;

        case MODE_HARVEST:
            /*
             * Mature crops give coins and increase the harvest counter.
             */
            if (tile->state == TILE_MATURE) {
                Farm_ClearTile(tile);

                Add_Gold(10);
                Farm_SyncCoinsFromSharedEconomy();

                farm_state.harvest_count += 1;

                SevenSeg_DisplayDigit(farm_state.harvest_count % 10);

                FarmEffects_TriggerCoinBounce();

                FarmEffects_StartTileEffect(FARM_EFFECT_HARVEST,
                                            farm_state.game.cursor_row,
                                            farm_state.game.cursor_col,
                                            400);

                Farm_SetGirlMood(GIRL_MOOD_HAPPY, 1000);

                Farm_StartToneSequence(FARM_SOUND_HARVEST_FREQS,
                                       FARM_SOUND_HARVEST_DURS,
                                       3,
                                       1);

                action_done = 1;
            }
            /*
             * Dead crops can be cleared in harvest mode.
             */
            else if (tile->state == TILE_DEAD) {
                Farm_ClearTile(tile);

                Farm_PlayFeedbackTone(FARM_SOUND_CLEAR_FREQ,
                                      FARM_SOUND_CLEAR_VOL,
                                      FARM_SOUND_CLEAR_MS);

                action_done = 1;
            }
            break;
    }

    /*
     * Consume one energy only when an action was actually completed.
     */
    if (action_done && farm_state.game.energy > 0) {
        farm_state.game.energy--;
    }
}

/*
 * Update crop growth for every tile.
 * Crop growth depends on daytime, temperature, water level, and dry duration.
 */
static void Farm_UpdateGrowth(void)
{
    int r, c;
    uint8_t any_crop_died = 0;

    /*
     * Crops do not grow at night.
     */
    if (!farm_state.is_daytime) {
        return;
    }

    for (r = 0; r < FARM_ROWS; r++) {
        for (c = 0; c < FARM_COLS; c++) {
            CropTile *tile = &farm_state.game.tiles[r][c];

            if (tile->state == TILE_SEEDED || tile->state == TILE_GROWING) {

                /*
                 * Cold temperature stops growth but does not kill the crop directly.
                 */
                if (farm_state.temp_state == TEMP_STATE_COLD) {
                    continue;
                }

                /*
                 * If the crop has no water, increase drought duration.
                 * Too much drought kills the crop.
                 */
                if (tile->water_level == 0) {
                    tile->dry_ticks++;

                    if (tile->dry_ticks >= DROUGHT_DEATH_TICKS) {
                        tile->state = TILE_DEAD;
                        tile->planted = false;
                        any_crop_died = 1;
                    }

                    continue;
                }

                /*
                 * If the crop has water, it can grow and its dry counter resets.
                 */
                tile->dry_ticks = 0;
                tile->growth_ticks++;

                /*
                 * Water decreases faster in hot temperature.
                 */
                {
                    uint8_t water_decay = (farm_state.temp_state == TEMP_STATE_HOT) ?
                                          WATER_DECAY_HOT : WATER_DECAY_NORMAL;

                    if (tile->water_level > water_decay) {
                        tile->water_level -= water_decay;
                    } else {
                        tile->water_level = 0;
                    }
                }

                /*
                 * Change visual crop state according to accumulated growth ticks.
                 */
                if (tile->growth_ticks > SEEDED_TO_GROWING_TICKS &&
                    tile->state == TILE_SEEDED) {
                    tile->state = TILE_GROWING;
                }

                if (tile->growth_ticks > GROWING_TO_MATURE_TICKS) {
                    tile->state = TILE_MATURE;
                }
            }
        }
    }

    if (any_crop_died) {
        Farm_SetGirlMood(GIRL_MOOD_SAD, 1000);

        Farm_StartToneSequence(FARM_SOUND_DEATH_FREQS,
                               FARM_SOUND_DEATH_DURS,
                               3,
                               1);
    }
}

/*
 * Main farm update function.
 * It handles sound updates, scene logic, input, sensors, character movement,
 * crop growth, and state transitions.
 */
void Farm_Update(void)
{
    static uint32_t last_move_time = 0;
    static uint32_t last_growth_tick = 0;

    uint32_t now = HAL_GetTick();

    /*
     * Update visual effects every frame.
     */
    FarmEffects_Update();

    /*
     * Update buzzer output.
     * Sequence sounds are handled first; otherwise short single tones are stopped
     * after their scheduled duration.
     */
    if (farm_buzzer_seq_active) {
        Farm_UpdateToneSequence(now);
    } else if (farm_state.sound_playing && now >= farm_state.sound_off_time) {
        buzzer_off(&buzzer_cfg);
        farm_state.sound_playing = 0;
    }

    /*
     * Rules / introduction scene.
     * The joystick button starts the actual farm scene.
     */
    if (farm_state.game.scene == FARM_SCENE_RULES) {
        if (current_input.joy_pressed) {
            Farm_StopAllSound();
            farm_state.game.scene = FARM_SCENE_FIELD;
            return;
        }

        Farm_StartIntroMusicIfNeeded();
        return;
    }

    /*
     * Home scene.
     * BTN2 leaves the home scene.
     * BTN3 starts the sleep sequence and advances to the next day.
     */
    if (farm_state.game.scene == FARM_SCENE_HOME) {
        if (current_input.btn2_pressed && farm_state.game.home_sleeping_message == 0) {
            farm_state.game.scene = FARM_SCENE_FIELD;
            farm_state.game.selecting_home = 0;
            farm_state.game.selecting_shop = 0;
            farm_state.game.home_sleeping_message = 0;
            farm_state.selected_target = FARM_SELECT_NONE;
            farm_sleep_return_time = 0;
        }

        if (current_input.btn3_pressed && farm_state.game.home_sleeping_message == 0) {
            farm_state.game.home_sleeping_message = 1;

            Farm_ResetForNextDay();

            Farm_StartToneSequence(FARM_SOUND_SLEEP_FREQS,
                                   FARM_SOUND_SLEEP_DURS,
                                   FARM_SOUND_SLEEP_LEN,
                                   1);

            farm_sleep_return_time = HAL_GetTick() + 5200;
        }

        /*
         * After the sleep animation has played long enough, return to the field.
         */
        if (farm_state.game.home_sleeping_message &&
            farm_sleep_return_time != 0 &&
            HAL_GetTick() >= farm_sleep_return_time) {
            farm_state.game.scene = FARM_SCENE_FIELD;
            farm_state.game.selecting_home = 0;
            farm_state.game.selecting_shop = 0;
            farm_state.game.home_sleeping_message = 0;
            farm_state.selected_target = FARM_SELECT_NONE;
            farm_sleep_return_time = 0;
        }

        return;
    }

    /*
     * Shop scene.
     * Before entering the shop update, copy the shared gold balance into the
     * farm display value. After the shop changes that local value, commit the
     * difference back to the shared economy API.
     */
    if (farm_state.game.scene == FARM_SCENE_SHOP) {
        uint16_t coins_before;
        uint16_t coins_after;
        int shop_result;

        Farm_SyncCoinsFromSharedEconomy();
        coins_before = farm_state.game.coins;

        Joystick_Read(&joystick_cfg, &joystick_data);

        shop_result = farm_shop_update(current_input.btn2_pressed,
                                       current_input.btn3_pressed,
                                       joystick_data.direction,
                                       now,
                                       &farm_state.game.coins,
                                       &farm_state.game.seeds);

        coins_after = farm_state.game.coins;
        Farm_CommitCoinDeltaToSharedEconomy(coins_before, coins_after);

        if (shop_result == FARM_SHOP_RESULT_LEAVE) {
            farm_state.game.scene = FARM_SCENE_FIELD;
            farm_state.game.selecting_home = 0;
            farm_state.game.selecting_shop = 0;
            farm_state.selected_target = FARM_SELECT_NONE;
        }

        return;
    }

    /*
     * Field scene environmental updates.
     */
    Farm_UpdateDayNight();
    Farm_UpdateTemperatureEvent();
    Farm_RGB_UpdateByTemperature();

    /*
     * In watering mode, continuously update the displayed water preview amount
     * from the potentiometer.
     */
    if (farm_state.game.mode == MODE_WATER) {
        farm_state.water_preview_amount = Farm_GetWaterAmountFromPot();
    }

    /*
     * Read joystick state and process field scene input.
     */
    Joystick_Read(&joystick_cfg, &joystick_data);

    {
        UserInput joy_input = Joystick_GetInput(&joystick_data);

        /*
         * Move the crop grid cursor only when no building is selected.
         * A short movement delay prevents the cursor from moving too fast.
         */
        if (farm_state.selected_target == FARM_SELECT_NONE) {
            if (now - last_move_time > 150U) {
                if      (joy_input.direction == N) { Farm_MoveCursor(-1, 0); last_move_time = now; }
                else if (joy_input.direction == S) { Farm_MoveCursor( 1, 0); last_move_time = now; }
                else if (joy_input.direction == W) { Farm_MoveCursor( 0,-1); last_move_time = now; }
                else if (joy_input.direction == E) { Farm_MoveCursor( 0, 1); last_move_time = now; }
            }
        }

        /*
         * BTN2 cycles through plant, water, and harvest modes.
         */
        if (current_input.btn2_pressed) {
            farm_state.game.mode = (FarmMode)((farm_state.game.mode + 1) % 3);

            Farm_PlayFeedbackTone(FARM_SOUND_MODE_FREQ,
                                  FARM_SOUND_MODE_VOL,
                                  FARM_SOUND_MODE_MS);
        }

        /*
         * BTN3 cycles the building selection:
         * none -> home -> shop -> none.
         */
        if (current_input.btn3_pressed) {
            if (farm_state.selected_target == FARM_SELECT_NONE) {
                farm_state.selected_target = FARM_SELECT_HOME;
            } else if (farm_state.selected_target == FARM_SELECT_HOME) {
                farm_state.selected_target = FARM_SELECT_SHOP;
            } else {
                farm_state.selected_target = FARM_SELECT_NONE;
            }

            farm_state.game.selecting_home = (farm_state.selected_target == FARM_SELECT_HOME);
            farm_state.game.selecting_shop = (farm_state.selected_target == FARM_SELECT_SHOP);
        }

        /*
         * Update character movement after the selected target is changed.
         */
        Farm_UpdateGirlState(now);

        /*
         * Pressing the joystick button enters the selected building,
         * or performs a farm action when no building is selected.
         */
        if (farm_state.selected_target == FARM_SELECT_HOME) {
            if (current_input.joy_pressed) {
                farm_state.game.scene = FARM_SCENE_HOME;
                farm_state.game.selecting_home = 0;
                farm_state.game.selecting_shop = 0;
                farm_state.game.home_sleeping_message = 0;
                farm_state.selected_target = FARM_SELECT_NONE;
                farm_sleep_return_time = 0;
                return;
            }
        } else if (farm_state.selected_target == FARM_SELECT_SHOP) {
            if (current_input.joy_pressed) {
                farm_shop_init();

                farm_state.game.scene = FARM_SCENE_SHOP;
                farm_state.game.selecting_home = 0;
                farm_state.game.selecting_shop = 0;
                farm_state.selected_target = FARM_SELECT_NONE;
                return;
            }
        } else {
            if (current_input.joy_pressed) {
                Farm_PerformAction();
            }
        }
    }

    /*
     * Crop growth is updated using the TIM6 tick base rather than the frame rate.
     */
    if ((g_tim6_ticks - last_growth_tick) >= FARM_GROWTH_TICK_INTERVAL) {
        Farm_UpdateGrowth();
        last_growth_tick = g_tim6_ticks;
    }
}

/*
 * Deinitialise the farm module.
 * This stops sound, turns off the RGB LED, blanks the seven-segment display,
 * and resets temporary runtime flags.
 */
void Farm_Deinit(void)
{
    Farm_StopAllSound();

    HAL_GPIO_WritePin(RGB_R_GPIO_Port, RGB_R_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_G_GPIO_Port, RGB_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RGB_B_GPIO_Port, RGB_B_Pin, GPIO_PIN_RESET);

    SevenSeg_Blank();

    farm_state.sound_playing = 0;
    farm_state.sound_off_time = 0;
    farm_state.is_daytime = 1;
    farm_sleep_return_time = 0;
}

/*
 * Render the current farm scene.
 * The actual drawing is delegated to the corresponding scene renderer.
 */
void Farm_Render(void)
{
    Farm_SyncCoinsFromSharedEconomy();

    if (farm_state.game.scene == FARM_SCENE_RULES) {
        FarmRender_RenderRules(HAL_GetTick());
        return;
    }

    if (farm_state.game.scene == FARM_SCENE_HOME) {
        FarmHome_Render(farm_state.game.home_sleeping_message);
        return;
    }

    if (farm_state.game.scene == FARM_SCENE_SHOP) {
        farm_shop_render(HAL_GetTick(), farm_state.game.coins, farm_state.game.seeds);
        return;
    }

    FarmRender_RenderField(&farm_state);
}