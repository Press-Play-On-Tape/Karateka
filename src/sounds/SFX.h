#ifndef SFX_H
#define SFX_H


const PROGMEM struct SFX_ouch_data {
  uint8_t fmt;
  uint8_t pattern0[11];
} SFX_ouch = {
  .fmt = ATM_SCORE_FMT_MINIMAL_MONO,
  .pattern0 = {
    ATM_CMD_M_SET_TEMPO(22),
    ATM_CMD_M_SET_VOLUME(128),
    ATM_CMD_I_NOTE_E3,
    ATM_CMD_M_DELAY_TICKS(2),
    ATM_CMD_I_NOTE_C3,
    ATM_CMD_M_DELAY_TICKS(1),    
    ATM_CMD_I_STOP,
  },
};

const PROGMEM struct SFX_step_data {
  uint8_t fmt;
  uint8_t pattern0[8];
} SFX_step = {
  .fmt = ATM_SCORE_FMT_MINIMAL_MONO,
  .pattern0 = {
    ATM_CMD_M_SET_TEMPO(22),
    ATM_CMD_M_SET_VOLUME(128),
    ATM_CMD_I_NOTE_C5,
    ATM_CMD_M_DELAY_TICKS(1),
    ATM_CMD_I_STOP,
  },
};


#endif