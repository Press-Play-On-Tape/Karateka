#ifndef SONG_01_ENTER_ARENA_H
#define SONG_01_ENTER_ARENA_H
  
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif
  
#ifndef NUM_PATTERNS
#define NUM_PATTERNS(struct_) (ARRAY_SIZE( ((struct_ *)0)->patterns_offset))
#endif
  
#ifndef DEFINE_PATTERN
#define DEFINE_PATTERN(pattern_id, values) const uint8_t pattern_id[] = values;
#endif
  
/* pattern (channel) / bytes = 11*/
#define song_01_enter_arena_pattern0_data { \
    ATM_CMD_M_SET_VOLUME(110), \
    ATM_CMD_M_SET_TEMPO(18), \
    ATM_CMD_M_CALL(4), \
    ATM_CMD_M_CALL(5), \
    ATM_CMD_M_CALL(6), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern0_array, song_01_enter_arena_pattern0_data);
    
/* pattern (channel) / bytes = 3*/
#define song_01_enter_arena_pattern1_data { \
    ATM_CMD_M_SET_VOLUME(32), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern1_array, song_01_enter_arena_pattern1_data);
    
/* pattern (channel) / bytes = 3*/
#define song_01_enter_arena_pattern2_data { \
    ATM_CMD_M_SET_VOLUME(32), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern2_array, song_01_enter_arena_pattern2_data);
    
/* pattern (channel) / bytes = 3*/
#define song_01_enter_arena_pattern3_data { \
    ATM_CMD_M_SET_VOLUME(32), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern3_array, song_01_enter_arena_pattern3_data);
    
/* pattern (tune) / "C00|P00" / bytes = 17*/
#define song_01_enter_arena_pattern4_data { \
    ATM_CMD_I_NOTE_E4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_E4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_A4, \
    ATM_CMD_M_DELAY_TICKS(12), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(8), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern4_array, song_01_enter_arena_pattern4_data);
    
/* pattern (tune) / "C00|P01" / bytes = 19*/
#define song_01_enter_arena_pattern5_data { \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_A4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_B4, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_NOTE_F5, \
    ATM_CMD_M_DELAY_TICKS(12), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_E3, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_E4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_F4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_B4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern5_array, song_01_enter_arena_pattern5_data);
    
/* pattern (tune) / "C00|P02" / bytes = 15*/
#define song_01_enter_arena_pattern6_data { \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_G4, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_B4, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_E5, \
    ATM_CMD_M_DELAY_TICKS(18), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(4), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(song_01_enter_arena_pattern6_array, song_01_enter_arena_pattern6_data);
    
const PROGMEM struct song_01_enter_arena_score_data {
  uint8_t fmt;
  uint8_t num_patterns;
  uint16_t patterns_offset[7];
  uint8_t num_channels;
  uint8_t start_patterns[4];
  uint8_t song_01_enter_arena_pattern0[sizeof(song_01_enter_arena_pattern0_array)];
  uint8_t song_01_enter_arena_pattern1[sizeof(song_01_enter_arena_pattern1_array)];
  uint8_t song_01_enter_arena_pattern2[sizeof(song_01_enter_arena_pattern2_array)];
  uint8_t song_01_enter_arena_pattern3[sizeof(song_01_enter_arena_pattern3_array)];
  uint8_t song_01_enter_arena_pattern4[sizeof(song_01_enter_arena_pattern4_array)];
  uint8_t song_01_enter_arena_pattern5[sizeof(song_01_enter_arena_pattern5_array)];
  uint8_t song_01_enter_arena_pattern6[sizeof(song_01_enter_arena_pattern6_array)];
} song_01_enter_arena = {
  .fmt = ATM_SCORE_FMT_FULL,
  .num_patterns = NUM_PATTERNS(struct song_01_enter_arena_score_data),
  .patterns_offset = {
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern0),
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern1),
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern2),
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern3),
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern4),
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern5),
      offsetof(struct song_01_enter_arena_score_data, song_01_enter_arena_pattern6),
  },
  .num_channels = 4,
  .start_patterns = {
    0x00,                         // Channel 0 entry pattern (SQUARE)
    0x01,                         // Channel 1 entry pattern (SQUARE)
    0x02,                         // Channel 2 entry pattern (SQUARE)
    0x03,                         // Channel 3 entry pattern (NOISE)
  },
  .song_01_enter_arena_pattern0 = song_01_enter_arena_pattern0_data,
  .song_01_enter_arena_pattern1 = song_01_enter_arena_pattern1_data,
  .song_01_enter_arena_pattern2 = song_01_enter_arena_pattern2_data,
  .song_01_enter_arena_pattern3 = song_01_enter_arena_pattern3_data,
  .song_01_enter_arena_pattern4 = song_01_enter_arena_pattern4_data,
  .song_01_enter_arena_pattern5 = song_01_enter_arena_pattern5_data,
  .song_01_enter_arena_pattern6 = song_01_enter_arena_pattern6_data,
};

#endif