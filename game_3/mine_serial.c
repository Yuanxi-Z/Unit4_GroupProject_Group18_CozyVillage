#include "mine_serial.h"
#include "mine_map.h"
#include "mine_player.h"
#include "mine_inventory.h"
#include "mine_bat.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

// =============================================================================
// Configuration
// -----------------------------------------------------------------------------
// 1 -> use ANSI escape codes for colour + cursor control (PuTTY, Tera Term,
//      VSCode Serial Monitor with colour mode, most Linux terminals)
// 0 -> plain ASCII only (universal but plainer)
//
// If your terminal shows "[31m" instead of red text, flip this to 0.
// =============================================================================
#define USE_COLOR  1

// =============================================================================
// ANSI helpers
// =============================================================================
#if USE_COLOR
  #define ANSI_RESET      "\x1b[0m"
  #define ANSI_BOLD       "\x1b[1m"
  #define ANSI_DIM        "\x1b[2m"
  #define ANSI_BLINK      "\x1b[5m"

  #define FG_BLACK        "\x1b[30m"
  #define FG_RED          "\x1b[31m"
  #define FG_GREEN        "\x1b[32m"
  #define FG_YELLOW       "\x1b[33m"
  #define FG_BLUE         "\x1b[34m"
  #define FG_MAGENTA      "\x1b[35m"
  #define FG_CYAN         "\x1b[36m"
  #define FG_WHITE        "\x1b[37m"

  // Bright variants (warmer, pop better on dark terminals)
  #define FG_BR_RED       "\x1b[91m"
  #define FG_BR_GREEN     "\x1b[92m"
  #define FG_BR_YELLOW    "\x1b[93m"
  #define FG_BR_BLUE      "\x1b[94m"
  #define FG_BR_MAGENTA   "\x1b[95m"
  #define FG_BR_CYAN      "\x1b[96m"
  #define FG_BR_WHITE     "\x1b[97m"

  // Clear screen + home cursor
  #define CLS             "\x1b[2J\x1b[H"
#else
  #define ANSI_RESET      ""
  #define ANSI_BOLD       ""
  #define ANSI_DIM        ""
  #define ANSI_BLINK      ""
  #define FG_BLACK        ""
  #define FG_RED          ""
  #define FG_GREEN        ""
  #define FG_YELLOW       ""
  #define FG_BLUE         ""
  #define FG_MAGENTA      ""
  #define FG_CYAN         ""
  #define FG_WHITE        ""
  #define FG_BR_RED       ""
  #define FG_BR_GREEN     ""
  #define FG_BR_YELLOW    ""
  #define FG_BR_BLUE      ""
  #define FG_BR_MAGENTA   ""
  #define FG_BR_CYAN      ""
  #define FG_BR_WHITE     ""
  // Two newlines as a cheap "scroll separator" when colour is off
  #define CLS             "\r\n\r\n"
#endif

// =============================================================================
// Timestamp helper: format HAL_GetTick() as HH:MM:SS into a static buffer.
// =============================================================================
static const char* fmt_timestamp(void) {
    static char buf[10];
    uint32_t ms = HAL_GetTick();
    uint32_t s  = ms / 1000u;
    uint32_t mm = (s / 60u) % 60u;
    uint32_t ss = s % 60u;
    uint32_t hh = s / 3600u;
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu",
             (unsigned long)hh, (unsigned long)mm, (unsigned long)ss);
    return buf;
}

// =============================================================================
// serial_init - boot banner
// =============================================================================
void serial_init(void) {
    printf(CLS);
    printf(FG_BR_YELLOW ANSI_BOLD);
    printf("========================================\r\n");
    printf("  COZY VILLAGE  -  Mine Debug Terminal  \r\n");
    printf("========================================\r\n");
    printf(ANSI_RESET);
    printf(FG_BR_CYAN   "  Build: %s %s\r\n", __DATE__, __TIME__);
    printf(FG_WHITE     "  Transport: USART2 @ 115200, 8N1\r\n");
    printf(FG_WHITE     "  Log format: [HH:MM:SS] TAG  detail\r\n");
    printf(ANSI_RESET   "\r\n");
}

// =============================================================================
// Event loggers
// =============================================================================
static void log_line(const char* colour, const char* tag, const char* detail) {
    printf("[%s]  %s%-8s" ANSI_RESET "  %s\r\n",
           fmt_timestamp(), colour, tag, detail);
}

void serial_log_event(const char* tag, const char* detail) {
    log_line(FG_BR_CYAN, tag, detail);
}

void serial_log_warn(const char* tag, const char* detail) {
    log_line(FG_BR_RED ANSI_BOLD, tag, detail);
}

void serial_log_ok(const char* tag, const char* detail) {
    log_line(FG_BR_GREEN, tag, detail);
}

