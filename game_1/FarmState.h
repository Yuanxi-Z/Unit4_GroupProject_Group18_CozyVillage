#ifndef FARM_STATE_H
#define FARM_STATE_H

#include "Farm.h"
#include <stdint.h>

/*
 * Temperature state used by the farm environment system.
 * It affects crop growth and water consumption.
 */
typedef enum {
    TEMP_STATE_COLD = 0,
    TEMP_STATE_NORMAL,
    TEMP_STATE_HOT
} FarmTempState;

/*
 * Current selected interactive target in the village area.
 */
typedef enum {
    FARM_SELECT_NONE = 0,
    FARM_SELECT_HOME,
    FARM_SELECT_SHOP
} FarmSelectTarget;

/*
 * Temporary mood state for the farm girl sprite.
 */
typedef enum {
    GIRL_MOOD_IDLE = 0,
    GIRL_MOOD_HAPPY,
    GIRL_MOOD_SAD
} FarmGirlMood;

/*
 * Complete runtime state of the farm module.
 * This structure combines gameplay data, environment data, sound state,
 * selection state, and character animation state.
 */
typedef struct {
    /*
     * Core gameplay state, including crop tiles, resources, cursor,
     * current mode, and current scene.
     */
    FarmGame game;

    /*
     * Sound control state.
     * Used to stop short tones or tone sequences at the correct time.
     */
    uint32_t sound_off_time;
    uint8_t  sound_playing;

    /*
     * Environment and gameplay display state.
     */
    uint8_t  is_daytime;
    uint8_t  water_preview_amount;
    uint16_t harvest_count;

    /*
     * Temperature sensor state.
     * DS18B20 conversion is handled asynchronously, so the module stores
     * whether a conversion is active and when it was requested.
     */
    FarmTempState temp_state;
    float         temp_c;
    uint8_t       temp_conversion_active;
    uint32_t      temp_request_time;
    uint32_t      temp_last_update_time;

    /*
     * Currently selected village target.
     * This is used to move the girl towards the home or shop and to enter
     * the selected scene when the joystick button is pressed.
     */
    FarmSelectTarget selected_target;

    /*
     * Farm girl mood and movement state.
     * These values decide which sprite frame is drawn and where the character
     * appears on the village area.
     */
    FarmGirlMood girl_mood;
    uint32_t     girl_mood_until;
    int16_t      girl_x;
    int16_t      girl_y;
    uint8_t      girl_is_moving;
    uint8_t      girl_walk_frame;      /* Walking animation frame index, 0-3. */
    uint32_t     girl_walk_tick;       /* Last time the walking frame changed. */
    uint8_t      girl_spin_active;     /* 1 while the happy spin animation is active. */
    uint32_t     girl_spin_start;      /* HAL_GetTick() value when the spin started. */
} FarmState;

#endif