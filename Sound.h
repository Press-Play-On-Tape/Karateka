#ifndef SOUND_H
#define SOUND_H

// @dxb @filmote why is this required? If i don't include Arduino.h, i get errors like:
// sketch/Sound.h:8:7: error: 'uint8_t' does not name a type
// const uint8_t SFX_OUCH = 0;
//       ^
#include <Arduino.h>

const uint8_t SFX_OUCH = 0;


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
