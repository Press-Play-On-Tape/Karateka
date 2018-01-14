#ifndef SFX_H
#define SFX_H


const PROGMEM struct sfx1_data {
  uint8_t fmt;
  uint8_t pattern0[13];
} SFX_ouch = {
  .fmt = ATM_SCORE_FMT_MINIMAL_MONO,
  .pattern0 = {
    ATM_CMD_M_SET_TEMPO(22),
    ATM_CMD_M_SET_VOLUME(110),
    ATM_CMD_M_SLIDE_FREQ_ON(-50),
    ATM_CMD_I_NOTE_E5,

    ATM_CMD_M_DELAY_TICKS(11),
    ATM_CMD_I_STOP,
  },
};

#endif