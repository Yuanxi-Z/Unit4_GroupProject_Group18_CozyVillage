#ifndef FISH_COLLECTION_H
#define FISH_COLLECTION_H

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief Fish Collection Book
 * Manages the player's catch history and renders the collection UI.
 */

// Increments the catch counter for a specific fish type
void Fish_Collection_Add(int fish_type);

// Renders the main collection book interface
void Fish_Collection_Draw_UI(uint32_t tick);

#endif // FISH_COLLECTION_H