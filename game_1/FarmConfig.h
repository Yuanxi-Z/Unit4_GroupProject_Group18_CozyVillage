#ifndef FARM_CONFIG_H
#define FARM_CONFIG_H

#include "adc.h"

/* -------------------------------------------------------------------------- */
/* Gameplay timing                                                             */
/* -------------------------------------------------------------------------- */

/*
 * Crop growth update interval.
 * This value is compared with the TIM6 tick counter.
 * A smaller value makes crops update more frequently.
 */
#define FARM_GROWTH_TICK_INTERVAL   10

/* -------------------------------------------------------------------------- */
/* Crop, water, and growth parameters                                          */
/* -------------------------------------------------------------------------- */

/*
 * Normal water decay amount per growth update.
 * This value is used when the temperature is in the normal range.
 */
#define WATER_DECAY_NORMAL           5

/*
 * Hot-weather water decay amount per growth update.
 * Crops lose water faster when the temperature is too high.
 */
#define WATER_DECAY_HOT              8

/*
 * Maximum safe water level.
 * If watering makes the tile water level exceed this value,
 * the crop dies from overwatering.
 */
#define OVERWATER_DEATH_LEVEL       80

/*
 * Maximum number of dry growth updates before a crop dies.
 * This value is intentionally relaxed so that crops do not die too quickly.
 */
#define DROUGHT_DEATH_TICKS         14

/*
 * Dryness warning threshold.
 * This can be used by the renderer or effects system to show that a crop
 * is close to drought danger.
 */
#define FARM_DRY_ALERT_TICKS        10

/*
 * Growth threshold from seeded state to growing state.
 * When growth_ticks exceeds this value, TILE_SEEDED becomes TILE_GROWING.
 */
#define SEEDED_TO_GROWING_TICKS     30

/*
 * Growth threshold from growing state to mature state.
 * When growth_ticks exceeds this value, the crop becomes harvestable.
 */
#define GROWING_TO_MATURE_TICKS     60

/* -------------------------------------------------------------------------- */
/* Sound parameters                                                            */
/* -------------------------------------------------------------------------- */

/*
 * Basic sound parameters for harvest and crop death events.
 * Some newer sound effects may use tone sequences instead of these single tones,
 * but these values remain available for simple feedback sounds.
 */
#define FARM_SOUND_HARVEST_FREQ    1200
#define FARM_SOUND_DEATH_FREQ       350
#define FARM_SOUND_HARVEST_VOL       30
#define FARM_SOUND_DEATH_VOL         35
#define FARM_SOUND_HARVEST_MS        80
#define FARM_SOUND_DEATH_MS         140

/* -------------------------------------------------------------------------- */
/* ADC and environment sensor configuration                                    */
/* -------------------------------------------------------------------------- */

/*
 * Light sensor ADC channel.
 * The light value is used to determine whether the farm is in daytime or night.
 */
#define LIGHT_ADC_CHANNEL           ADC_CHANNEL_5

/*
 * Day/night threshold for the light sensor.
 * Values equal to or above this threshold are treated as daytime.
 */
#define LIGHT_DAY_THRESHOLD         2000

/*
 * Potentiometer ADC channel.
 * The potentiometer controls the watering amount in watering mode.
 */
#define POT_ADC_CHANNEL             ADC_CHANNEL_6

/*
 * Minimum and maximum water amount generated from the potentiometer.
 * The ADC value is mapped into this range before watering a crop.
 */
#define POT_WATER_MIN               2
#define POT_WATER_MAX               50

/*
 * Temperature thresholds for DS18B20-based environment events.
 * Cold temperature stops crop growth.
 * Hot temperature increases water decay.
 */
#define TEMP_COLD_THRESHOLD_C      25.0f
#define TEMP_HOT_THRESHOLD_C       29.0f

/*
 * Temperature sampling interval.
 * A new DS18B20 conversion is requested after this time interval.
 */
#define TEMP_SAMPLE_INTERVAL_MS    1500

/*
 * Maximum waiting time for one DS18B20 temperature conversion.
 * This prevents the game loop from waiting forever if the sensor does not respond.
 */
#define TEMP_CONVERSION_TIMEOUT_MS 900

/*
 * Temperature calibration offset.
 * This value is added to the measured temperature to compensate for sensor
 * or board-level measurement deviation.
 */
