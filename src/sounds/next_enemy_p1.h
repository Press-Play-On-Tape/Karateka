#ifndef NEXT_ENEMY_P1MID_H
#define NEXT_ENEMY_P1MID_H
  
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
#define next_enemy_p1mid_pattern0_data { \
    ATM_CMD_M_SET_VOLUME(110), \
    ATM_CMD_M_SET_TEMPO(16), \
    ATM_CMD_M_CALL(4), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(next_enemy_p1mid_pattern0_array, next_enemy_p1mid_pattern0_data);
    
/* pattern (channel) / bytes = 1*/
#define next_enemy_p1mid_pattern1_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(next_enemy_p1mid_pattern1_array, next_enemy_p1mid_pattern1_data);
    
/* pattern (channel) / bytes = 1*/
#define next_enemy_p1mid_pattern2_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(next_enemy_p1mid_pattern2_array, next_enemy_p1mid_pattern2_data);
    
/* pattern (channel) / bytes = 1*/
#define next_enemy_p1mid_pattern3_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(next_enemy_p1mid_pattern3_array, next_enemy_p1mid_pattern3_data);
    
/* pattern (tune) / "C00|P00" / bytes = 11*/
#define next_enemy_p1mid_pattern4_data { \
    ATM_CMD_I_NOTE_D3_, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_E3, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_F3, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_E3, \
    ATM_CMD_M_DELAY_TICKS(13), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(next_enemy_p1mid_pattern4_array, next_enemy_p1mid_pattern4_data);
    
const PROGMEM struct next_enemy_p1mid_score_data {
  uint8_t fmt;
  uint8_t num_patterns;
  uint16_t patterns_offset[5];
  uint8_t num_channels;
  uint8_t start_patterns[4];
  uint8_t next_enemy_p1mid_pattern0[sizeof(next_enemy_p1mid_pattern0_array)];
  uint8_t next_enemy_p1mid_pattern1[sizeof(next_enemy_p1mid_pattern1_array)];
  uint8_t next_enemy_p1mid_pattern2[sizeof(next_enemy_p1mid_pattern2_array)];
  uint8_t next_enemy_p1mid_pattern3[sizeof(next_enemy_p1mid_pattern3_array)];
  uint8_t next_enemy_p1mid_pattern4[sizeof(next_enemy_p1mid_pattern4_array)];
} next_enemy_p1mid = {
  .fmt = ATM_SCORE_FMT_FULL,
  .num_patterns = NUM_PATTERNS(struct next_enemy_p1mid_score_data),
  .patterns_offset = {
      offsetof(struct next_enemy_p1mid_score_data, next_enemy_p1mid_pattern0),
      offsetof(struct next_enemy_p1mid_score_data, next_enemy_p1mid_pattern1),
      offsetof(struct next_enemy_p1mid_score_data, next_enemy_p1mid_pattern2),
      offsetof(struct next_enemy_p1mid_score_data, next_enemy_p1mid_pattern3),
      offsetof(struct next_enemy_p1mid_score_data, next_enemy_p1mid_pattern4),
  },
  .num_channels = 4,
  .start_patterns = {
    0x00,                         // Channel 0 entry pattern (SQUARE)
    0x01,                         // Channel 1 entry pattern (SQUARE)
    0x02,                         // Channel 2 entry pattern (SQUARE)
    0x03,                         // Channel 3 entry pattern (NOISE)
  },
  .next_enemy_p1mid_pattern0 = next_enemy_p1mid_pattern0_data,
  .next_enemy_p1mid_pattern1 = next_enemy_p1mid_pattern1_data,
  .next_enemy_p1mid_pattern2 = next_enemy_p1mid_pattern2_data,
  .next_enemy_p1mid_pattern3 = next_enemy_p1mid_pattern3_data,
  .next_enemy_p1mid_pattern4 = next_enemy_p1mid_pattern4_data,
};

#endif