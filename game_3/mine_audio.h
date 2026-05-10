#ifndef MINE_AUDIO_H
#define MINE_AUDIO_H

#include <stdint.h>

void audio_init(void);
void audio_update(uint32_t now);           // call every frame — handles auto-off + LED

// Sound triggers (non-blocking, set timer then auto-off)
void audio_ping(int distance);             // metal detector: closer = faster ping
void audio_pickup_copper(void);
void audio_pickup_gold(void);
void audio_pickup_diamond(void);           // 3-note rising melody
void audio_mine_wall(void);                // low thud
void audio_explode(void);                  // dynamite boom
void audio_faint(void);                    // sad low tone
void audio_buy(void);                      // shop purchase chime

// LED torch control
void audio_torch_led(int16_t torch_ticks); // brightness + breathing flash

#endif
