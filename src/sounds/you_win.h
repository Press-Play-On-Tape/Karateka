#ifndef YOU_WIN_H
#define YOU_WIN_H
  
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif
  
#ifndef NUM_PATTERNS
#define NUM_PATTERNS(struct_) (ARRAY_SIZE( ((struct_ *)0)->patterns_offset))
#endif
  
#ifndef DEFINE_PATTERN
#define DEFINE_PATTERN(pattern_id, values) const uint8_t pattern_id[] = values;
#endif
  
/* pattern (channel) / bytes = 7*/
#define you_win_pattern0_data { \
    ATM_CMD_M_SET_VOLUME(110), \
    ATM_CMD_M_SET_TEMPO(14), \
    ATM_CMD_M_CALL(4), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(you_win_pattern0_array, you_win_pattern0_data);
    
/* pattern (channel) / bytes = 1*/
#define you_win_pattern1_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(you_win_pattern1_array, you_win_pattern1_data);
    
/* pattern (channel) / bytes = 1*/
#define you_win_pattern2_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(you_win_pattern2_array, you_win_pattern2_data);
    
/* pattern (channel) / bytes = 1*/
#define you_win_pattern3_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(you_win_pattern3_array, you_win_pattern3_data);
    
/* pattern (tune) / "C00|P00" / bytes = 25*/
#define you_win_pattern4_data { \
    ATM_CMD_I_NOTE_E4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_E4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_A4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C5_, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_C5_, \
    ATM_CMD_M_DELAY_TICKS(12), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(you_win_pattern4_array, you_win_pattern4_data);
    
const PROGMEM struct you_win_score_data {
  uint8_t fmt;
  uint8_t num_patterns;
  uint16_t patterns_offset[5];
  uint8_t num_channels;
  uint8_t start_patterns[4];
  uint8_t you_win_pattern0[sizeof(you_win_pattern0_array)];
  uint8_t you_win_pattern1[sizeof(you_win_pattern1_array)];
  uint8_t you_win_pattern2[sizeof(you_win_pattern2_array)];
  uint8_t you_win_pattern3[sizeof(you_win_pattern3_array)];
  uint8_t you_win_pattern4[sizeof(you_win_pattern4_array)];
} you_win = {
  .fmt = ATM_SCORE_FMT_FULL,
  .num_patterns = NUM_PATTERNS(struct you_win_score_data),
  .patterns_offset = {
      offsetof(struct you_win_score_data, you_win_pattern0),
      offsetof(struct you_win_score_data, you_win_pattern1),
      offsetof(struct you_win_score_data, you_win_pattern2),
      offsetof(struct you_win_score_data, you_win_pattern3),
      offsetof(struct you_win_score_data, you_win_pattern4),
  },
  .num_channels = 4,
  .start_patterns = {
    0x00,                         // Channel 0 entry pattern (SQUARE)
    0x01,                         // Channel 1 entry pattern (SQUARE)
    0x02,                         // Channel 2 entry pattern (SQUARE)
    0x03,                         // Channel 3 entry pattern (NOISE)
  },
  .you_win_pattern0 = you_win_pattern0_data,
  .you_win_pattern1 = you_win_pattern1_data,
  .you_win_pattern2 = you_win_pattern2_data,
  .you_win_pattern3 = you_win_pattern3_data,
  .you_win_pattern4 = you_win_pattern4_data,
};

#endif