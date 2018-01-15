#include "Sound.h"


#include "src/Arduboy2/Arduboy2Audio.h"
static Arduboy2Audio audio;

#include "src/ATMLib2/ATMlib.h"

// Todo change to <ATMLib.h> once we publish
#include "src/sounds/SFX.h"
#include "src/sounds/song_01_enter_arena.h"
#include "src/sounds/song_02_fight.h"

uint8_t current_song = -1;

struct atm_sfx_state sfx_state;
void Sound::init() {
  // Initialize audio system
  audio.on();

  // Initialize ATMLib2
  atm_synth_setup();
}


void Sound::play_sound(uint8_t id) {
  static const PROGMEM uint8_t *const sounds[] = {
    (uint8_t *)&SFX_ouch,
    (uint8_t *)&SFX_step
  };

  atm_synth_stop_sfx_track(&sfx_state);
  atm_synth_play_sfx_track(OSC_CH_ONE, pgm_read_word(&sounds[id]), &sfx_state);
}


// Shut down audio
void Sound::stfu() {
  current_song = -1;
  atm_synth_stop_score();
}

void Sound::play_score(uint8_t id) {
  static const PROGMEM uint8_t *const songs[] = {
    (uint8_t *)&song_01_enter_arena, 
    (uint8_t *)&song_02_fight,   
  };

  if (current_song == id) {
    return;
  }

  atm_synth_play_score(pgm_read_word(&songs[id]));
  current_song = id;
}

