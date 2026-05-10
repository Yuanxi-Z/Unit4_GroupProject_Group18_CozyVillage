#include "mine_inventory.h"

OreCount_t  ores;
ItemCount_t items;
uint16_t    coins;

#define POTION_COST        50

void inv_reset(void) {
    ores.copper = 0; ores.gold = 0; ores.diamond = 0;
    items.torch = 1; items.dynamite = 1; items.potion = 1;
    coins = 0;
}

void ore_add(uint8_t type) {
    switch (type) {
        case 0: if (ores.copper < 99)  ores.copper++;  break;
        case 1: if (ores.gold   < 99)  ores.gold++;    break;
        case 2: if (ores.diamond < 99) ores.diamond++; break;
    }
}

uint8_t ore_total(void) {
    return ores.copper + ores.gold + ores.diamond;
}

uint16_t ore_sell_all(void) {
    uint16_t earned = ores.copper * 10 + ores.gold * 30 + ores.diamond * 80;
    coins += earned;
    ores.copper = 0; ores.gold = 0; ores.diamond = 0;
    return earned;
}

void ore_lose_half(void) {
    ores.copper  /= 2;
    ores.gold    /= 2;
    ores.diamond /= 2;
}

uint16_t item_buy_cost(uint8_t type) {
    switch (type) {
        case 0: return 20;   // torch
        case 1: return 30;   // dynamite
        case 2: return POTION_COST;  // potion
        default: return 999;
    }
}

uint8_t item_buy(uint8_t type) {
    uint16_t cost = item_buy_cost(type);
    if (coins < cost) return 0;
    coins -= cost;
    switch (type) {
        case 0: items.torch++;    break;
        case 1: items.dynamite++; break;
        case 2: items.potion++;   break;
    }
    return 1;
}

uint8_t item_use_potion(void) {
    if (items.potion == 0) return 0;
    items.potion--; // Consume one potion
    return 1;
}