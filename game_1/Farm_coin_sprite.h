#ifndef FARM_COIN_SPRITE_H
#define FARM_COIN_SPRITE_H

#include <stdint.h>

/* PALETTE_FARM_UNIFIED
 * 0 black
 * 1 cream
 * 2 soft red
 * 3 leaf green
 * 4 denim blue
 * 5 warm orange
 * 6 soil brown
 * 7 dry soil
 * 8 honey gold
 * 9 panel blue
 * 10 stress orange
 * 11 grass green
 * 12 night blue
 * 13 stone grey
 * 14 water cyan
 * 15 sky cyan
 */

#define _T 255

static const uint8_t ICO_FARM_COIN[10 * 10] = {
    _T, _T,  6,  8,  8,  8,  6, _T, _T, _T,
    _T,  6,  8,  1,  1,  1,  8,  6, _T, _T,
    _T,  8,  1,  8,  8,  1,  8,  8, _T, _T,
    _T,  8,  1,  8,  1,  1,  8,  8, _T, _T,
    _T,  8,  1,  8,  8,  1,  8,  8, _T, _T,
    _T,  8,  1,  1,  1,  1,  8,  8, _T, _T,
    _T,  6,  8,  8,  8,  8,  6, _T, _T, _T,
    _T, _T, _T, _T, _T, _T, _T, _T, _T, _T,
    _T, _T, _T, _T, _T, _T, _T, _T, _T, _T,
    _T, _T, _T, _T, _T, _T, _T, _T, _T, _T,
};

#undef _T

#endif