#ifndef FISH_AUDIO_H
#define FISH_AUDIO_H

#include <stdint.h>

/*
 * @brief Fishing Minigame Audio System
 * Manages the PWM buzzer sound effects for various gameplay events.
 */

// Initializes the PWM timer for the buzzer
void Fish_Audio_Init(void);

// Plays a sliding pitch effect when the rod is physically cast
void Fish_Audio_Play_Cast(void);

// High-pitched alert when a fish bites
void Fish_Audio_Play_Bite(void);

// Dynamic feedback sound that changes pitch based on the current line tension
void Fish_Audio_Play_Reeling(int tension, uint32_t tick);

// Victory jingle played when a fish is successfully caught
void Fish_Audio_Play_Success(void);

// Low-pitch failure tone played when a fish escapes
void Fish_Audio_Play_Fail(void);

#endif // FISH_AUDIO_H