#define TEMP_CALIB_OFFSET_C       -1.0f

/* -------------------------------------------------------------------------- */
/* Initial gameplay values                                                     */
/* -------------------------------------------------------------------------- */

/*
 * Maximum player energy.
 * Energy is consumed by successful farm actions and restored after sleeping.
 */
#define FARM_MAX_ENERGY            20

/*
 * Starting number of seeds.
 * The player can buy more seeds from the shop later.
 */
#define FARM_START_SEEDS            3

/* -------------------------------------------------------------------------- */
/* Screen layout                                                               */
/* -------------------------------------------------------------------------- */

/*
 * LCD screen size.
 * The project uses a 240 x 240 ST7789 display.
 */
#define SCR_W          240
#define SCR_H          240

/*
 * Main vertical layout regions.
 *
 * TOP_H    : title / top information area
 * HUD_Y    : starting Y position of the HUD
 * HUD_H    : height of the status HUD
 * SCENE_Y  : starting Y position of the main scene area
 * FOOTER_H : height of the bottom instruction area
 * FOOTER_Y : starting Y position of the footer
 */
#define TOP_H           40
#define HUD_Y           40
#define HUD_H           22
#define SCENE_Y        (HUD_Y + HUD_H)
#define FOOTER_H        22
#define FOOTER_Y       (SCR_H - FOOTER_H)

/*
 * Crop tile size and spacing.
 * TILE_STEP is the distance from one tile origin to the next tile origin.
 */
#define TILE_SIZE       22
#define TILE_GAP         4
#define TILE_STEP       (TILE_SIZE + TILE_GAP)

/*
 * Farm grid size used by the renderer.
 * This matches the 4 x 4 gameplay grid.
 */
#define FIELD_ROWS       4
#define FIELD_COLS       4

/*
 * Inner farm field size.
 * This includes only crop tiles and gaps, not the outer padding.
 */
#define FIELD_INNER_W   (FIELD_COLS * TILE_SIZE + (FIELD_COLS - 1) * TILE_GAP)
#define FIELD_INNER_H   (FIELD_ROWS * TILE_SIZE + (FIELD_ROWS - 1) * TILE_GAP)

/*
 * Outer farm field panel size.
 * Padding creates a border area around the crop grid.
 */
#define FIELD_PAD        9
#define FIELD_W         (FIELD_INNER_W + FIELD_PAD * 2)
#define FIELD_H         (FIELD_INNER_H + FIELD_PAD * 2)

/*
 * Top-left position of the farm field panel.
 */
#define FIELD_X         108
#define FIELD_Y          72

/*
 * Top-left position of the first crop tile.
 * All other tile positions are calculated from this point.
 */
#define TILE_START_X    (FIELD_X + FIELD_PAD)
#define TILE_START_Y    (FIELD_Y + FIELD_PAD)

/*
 * Cursor padding around the selected tile.
 * This gives the selection box a small margin outside the tile.
 */
#define CURSOR_PAD       2

/*
 * Water preview bar layout.
 * This bar displays the current watering amount from the potentiometer.
 */
#define WATER_BAR_X      22
#define WATER_BAR_Y      176
#define WATER_BAR_W      52
#define WATER_BAR_H       8

/* -------------------------------------------------------------------------- */
/* Village layout                                                              */
/* -------------------------------------------------------------------------- */

/*
 * Home building position.
 * HOUSE_X and HOUSE_Y define the drawing origin of the house icon.
 */
#define HOUSE_X          10
#define HOUSE_Y          85

/*
 * Home selection frame.
 * These values define the highlight box used when the home is selected.
 */
#define HOUSE_FRAME_X     8
#define HOUSE_FRAME_Y    83
#define HOUSE_FRAME_W    44
#define HOUSE_FRAME_H    44

/*
 * Shop building position.
 * SHOP_X and SHOP_Y define the drawing origin of the shop icon.
 */
#define SHOP_X           62
#define SHOP_Y           90

/*
 * Shop selection frame.
 * These values define the highlight box used when the shop is selected.
 */
#define SHOP_FRAME_X     60
#define SHOP_FRAME_Y     86
#define SHOP_FRAME_W     44
#define SHOP_FRAME_H     44

/*
 * Default farm girl position.
 * The character moves away from this position when a building is selected,
 * then returns here when no building is selected.
 */
#define GIRL_X           43
#define GIRL_Y          132

#endif