#ifndef FARM_SPRITES_H
#define FARM_SPRITES_H

#include <stdint.h>

/* ── sprite dimensions ─────────────────────────────────────────── */

#define SPR_COIN_W       8
#define SPR_COIN_H       8

/* House: compact cozy cottage with red roof, 20×20 */
#define SPR_HOUSE_W     20
#define SPR_HOUSE_H     20

/* Fence post segment, 8×8 tiling unit */
#define SPR_FENCE_W      8
#define SPR_FENCE_H      8

/* Crop tiles: 10×10, drawn at scale 2 → 20×20 px on screen */
#define SPR_TILE_W      10
#define SPR_TILE_H      10

/* Cursor: simple white square border, 14×14 (drawn at scale 2 → 28×28) */
#define SPR_CURSOR_W    14
#define SPR_CURSOR_H    14

/* Tree canopy sprite for organic look, 16×16 */
#define SPR_TREE_W      16
#define SPR_TREE_H      16

/* Pumpkin decoration, 8×8 */
#define SPR_PUMPKIN_W    8
#define SPR_PUMPKIN_H    8

/* ── extern declarations ──────────────────────────────────────── */

extern const uint8_t spr_coin   [SPR_COIN_W   * SPR_COIN_H  ];
extern const uint8_t spr_house  [SPR_HOUSE_W  * SPR_HOUSE_H ];
extern const uint8_t spr_fence  [SPR_FENCE_W  * SPR_FENCE_H ];
extern const uint8_t spr_tree   [SPR_TREE_W   * SPR_TREE_H  ];
extern const uint8_t spr_pumpkin[SPR_PUMPKIN_W* SPR_PUMPKIN_H];

extern const uint8_t spr_tile_empty  [SPR_TILE_W * SPR_TILE_H];
extern const uint8_t spr_tile_seed   [SPR_TILE_W * SPR_TILE_H];
extern const uint8_t spr_tile_growing[SPR_TILE_W * SPR_TILE_H];
extern const uint8_t spr_tile_mature [SPR_TILE_W * SPR_TILE_H];
extern const uint8_t spr_tile_dead   [SPR_TILE_W * SPR_TILE_H];

extern const uint8_t spr_cursor[SPR_CURSOR_W * SPR_CURSOR_H];

#endif /* FARM_SPRITES_H */