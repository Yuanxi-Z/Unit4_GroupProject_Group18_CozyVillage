#ifndef MINE_SERIAL_H
#define MINE_SERIAL_H

#include <stdint.h>

// =============================================================================
// mine_serial - UART terminal output (debug logger + ASCII map)
// -----------------------------------------------------------------------------
// Turns the PC's serial terminal (PuTTY, Tera Term, VSCode Serial Monitor)
// into a secondary display running in parallel with the LCD.
//
// Transport: stdio printf -> _write() -> HAL_UART_Transmit on USART2 (already
// set up in main.c). No extra drivers needed.
//
// Two kinds of output:
//   1. Event log: one line per game event with timestamp + metric
//        [00:04:12]  MINE      copper at (2,1)  -> Cu=3 Au=1 Di=0
//        [00:04:24]  POISON    bat at (5,3) -> is_poisoned=1
//        [00:04:30]  POTION    used, potions left=1
//
//   2. Full-screen refresh: ASCII map + status panel, redrawn on demand
//      Looks like a retro roguelike terminal when viewed in PuTTY/Tera Term.
//
// Colour is controlled by USE_COLOR in mine_serial.c. Set to 0 if your
// terminal does not understand ANSI escape codes (you'll see raw "\x1b[31m"
// garbage in that case, flip the flag and rebuild).
// =============================================================================

// Initialise - prints a header/banner on boot so you know UART is alive.
void serial_init(void);

// -----------------------------------------------------------------------------
// Event logger - one line per call, timestamped
// -----------------------------------------------------------------------------

// Log an exploration event (e.g. picking up ore). `tag` is a short uppercase
// category (MINE / MOVE / EXIT / ...), `detail` is a free-form description.
void serial_log_event(const char* tag, const char* detail);

// Log a combat event (red-coloured). Used for bat bite / poison / faint.
void serial_log_warn(const char* tag, const char* detail);

// Log a successful action (green-coloured). Used for potion / purchase.
void serial_log_ok(const char* tag, const char* detail);

// -----------------------------------------------------------------------------
// Full screen render - clears the terminal and redraws map + status panel
// Call this on demand (e.g. every N frames, or on state change).
// Heavier than serial_log_*, don't call every frame or you'll flood UART.
// -----------------------------------------------------------------------------
void serial_render_frame(void);

#endif // MINE_SERIAL_H
