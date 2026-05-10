#ifndef FARM_HOME_H
#define FARM_HOME_H

#include <stdint.h>

/*
 * Render the farm home scene.
 *
 * Parameters:
 *   sleeping_message - controls which home state is displayed.
 *
 *   0: Draw the normal home scene with the bottom control hint.
 *   1: Draw the sleep transition overlay, including the night fade,
 *      sleeping message, and next-day feedback.
 */
void FarmHome_Render(uint8_t sleeping_message);

#endif /* FARM_HOME_H */