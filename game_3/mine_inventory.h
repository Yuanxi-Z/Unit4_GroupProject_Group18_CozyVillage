#ifndef MINE_INVENTORY_H
#define MINE_INVENTORY_H

#include <stdint.h>

typedef struct {
    uint8_t copper;
    uint8_t gold;
    uint8_t diamond;
} OreCount_t;

typedef struct {
    uint8_t torch;
    uint8_t dynamite;
    uint8_t potion;
} ItemCount_t;

extern OreCount_t  ores;
extern ItemCount_t items;
extern uint16_t    coins;

void inv_reset(void);

void    ore_add(uint8_t type);
uint8_t ore_total(void);
uint16_t ore_sell_all(void);
void    ore_lose_half(void);

uint16_t item_buy_cost(uint8_t type);
uint8_t  item_buy(uint8_t type);
uint8_t  item_use_potion(void);

#endif // MINE_INVENTORY_H