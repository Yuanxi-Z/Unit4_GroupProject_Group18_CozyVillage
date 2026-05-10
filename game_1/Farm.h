#ifndef FARM_H
#define FARM_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Farm grid size.
 * The main field is a 4 x 4 crop grid, giving 16 planting tiles in total.
 */
#define FARM_ROWS 4
#define FARM_COLS 4

/*
 * Crop tile state.
 * Each tile on the farm grid can be empty, planted, growing, mature, or dead.
 */
typedef enum {
    TILE_EMPTY = 0,   /* Empty soil, available for planting. */
    TILE_SEEDED,      /* A seed has been planted but has not grown yet. */
    TILE_GROWING,     /* The crop is growing and still needs water. */
    TILE_MATURE,      /* The crop is fully grown and can be harvested. */
    TILE_DEAD         /* The crop has died due to drought or overwatering. */
} TileState;

/*
 * Current player action mode.
 * The selected mode decides what happens when the player presses the action button.
 */
typedef enum {
    MODE_PLANT = 0,   /* Plant a seed on an empty tile. */
    MODE_WATER,       /* Add water to a seeded or growing crop. */
    MODE_HARVEST      /* Harvest mature crops or clear dead crops. */
} FarmMode;

/*
 * Main farm scene state.
 * The game switches between the intro rules page, field, home, and shop.
 */
typedef enum {
    FARM_SCENE_RULES = 0,  /* Introduction / rules screen shown before gameplay starts. */
    FARM_SCENE_FIELD,      /* Main farm field scene with crop grid and character. */
    FARM_SCENE_HOME,       /* Home scene used for sleeping and advancing to the next day. */
    FARM_SCENE_SHOP        /* Shop scene used for buying seeds or other items. */
} FarmScene;

/*
 * Crop tile data.
 * This structure stores all gameplay information for one tile in the farm grid.
 */
typedef struct {
    TileState state;        /* Current crop state of this tile. */
    uint8_t water_level;    /* Current water amount stored in this tile. */
    uint16_t growth_ticks;  /* Growth progress counter. */
    uint16_t dry_ticks;     /* Number of growth ticks without water. */
    bool planted;           /* True if this tile currently contains a planted crop. */
} CropTile;

/*
 * Main farm gameplay data.
 * This structure stores the field state, player resources, cursor position,
 * current scene, and interaction flags.
 */
typedef struct {
    /*
     * Crop grid data.
     * Each element represents one tile in the 4 x 4 farm field.
     */
    CropTile tiles[FARM_ROWS][FARM_COLS];

    /*
     * Cursor position on the crop grid.
     * These values are used to select which tile receives the current action.
     */
    uint8_t cursor_row;
    uint8_t cursor_col;

    /*
     * Current action mode and player coins.
     */
    FarmMode mode;
    uint16_t coins;

    /*
     * Day and energy system.
     * Energy is consumed by valid farm actions and restored after sleeping.
     */
    uint8_t day;
    uint8_t energy;
    uint8_t max_energy;

    /*
     * Seed inventory.
     * Seeds are consumed when planting and can be increased through the shop.
     */
    uint8_t seeds;

    /*
     * Current active scene.
     * This controls which update and render logic should be used.
     */
    FarmScene scene;

    /*
     * Building selection flags.
     * These are used by the renderer to highlight the selected home or shop.
     */
    uint8_t selecting_home;
    uint8_t selecting_shop;

    /*
     * Home sleep message state.
     * When this flag is active, the home scene displays the sleeping animation
     * and then returns to the main farm field.
     */
    uint8_t home_sleeping_message;
} FarmGame;

/*
 * Initialise the farm module.
 * This resets gameplay data, crop tiles, sensors, effects, sound state,
 * display modules, and default scene settings.
 */
void Farm_Init(void);

/*
 * Update the farm module.
 * This function handles input, scene transitions, crop growth, sensors,
 * sound playback, animation states, and gameplay logic.
 */
void Farm_Update(void);

/*
 * Render the current farm scene.
 * The displayed screen depends on the active FarmScene value.
 */
void Farm_Render(void);

/*
 * Deinitialise the farm module.
 * This stops active outputs such as sound, RGB LED, and display indicators.
 */
void Farm_Deinit(void);

#endif