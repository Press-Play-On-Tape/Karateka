#ifndef SONG_02_FIGHT_H
#define SONG_02_FIGHT_H
  
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif
  
#ifndef NUM_PATTERNS
#define NUM_PATTERNS(struct_) (ARRAY_SIZE( ((struct_ *)0)->patterns_offset))
#endif
  
#ifndef DEFINE_PATTERN
#define DEFINE_PATTERN(pattern_id, values) const uint8_t pattern_id[] = values;
#endif
  
/* pattern (channel) / bytes = 13*/
#define song_02_fight_pattern0_data { \
    ATM_CMD_M_SET_VOLUME(90), \
    ATM_CMD_M_SET_TEMPO(19), \
    ATM_CMD_M_CALL(4), \
    ATM_CMD_M_CALL(5), \
    ATM_CMD_M_CALL(6), \
    ATM_CMD_M_CALL(7), \
    ATM_CMD_M_SET_LOOP_PATTERN(0), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_02_fight_pattern0_array, song_02_fight_pattern0_data);
    
/* pattern (channel) / bytes = 1*/
#define song_02_fight_pattern1_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_02_fight_pattern1_array, song_02_fight_pattern1_data);
    
/* pattern (channel) / bytes = 1*/
#define song_02_fight_pattern2_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_02_fight_pattern2_array, song_02_fight_pattern2_data);
    
/* pattern (channel) / bytes = 1*/
#define song_02_fight_pattern3_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_02_fight_pattern3_array, song_02_fight_pattern3_data);
    
/* pattern (tune) / "C00|P00" / bytes = 21*/
#define song_02_fight_pattern4_data { \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_02_fight_pattern4_array, song_02_fight_pattern4_data);
    
/* pattern (tune) / "C00|P01" / bytes = 19*/
#define song_02_fight_pattern5_data { \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_02_fight_pattern5_array, song_02_fight_pattern5_data);
    
/* pattern (tune) / "C00|P02" / bytes = 21*/
#define song_02_fight_pattern6_data { \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_F4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_F4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_02_fight_pattern6_array, song_02_fight_pattern6_data);
    
/* pattern (tune) / "C00|P03" / bytes = 19*/
#define song_02_fight_pattern7_data { \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_F4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_F4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4_, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_02_fight_pattern7_array, song_02_fight_pattern7_data);
    
const PROGMEM struct song_02_fight_score_data {
  uint8_t fmt;
  uint8_t num_patterns;
  uint16_t patterns_offset[8];
  uint8_t num_channels;
  uint8_t start_patterns[4];
  uint8_t song_02_fight_pattern0[sizeof(song_02_fight_pattern0_array)];
  uint8_t song_02_fight_pattern1[sizeof(song_02_fight_pattern1_array)];
  uint8_t song_02_fight_pattern2[sizeof(song_02_fight_pattern2_array)];
  uint8_t song_02_fight_pattern3[sizeof(song_02_fight_pattern3_array)];
  uint8_t song_02_fight_pattern4[sizeof(song_02_fight_pattern4_array)];
  uint8_t song_02_fight_pattern5[sizeof(song_02_fight_pattern5_array)];
  uint8_t song_02_fight_pattern6[sizeof(song_02_fight_pattern6_array)];
  uint8_t song_02_fight_pattern7[sizeof(song_02_fight_pattern7_array)];
} song_02_fight = {
  .fmt = ATM_SCORE_FMT_FULL,
  .num_patterns = NUM_PATTERNS(struct song_02_fight_score_data),
  .patterns_offset = {
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern0),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern1),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern2),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern3),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern4),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern5),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern6),
      offsetof(struct song_02_fight_score_data, song_02_fight_pattern7),
  },
  .num_channels = 4,
  .start_patterns = {
    0x00,                         // Channel 0 entry pattern (SQUARE)
    0x01,                         // Channel 1 entry pattern (SQUARE)
    0x02,                         // Channel 2 entry pattern (SQUARE)
    0x03,                         // Channel 3 entry pattern (NOISE)
  },
  .song_02_fight_pattern0 = song_02_fight_pattern0_data,
  .song_02_fight_pattern1 = song_02_fight_pattern1_data,
  .song_02_fight_pattern2 = song_02_fight_pattern2_data,
  .song_02_fight_pattern3 = song_02_fight_pattern3_data,
  .song_02_fight_pattern4 = song_02_fight_pattern4_data,
  .song_02_fight_pattern5 = song_02_fight_pattern5_data,
  .song_02_fight_pattern6 = song_02_fight_pattern6_data,
  .song_02_fight_pattern7 = song_02_fight_pattern7_data,
};

#endif