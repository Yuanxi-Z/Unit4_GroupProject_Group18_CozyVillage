#ifndef FARM_PALETTE_H
#define FARM_PALETTE_H

/*
 * Transparent pixel index.
 * Pixels using this value should be skipped by the drawing function.
 */
#define PX_TRANSPARENT   255

/*
 * Raw palette indices for PALETTE_FARM_UNIFIED.
 *
 * The project uses a 16-colour indexed palette.
 * Each colour name below maps to one fixed palette slot.
 * These raw indices should stay consistent with the actual RGB colour table.
 */
#define CLR_BLACK         0
#define CLR_CREAM         1
#define CLR_SOFT_RED      2
#define CLR_LEAF_GREEN    3
#define CLR_DENIM_BLUE    4
#define CLR_WARM_ORANGE   5
#define CLR_SOIL_BROWN    6
#define CLR_DRY_SOIL      7
#define CLR_HONEY_GOLD    8
#define CLR_PANEL_BLUE    9
#define CLR_STRESS_ORANGE 10
#define CLR_GRASS_GREEN   11
#define CLR_NIGHT_BLUE    12
#define CLR_STONE_GREY    13
#define CLR_WATER_CYAN    14
#define CLR_SKY_CYAN      15

/*
 * General drawing aliases.
 *
 * These aliases make the code easier to read because game objects can use
 * semantic colour names instead of raw palette indices.
 */
#define CLR_INK           CLR_BLACK
#define CLR_TEXT_MAIN     CLR_CREAM
#define CLR_TEXT_ACCENT   CLR_HONEY_GOLD
#define CLR_TEXT_DIM      CLR_STONE_GREY

/*
 * Sky and weather colours.
 *
 * These colours are mainly used by the outdoor farm scene, sleep animation,
 * day/night effects, clouds, stars, and moon elements.
 */
#define CLR_SKY_DAY       CLR_SKY_CYAN
#define CLR_SKY_NIGHT     CLR_NIGHT_BLUE
#define CLR_CLOUD         CLR_CREAM
#define CLR_MOON          CLR_HONEY_GOLD

/*
 * Ground and path colours.
 *
 * These aliases are used for the main farm background, grass areas,
 * walking path, and simple environmental decoration.
 */
#define CLR_GRASS         CLR_GRASS_GREEN
#define CLR_PATH          CLR_STONE_GREY

/*
 * Wood and signboard colours.
 *
 * These colours are used for farm buildings, fences, wooden signs,
 * panels, doors, and other rustic objects.
 */
#define CLR_WOOD_DARK     CLR_SOIL_BROWN
#define CLR_WOOD_MID      CLR_DRY_SOIL
#define CLR_WOOD_LIGHT    CLR_HONEY_GOLD
#define CLR_PARCH         CLR_CREAM

/*
 * HUD colour aliases.
 *
 * These colours are used for the top/bottom status panels, coin display,
 * mode display, text labels, and interface borders.
 */
#define CLR_HUD_BG        CLR_PANEL_BLUE
#define CLR_HUD_BORDER    CLR_CREAM
#define CLR_HUD_TEXT      CLR_HONEY_GOLD
#define CLR_HUD_DIM       CLR_STONE_GREY

/*
 * Selection highlight colours.
 *
 * These colours are used to draw the current selected tile, object,
 * or interactive UI area.
 */
#define CLR_SELECT_EDGE   CLR_CREAM
#define CLR_SELECT_BG     CLR_HONEY_GOLD

/*
 * Feedback colours.
 *
 * These semantic names are used for positive states, warnings,
 * and error or failure states.
 */
#define CLR_OK            CLR_LEAF_GREEN
#define CLR_WARN          CLR_STRESS_ORANGE
#define CLR_DANGER        CLR_SOFT_RED

/*
 * Crop tile colours.
 *
 * These colours represent the full crop life cycle on the 4 x 4 farm grid:
 *
 *   Empty soil      -> dark brown
 *   Seeded soil     -> dry light brown
 *   Recently watered-> cyan blue
 *   Growing crop    -> grass green
 *   Dry growing crop-> leaf green
 *   Mature crop     -> honey gold
 *   Dead crop       -> soft red
 *
 * The tile border uses stone grey instead of white so that the grid looks
 * softer and fits the pixel farm style better.
 */
#define CLR_TILE_EMPTY       CLR_SOIL_BROWN
#define CLR_TILE_SEEDED      CLR_DRY_SOIL
#define CLR_TILE_WET         CLR_WATER_CYAN
#define CLR_TILE_GROWING     CLR_GRASS_GREEN
#define CLR_TILE_GROWING_DRY CLR_LEAF_GREEN
#define CLR_TILE_MATURE      CLR_HONEY_GOLD
#define CLR_TILE_DEAD        CLR_SOFT_RED
#define CLR_TILE_BORDER      CLR_STONE_GREY

/*
 * Room colour aliases.
 *
 * These names are kept compatible with mine_home.c and mine_palette.h.
 * They all map back to the same 16-colour unified farm palette so that
 * the home scene and farm scene share one consistent colour system.
 */
#define CLR_DARK_BROWN    CLR_DRY_SOIL
#define CLR_TRUE_BROWN    CLR_SOIL_BROWN
#define CLR_AMBER         CLR_HONEY_GOLD
#define CLR_CORAL         CLR_STRESS_ORANGE
#define CLR_DENIM         CLR_DENIM_BLUE
#define CLR_SKY           CLR_SKY_CYAN
#define CLR_SOFT_PINK     CLR_CREAM
#define CLR_SAGE          CLR_GRASS_GREEN
#define CLR_DUSK_PURPLE   CLR_NIGHT_BLUE

#endif /* FARM_PALETTE_H */