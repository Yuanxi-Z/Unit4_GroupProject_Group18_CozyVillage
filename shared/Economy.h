#ifndef SHARED_ECONOMY_H
#define SHARED_ECONOMY_H

#include <stdint.h>
#include <stdbool.h>

/* ==========================================================
 * Global Economy System API
 * ==========================================================
 * Description: All mini-games (Game 1, Game 2, Game 3) must use 
 * these interfaces to manage gold. Directly modifying the global 
 * variable is strictly prohibited!
 * ========================================================== */

// Get the player's current total global gold
int Get_Total_Gold(void);

// Add gold (e.g., call when catching a fish or mining an ore)
void Add_Gold(int amount);

// Spend gold (e.g., call when buying items in a shop)
// Returns: true if purchase is successful and deducted, false if balance is insufficient
bool Spend_Gold(int amount);

#endif /* SHARED_ECONOMY_H */