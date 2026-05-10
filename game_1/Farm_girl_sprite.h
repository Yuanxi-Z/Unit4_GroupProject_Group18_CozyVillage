#ifndef FARM_GIRL_SPRITE_H
#define FARM_GIRL_SPRITE_H

#include <stdint.h>

/*
 * Transparent pixel marker.
 * The drawing function should skip pixels with this value so that the farm
 * background remains visible around the character.
 */
#define _T 255

/*
 * Farm girl sprite notes:
 *
 * - Each sprite is 14 x 14 pixels.
 * - Pixel data is stored in row-major order.
 * - Each number is a palette index rather than a direct RGB colour.
 * - The actual colours are defined by the rendering function or colour table.
 * - _T represents a transparent pixel.
 *
 * General palette meaning used by this sprite set:
 *   0  = dark facial detail
 *   1  = dress / inner clothing detail
 *   2  = main clothing colour
 *   4  = legs
 *   5  = skin tone
 *   6  = hair outline / side hair
 *   7  = shoes
 *   8  = hair top / hat-like hair detail
 *   10 = cheek / face highlight
 */

/*
 * Idle frame.
 * This is the default standing pose used when the player is not moving.
 */
static const uint8_t SPR_FARM_GIRL_IDLE[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6, 5, 0, 5, 5, 5, 5, 0, 5, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 2, 2, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T,_T, 4, 4,_T,_T, 4, 4,_T,_T,_T,_T,
    _T,_T,_T, 7, 7, 7,_T,_T, 7, 7, 7,_T,_T,_T,
};

/*
 * Walk animation frame 0.
 * The left leg is placed forward and the right leg is placed back.
 * This frame begins the walking cycle.
 */
static const uint8_t SPR_FARM_GIRL_WALK0[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6, 5, 0, 5, 5, 5, 5, 0, 5, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 2, 2, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T, 4, 4,_T,_T,_T,_T, 4, 4,_T,_T,_T,
    _T,_T, 7, 7, 7,_T,_T,_T,_T, 7, 7, 7,_T,_T,
};

/*
 * Walk animation frame 1.
 * Both legs return to a straight contact position.
 * This creates a smoother transition between the two stride poses.
 */
static const uint8_t SPR_FARM_GIRL_WALK1[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6, 5, 0, 5, 5, 5, 5, 0, 5, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 2, 2, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T,_T, 4, 4,_T,_T, 4, 4,_T,_T,_T,_T,
    _T,_T,_T, 7, 7, 7,_T,_T, 7, 7, 7,_T,_T,_T,
};

/*
 * Walk animation frame 2.
 * The right leg is placed forward and the left leg is placed back.
 * This is the opposite stride pose of frame 0.
 */
static const uint8_t SPR_FARM_GIRL_WALK2[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6, 5, 0, 5, 5, 5, 5, 0, 5, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 2, 2, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T, 4, 4,_T,_T,_T,_T, 4, 4,_T,_T,_T,
    _T,_T, 7, 7, 7,_T,_T,_T,_T, 7, 7, 7,_T,_T,
};

/*
 * Walk animation frame 3.
 * This repeats the contact pose to complete the four-frame walk cycle.
 * The full cycle can be played as WALK0 -> WALK1 -> WALK2 -> WALK3.
 */
static const uint8_t SPR_FARM_GIRL_WALK3[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6, 5, 0, 5, 5, 5, 5, 0, 5, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 2, 2, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T,_T, 4, 4,_T,_T, 4, 4,_T,_T,_T,_T,
    _T,_T,_T, 7, 7, 7,_T,_T, 7, 7, 7,_T,_T,_T,
};

/*
 * Happy expression frame.
 * This frame is used for positive feedback, such as successful planting,
 * harvesting, buying items, or completing a good action.
 */
static const uint8_t SPR_FARM_GIRL_HAPPY[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6,10, 0, 0, 5, 5, 0, 0,10, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 5, 5, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 2, 5, 5, 5, 5, 5, 5, 2, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T,_T, 4, 4,_T,_T, 4, 4,_T,_T,_T,_T,
    _T,_T,_T, 7, 7, 7,_T,_T, 7, 7, 7,_T,_T,_T,
};

/*
 * Sad expression frame.
 * This frame is used for negative feedback, such as failed actions,
 * insufficient coins, dead crops, or unavailable interactions.
 */
static const uint8_t SPR_FARM_GIRL_SAD[14 * 14] = {
    _T,_T,_T,_T, 8, 8, 8, 8, 8, 8,_T,_T,_T,_T,
    _T,_T,_T, 8, 8, 1, 8, 8, 8, 8, 8,_T,_T,_T,
    _T,_T, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,_T,_T,
    _T,_T,_T, 6, 6, 6, 6, 6, 6, 6, 6,_T,_T,_T,
    _T, 6, 6,_T, 5, 5, 5, 5, 5, 5,_T, 6, 6,_T,
    _T, 6, 6, 5, 0, 5, 5, 5, 5, 0, 5, 6, 6,_T,
    _T, 6, 6,10, 5, 5, 5, 5, 5, 5,10, 6, 6,_T,
    _T, 6, 2, 5, 5, 5, 2, 2, 5, 5, 5, 2, 6,_T,
    _T,_T, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2,_T,_T,
    _T,_T, 2, 2, 8, 1, 8, 8, 1, 8, 2, 2,_T,_T,
    _T,_T, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,_T,_T,
    _T, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5,_T,
    _T,_T,_T,_T, 4, 4,_T,_T, 4, 4,_T,_T,_T,_T,
    _T,_T,_T, 7, 7, 7,_T,_T, 7, 7, 7,_T,_T,_T,
};

/*
 * Remove the temporary transparent-pixel macro after all sprite definitions.
 * This prevents _T from affecting other files that include this header.
 */
#undef _T

#endif