// =============================================================================
// Map tile -> coloured character lookup
// -----------------------------------------------------------------------------
// Each call returns a zero-terminated string containing:
//   colour escape + character + reset escape
// Plus a trailing space, so tiles render nicely in a grid.
// =============================================================================
static const char* tile_to_glyph(uint8_t tile, int is_player, int is_bat) {
    if (is_player) return FG_BR_YELLOW ANSI_BOLD "@" ANSI_RESET " ";
    if (is_bat)    return FG_BR_MAGENTA ANSI_BOLD "b" ANSI_RESET " ";

    switch (tile) {
        case TILE_WALL:         return FG_WHITE ANSI_DIM "#" ANSI_RESET " ";
        case TILE_FLOOR:        return FG_BLACK ANSI_DIM "." ANSI_RESET " ";
        case TILE_ORE_CU:       return FG_YELLOW "c" ANSI_RESET " ";
        case TILE_ORE_AU:       return FG_BR_YELLOW "g" ANSI_RESET " ";
        case TILE_GEODE:        return FG_BR_CYAN ANSI_BOLD "G" ANSI_RESET " ";
        case TILE_EXIT:         return FG_BR_GREEN ANSI_BOLD ">" ANSI_RESET " ";
        case TILE_ITEM_DIAMOND: return FG_BR_CYAN ANSI_BOLD "*" ANSI_RESET " ";
        default:                return " . ";
    }
}

// =============================================================================
// serial_render_frame - clear screen + draw boxed header + map + status
// =============================================================================
void serial_render_frame(void) {
    printf(CLS);

    // -------- Header bar --------
    printf(FG_BR_YELLOW ANSI_BOLD);
    printf("+=============================================+\r\n");
    printf("|  COZY VILLAGE - MINE  floor=%d  time=%s  |\r\n",
           (int)player.depth, fmt_timestamp());
    printf("+=============================================+\r\n");
    printf(ANSI_RESET);

    // -------- Map --------
    printf(FG_WHITE "\r\n" ANSI_RESET);
    for (int y = 0; y < MAP_H; y++) {
        printf("    ");   // left margin
        for (int x = 0; x < MAP_W; x++) {
            uint8_t tile = map_get((int8_t)x, (int8_t)y);
            int is_player = (x == player.x && y == player.y);
            int is_bat    = bat_at((int8_t)x, (int8_t)y);
            fputs(tile_to_glyph(tile, is_player, is_bat), stdout);
        }
        printf("\r\n");
    }

    // -------- Legend --------
    printf("\r\n");
    printf(FG_WHITE ANSI_DIM
           "    Legend: "
           ANSI_RESET);
    printf(FG_BR_YELLOW ANSI_BOLD "@" ANSI_RESET FG_WHITE "=you  "    ANSI_RESET);
    printf(FG_WHITE     ANSI_DIM  "#" ANSI_RESET FG_WHITE "=wall  "  ANSI_RESET);
    printf(FG_YELLOW              "c" ANSI_RESET FG_WHITE "=copper  " ANSI_RESET);
    printf(FG_BR_YELLOW           "g" ANSI_RESET FG_WHITE "=gold  "   ANSI_RESET);
    printf(FG_BR_CYAN   ANSI_BOLD "G" ANSI_RESET FG_WHITE "=geode  "  ANSI_RESET);
    printf(FG_BR_GREEN  ANSI_BOLD ">" ANSI_RESET FG_WHITE "=stairs  " ANSI_RESET);
    printf(FG_BR_MAGENTA ANSI_BOLD"b" ANSI_RESET FG_WHITE "=bat"      ANSI_RESET);
    printf("\r\n\r\n");

    // -------- Status panel --------
    printf(FG_BR_YELLOW ANSI_BOLD
           "+-------- STATUS --------+\r\n"
           ANSI_RESET);

    // TORCH bar: 14 cells, visually fills
    int torch_cells = ((int)player.torch_ticks * 14) / 60;
    if (torch_cells > 14) torch_cells = 14;
    if (torch_cells < 0)  torch_cells = 0;

    const char* torch_col = (player.torch_ticks > 30) ? FG_BR_GREEN
                          : (player.torch_ticks > 10) ? FG_BR_YELLOW
                          : FG_BR_RED;

    printf(FG_WHITE "  TORCH  " ANSI_RESET "%s[", torch_col);
    for (int i = 0; i < 14; i++) {
        if (i < torch_cells) fputs("#", stdout);
        else                 fputs(" ", stdout);
    }
    printf("]" ANSI_RESET " %d/60\r\n", (int)player.torch_ticks);

    printf(FG_WHITE "  ORES   " ANSI_RESET);
    printf(FG_YELLOW    "Cu=%d  " ANSI_RESET, ores.copper);
    printf(FG_BR_YELLOW "Au=%d  " ANSI_RESET, ores.gold);
    printf(FG_BR_CYAN   "Di=%d"   ANSI_RESET, ores.diamond);
    printf("\r\n");

    printf(FG_WHITE "  ITEMS  " ANSI_RESET);
    printf("torch=%d  dyn=%d  potion=%d\r\n",
           items.torch, items.dynamite, items.potion);

    printf(FG_WHITE "  COINS  " ANSI_RESET);
    printf(FG_BR_YELLOW ANSI_BOLD "%d c" ANSI_RESET, coins);

    if (player.is_poisoned) {
        printf("    " FG_BR_MAGENTA ANSI_BOLD ANSI_BLINK
               "[POISONED]" ANSI_RESET);
    }
    printf("\r\n");

    printf(FG_BR_YELLOW ANSI_BOLD
           "+------------------------+\r\n"
           ANSI_RESET);

    // Force the UART buffer to drain before returning - prevents tearing if
    // the next frame starts drawing over a half-sent previous frame.
    fflush(stdout);
}
