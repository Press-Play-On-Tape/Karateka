#ifndef SOUND_H
#define SOUND_H


#include <stdint.h>
const uint8_t SFX_OUCH = 0;
static const uint8_t SFX_STEP = 1;

// SONGS
const uint8_t INTRO_SONG = 0;


class Sound {
public:
  static void init();
  static void play_sound(uint8_t id);
  static void stfu();
  static void play_score(uint8_t id);
};
#endif
