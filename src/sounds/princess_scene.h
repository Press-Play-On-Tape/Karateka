#ifndef PRINCESS_SCENE_H
#define PRINCESS_SCENE_H
  
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif
  
#ifndef NUM_PATTERNS
#define NUM_PATTERNS(struct_) (ARRAY_SIZE( ((struct_ *)0)->patterns_offset))
#endif
  
#ifndef DEFINE_PATTERN
#define DEFINE_PATTERN(pattern_id, values) const uint8_t pattern_id[] = values;
#endif
  
/* pattern (channel) / bytes = 9*/
#define princess_scene_pattern0_data { \
    ATM_CMD_M_SET_VOLUME(110), \
    ATM_CMD_M_SET_TEMPO(16), \
    ATM_CMD_M_CALL(4), \
    ATM_CMD_M_CALL(5), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(princess_scene_pattern0_array, princess_scene_pattern0_data);
    
/* pattern (channel) / bytes = 3*/
#define princess_scene_pattern1_data { \
    ATM_CMD_M_SET_VOLUME(32), \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(princess_scene_pattern1_array, princess_scene_pattern1_data);
    
/* pattern (channel) / bytes = 1*/
#define princess_scene_pattern2_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(princess_scene_pattern2_array, princess_scene_pattern2_data);
    
/* pattern (channel) / bytes = 1*/
#define princess_scene_pattern3_data { \
    ATM_CMD_I_STOP, \
}
DEFINE_PATTERN(princess_scene_pattern3_array, princess_scene_pattern3_data);
    
/* pattern (tune) / "C00|P00" / bytes = 13*/
#define princess_scene_pattern4_data { \
    ATM_CMD_I_NOTE_A4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_C5, \
    ATM_CMD_M_DELAY_TICKS(13), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_NOTE_E4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_B4, \
    ATM_CMD_M_DELAY_TICKS(13), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(1), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(princess_scene_pattern4_array, princess_scene_pattern4_data);
    
/* pattern (tune) / "C00|P01" / bytes = 17*/
#define princess_scene_pattern5_data { \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_C4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_A4, \
    ATM_CMD_M_DELAY_TICKS(8), \
    ATM_CMD_I_NOTE_OFF, \
    ATM_CMD_M_DELAY_TICKS(3), \
    ATM_CMD_I_NOTE_D4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_A4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_B4, \
    ATM_CMD_M_DELAY_TICKS(2), \
    ATM_CMD_I_NOTE_F5, \
    ATM_CMD_M_DELAY_TICKS(10), \
    ATM_CMD_I_RETURN, \
}
DEFINE_PATTERN(princess_scene_pattern5_array, princess_scene_pattern5_data);
    
const PROGMEM struct princess_scene_score_data {
  uint8_t fmt;
  uint8_t num_patterns;
  uint16_t patterns_offset[6];
  uint8_t num_channels;
  uint8_t start_patterns[4];
  uint8_t princess_scene_pattern0[sizeof(princess_scene_pattern0_array)];
  uint8_t princess_scene_pattern1[sizeof(princess_scene_pattern1_array)];
  uint8_t princess_scene_pattern2[sizeof(princess_scene_pattern2_array)];
  uint8_t princess_scene_pattern3[sizeof(princess_scene_pattern3_array)];
  uint8_t princess_scene_pattern4[sizeof(princess_scene_pattern4_array)];
  uint8_t princess_scene_pattern5[sizeof(princess_scene_pattern5_array)];
} princess_scene = {
  .fmt = ATM_SCORE_FMT_FULL,
  .num_patterns = NUM_PATTERNS(struct princess_scene_score_data),
  .patterns_offset = {
      offsetof(struct princess_scene_score_data, princess_scene_pattern0),
      offsetof(struct princess_scene_score_data, princess_scene_pattern1),
      offsetof(struct princess_scene_score_data, princess_scene_pattern2),
      offsetof(struct princess_scene_score_data, princess_scene_pattern3),
      offsetof(struct princess_scene_score_data, princess_scene_pattern4),
      offsetof(struct princess_scene_score_data, princess_scene_pattern5),
  },
  .num_channels = 4,
  .start_patterns = {
    0x00,                         // Channel 0 entry pattern (SQUARE)
    0x01,                         // Channel 1 entry pattern (SQUARE)
    0x02,                         // Channel 2 entry pattern (SQUARE)
    0x03,                         // Channel 3 entry pattern (NOISE)
  },
  .princess_scene_pattern0 = princess_scene_pattern0_data,
  .princess_scene_pattern1 = princess_scene_pattern1_data,
  .princess_scene_pattern2 = princess_scene_pattern2_data,
  .princess_scene_pattern3 = princess_scene_pattern3_data,
  .princess_scene_pattern4 = princess_scene_pattern4_data,
  .princess_scene_pattern5 = princess_scene_pattern5_data,
};

#endif