#ifndef MINE_SHOP_H
#define MINE_SHOP_H

#include <stdint.h>
#include "Joystick.h"

typedef enum {
    SHOP_RESULT_STAY,
    SHOP_RESULT_LEAVE
} ShopResult;

void shop_init(void);
ShopResult shop_update(uint8_t bt2, uint8_t bt3, Direction dir, uint32_t now_ms);
void shop_render(uint32_t now_ms);

#endif // MINE_SHOP_H