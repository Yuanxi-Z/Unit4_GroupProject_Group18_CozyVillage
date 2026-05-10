#ifndef MINE_HOME_H
#define MINE_HOME_H

#include <stdint.h>

// =============================================================================
// Home - cozy rescue scene after fainting.
// -----------------------------------------------------------------------------
// Three-phase presentation:
//   Phase A (0-2500ms):    Black overlay + "YOU FAINTED!" + stats
//   Phase B (2500-3000ms): Fade from black to home
//   Phase C (3000ms+):     Full home scene + "Rest well, little miner..."
// Phase timing is based on g_faint_start_tick (set by Game_3.c on faint).
// =============================================================================

void home_render(uint32_t now_ms);

#endif // MINE_HOME_H