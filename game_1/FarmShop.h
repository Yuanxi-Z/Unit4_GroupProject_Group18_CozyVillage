#ifndef FARM_SHOP_H
#define FARM_SHOP_H

#include <stdint.h>
#include "Joystick.h"

typedef enum {
    FARM_SHOP_RESULT_STAY,
    FARM_SHOP_RESULT_LEAVE
} FarmShopResult;

void farm_shop_init(void);

FarmShopResult farm_shop_update(uint8_t bt2,
                                uint8_t bt3,
                                Direction dir,
                                uint32_t now_ms,
                                uint16_t *coins,
                                uint8_t *seeds);

void farm_shop_render(uint32_t now_ms,
                      uint16_t coins,
                      uint8_t seeds);

#endif /* FARM_SHOP_H */