#include "src/utils/Arduboy2Ext.h"
#include "src/utils/Stack.h"
#include "src/utils/PlayerStances.h"
#include "src/utils/EnemyStances.h"
#include "src/images/images.h"
#include "Enums.h"

#ifdef SOUNDS_ARDUBOYTONES
#include <ArduboyTones.h>
#include "src/sounds/Sounds_ArduboyTones.h"
#endif

#ifdef SOUNDS_ATMLIB
#include <ATMlib.h>
#include "src/sounds/Sounds_ATMLib.h"
#endif

Arduboy2Ext arduboy;

#ifdef SOUNDS_ARDUBOYTONES
ArduboyTones sound(arduboy.audio.on);
#endif

#ifdef SOUNDS_ATMLIB
ATMsynth ATM;
#endif

Stack <uint8_t, 30> playerStack;
Stack <uint8_t, 30> enemyStack;

int8_t mainSceneX = 0;
bool displayHealth = false;
bool outdoors = true;

uint8_t enemyHit = 0;
uint8_t playerHit = 0;

uint8_t eagleMode = EAGLE_MODE_NONE;
bool eagleWingUp = false;

uint8_t emperorMode = EMPEROR_MODE_INIT;
uint8_t finalSceneMode = FINAL_SCENE_INACTIVE;

const uint8_t PROGMEM eagle_heights[] = { EAGLE_LEVEL_LOW, EAGLE_LEVEL_MED, EAGLE_LEVEL_HIGH };

#ifdef USE_DIFFERENT_BAMS
const uint8_t *bam_images[] =               { bam1, bam2, bam3 };
const uint8_t *bam_masks[] =                { bam1_mask, bam2_mask, bam3_mask };
#endif

bool outside = true;
GameStateDetails gameStateDetails;

#ifdef DEBUG_HITS
int16_t _distBetween = 0;
int16_t _distTest = 0;
uint8_t _targetStance = 0;
uint8_t _action = 0;
uint16_t _pos = 0;
#endif

Entity player = { STANCE_DEFAULT, 10, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
Entity enemy = { STANCE_DEFAULT, 153, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };

bool enemyImmediateAction = false;
bool enemyImmediateRetreat = false;



// ---------------------------------------------------------------------------------------------------------------
//  Setup
// ---------------------------------------------------------------------------------------------------------------

void setup() {

  arduboy.boot();
  arduboy.safeMode(); 
  arduboy.setFrameRate(23);
  arduboy.initRandomSeed();
  
  #ifdef SOUNDS_ATMLIB
  arduboy.audio.on;
  #endif

  gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

}


// ---------------------------------------------------------------------------------------------------------------
//  Main Loop ..
// ---------------------------------------------------------------------------------------------------------------

void loop() {

  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();

  switch (gameStateDetails.getCurrState()) {

    case GAME_STATE_TITLE_SCENE:

      player = { STANCE_DEFAULT, 10, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
      enemy = { STANCE_DEFAULT, 153, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
      arduboy.clear();
      showScene();
      break;

    case GAME_STATE_FOLLOW_SEQUENCE:
      {
        gameStateDetails.setCurrState(pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) ])); 
        gameStateDetails.delayInterval = pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 1]); 
      
        uint8_t arches = pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 2]);
        gameStateDetails.intArch = (arches <= 2 ? arches : 0); 
        gameStateDetails.extArch = (arches >= 3 ? arches - 2 : 0); 

        if (arches == 0) {

          gameStateDetails.archXPos = 0;
          gameStateDetails.enemyType = pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 3]); 
          
        }
        else {

          gameStateDetails.enemyType = 0;
          if (arches % 2 == 1) {
            gameStateDetails.archXPos = pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 3]);
          }
          else {
            gameStateDetails.archXPos = (uint16_t)WIDTH - (uint16_t)pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 3]);
          }
          
        }

        gameStateDetails.showCrevice = (pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 4]) == 1); 
        gameStateDetails.hasDelay = (gameStateDetails.delayInterval > 0);
        gameStateDetails.activity = pgm_read_byte(&gameSequence[ (gameStateDetails.sequence * GAME_STATE_SEQ_SIZE) + 5]);
        gameStateDetails.sequence++;

      }
      break;
      
    case GAME_STATE_JM_SCENE:
    case GAME_STATE_CASTLE_SCENE:
    case GAME_STATE_THE_END:
    case GAME_STATE_ENEMY_APPROACH:
    case GAME_STATE_PRINCESS:
      arduboy.clear();
      showScene();
      break;

    case GAME_STATE_EMPEROR_INIT:
      playerStack.clear();
      enemyStack.clear();
      emperorMode = EMPEROR_MODE_INIT;

      enemy = { STANCE_DEFAULT, 153, 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, HEALTH_STARTING_POINTS, 0, true, true, false };
      gameStateDetails.setCurrState(GAME_STATE_EMPEROR);
      outside = false;
      break;

    case GAME_STATE_EMPEROR:
      arduboy.clear();
      emperor_loop();
      break;
 
    case GAME_STATE_ENTRANCE_INIT:
      playerStack.clear();
      enemyStack.clear();
      gameStateDetails.setCurrState(GAME_STATE_ENTRANCE);
      player.stance = STANCE_ENTRANCE_1;
      enemy.xPos = 140;
      break;

    case GAME_STATE_ENTRANCE:

      arduboy.clear();
      gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
      player.stance = STANCE_ENTRANCE_1;

      playerStack.push(STANCE_STANDING_UPRIGHT, STANCE_ENTRANCE_6, STANCE_ENTRANCE_5);
      playerStack.push(STANCE_ENTRANCE_4, STANCE_ENTRANCE_3, STANCE_ENTRANCE_2);
      playerStack.push(STANCE_ENTRANCE_1);

      break;

    case GAME_STATE_PLAY_INIT:
      
      gameStateDetails.setCurrState(GAME_STATE_PLAY);
      
      player.xPos = 10;
      player.xPosOverall = 0;
      
      if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {
        if (player.health - 10 < HEALTH_STARTING_POINTS) { player.health = player.health + 10; }
        if (player.health - 10 < HEALTH_STARTING_POINTS) { player.health = player.health + 10; }
      }

      player.regainLimit = (enemy.health < HEALTH_STARTING_POINTS - (2 * HEALTH_UNIT) ? player.health + (2 * HEALTH_UNIT) : HEALTH_STARTING_POINTS);
      enemy = { STANCE_DEFAULT, (gameStateDetails.enemyType == ENEMY_TYPE_EAGLE ? 170 : 140), 0, 0, 55, 0, HEALTH_STARTING_POINTS, 0, 0, true, true, false };
      enemy.health = ((HEALTH_STARTING_POINTS * 2) - player.health  > HEALTH_MAX_POINTS ? HEALTH_MAX_POINTS : (HEALTH_STARTING_POINTS * 2) - player.health);
      enemy.regainLimit = enemy.health;
      eagleMode = EAGLE_MODE_FLY_INIT;
      break;

    case GAME_STATE_PLAY:
      arduboy.clear();
      play_loop();
      break;

    case GAME_STATE_ENEMY_APPROACH_INIT:
      playerStack.clear();
      enemyStack.clear();
      enemy_approach_init();
      break;

    case GAME_STATE_GO_THROUGH_GATE:
      playerStack.clear();
      enemyStack.clear();
      player.xPosOverall = -30;
      gameStateDetails.setCurrState(GAME_STATE_PLAY);
      break;
      
    case GAME_STATE_FINAL_SCENE:
      arduboy.clear();
      finalScene();
      break;

  }

  arduboy.display();
  
}

  
// ---------------------------------------------------------------------------------------------------------------
//  Draw the background ..
// ---------------------------------------------------------------------------------------------------------------

void draw_background() {

  const uint8_t *backdrop_img = (outdoors ? backdrop: backdrop2);

  arduboy.drawFastHLine(0, 47, WIDTH, WHITE);
  
  for (int i = 49; i < 58; i++) {
    drawHorizontalDottedLine(i % 2, WIDTH, i);
  }

  if (gameStateDetails.showCrevice) {
    arduboy.drawCompressedMirror(-player.xPosOverall, 0, crevice_mask, BLACK, false);
    arduboy.drawCompressedMirror(-player.xPosOverall, 0, crevice, WHITE, false);
  }
  
  if (outside) {
    arduboy.drawCompressedMirror(mainSceneX, -2, backdrop_img, WHITE, false);
    arduboy.drawCompressedMirror(mainSceneX + MAIN_SCENE_IMG_WIDTH, -2, backdrop_img, WHITE, false);
    arduboy.drawCompressedMirror(mainSceneX + (2 * MAIN_SCENE_IMG_WIDTH), -2, backdrop_img, WHITE, false);
    arduboy.drawCompressedMirror(mainSceneX + (3 * MAIN_SCENE_IMG_WIDTH), -2, backdrop_img, WHITE, false);
  }
  
  
  // Draw player triangles ..

  if (arduboy.everyXFrames(ANIMATION_FLASHING_TRIANGLES)) displayHealth = !displayHealth;

  if (player.health > 20 || displayHealth) {
    for (uint8_t i = 0; i < (player.health / 10); i++) {
      arduboy.drawCompressedMirror((i * 4), 59, arrow, WHITE, false);
    }
  }


  // Draw enemy triangles ..

  if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {

    if (enemy.health > 20 || displayHealth) {
        
      for (uint8_t i = (enemy.health / 10); i > 0; i--) {
        arduboy.drawCompressedMirror(129 - (i * 4), 59, arrow, WHITE, true);
      }

    }

  }

}


// ---------------------------------------------------------------------------------------------------------------
//  Play Karateka !
// ---------------------------------------------------------------------------------------------------------------

void play_loop() {

  draw_background();
  if (gameStateDetails.hasDelay && gameStateDetails.delayInterval == 0 && playerStack.isEmpty())   { 

    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); 

  }
  else {

    playerMovements();

  }

  switch (gameStateDetails.enemyType) {
  
    case ENEMY_TYPE_PERSON:
      enemyMovements();
      break;
  
    case ENEMY_TYPE_EAGLE:
      eagleMovements();
      break;

  }



  // Has the player or enemy been hit ?
 
  uint8_t player_BamX = 0;
  uint8_t player_BamY = 0;

  uint8_t enemy_BamX = 0;
  uint8_t enemy_BamY = 0;

  #ifdef USE_DIFFERENT_BAMS
  const int8_t player_BamXPos[] = {  32,  31,  33,            33,  33,  34,  34,  29,  29 };
  const int8_t enemy_BamXPos[] =  { -17, -17, -19,           -19, -19, -18, -18, -18, -18 };
  const int8_t both_BamYPos[] =   { -30, -17, -42,           -37, -37, -41, -41, -27, -27 };
  #endif
  //                           Kick   M,   L,   H     Punch   MR,  ML,  HR,  HL,  LR,  LL     
  #ifndef USE_DIFFERENT_BAMS
  const int8_t player_BamXPos[] = {  27,  25,  30,            30,  30,  30,  30,  23,  23 };
  const int8_t enemy_BamXPos[] =  { -20, -23, -19,           -22, -22, -18, -18, -24, -24 };
  const int8_t both_BamYPos[] =   { -33, -21, -42,           -39, -39, -45, -45, -31, -31 };
  #endif

  if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
    
    enemyHit = 0;
    playerHit = 0;

    if (gameStateDetails.hasDelay && gameStateDetails.delayInterval > 0)      { gameStateDetails.delayInterval--; }


    // Update the player and enemy stances from the stack ..

    if (!playerStack.isEmpty()) {
      player.stance = playerStack.pop();
    }
    else {
      player.xPosDelta = 0;
    }
        
    if (!enemyStack.isEmpty()) {
      enemy.stance = enemyStack.pop();
    }
    else {
      enemy.xPosDelta = 0;
    }


    // If we are fighting, check to see if a strike has been made ..

    if (gameStateDetails.enemyType != ENEMY_TYPE_NONE) {

      enemyImmediateAction = false;

      if (player.stance >= STANCE_KICK_MED_END && player.stance <= STANCE_PUNCH_LOW_LH_END) {

        enemyHit = inStrikingRange(getActionFromStance(player.stance), player.xPos, gameStateDetails.enemyType, enemy.stance, enemy.xPos);

      }

      if (enemy.stance >= STANCE_KICK_MED_END && enemy.stance <= STANCE_PUNCH_LOW_LH_END) {
    
        playerHit = inStrikingRange(getActionFromStance(enemy.stance), enemy.xPos, ENEMY_TYPE_PERSON, player.stance, player.xPos);

      }
      if (playerHit > 0 || enemyHit > 0) {


        #ifdef SOUNDS_ARDUBOYTONES
        if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
          sound.tones(ouch);
        }
        #endif 

        #ifdef SOUNDS_ATMLIB
        if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
          ATM.play(ouch);
        }
        #endif 

      }

    }

  }


  // Render the background, acrhes and the players ..

  if (player.xPosDelta != 0) {

    int16_t archXPos = gameStateDetails.archXPos;

    if ((gameStateDetails.extArch == 0 && gameStateDetails.intArch == 0) || 
        (gameStateDetails.extArch == ARCH_RIGHT_HAND && archXPos > 96) ||
        (gameStateDetails.intArch == ARCH_RIGHT_HAND && archXPos > 96) ||
        (gameStateDetails.extArch == ARCH_LEFT_HAND && archXPos < 30) ||
        (gameStateDetails.intArch == ARCH_LEFT_HAND && archXPos < 30)
    ) {

      mainSceneX = mainSceneX + player.xPosDelta;
      player.xPosOverall = player.xPosOverall - player.xPosDelta;
      enemy.xPos = enemy.xPos + player.xPosDelta;
      gameStateDetails.archXPos = archXPos + player.xPosDelta;

      if (mainSceneX < -MAIN_SCENE_IMG_WIDTH) { mainSceneX = 0; }
      if (mainSceneX > 0) { mainSceneX = mainSceneX - MAIN_SCENE_IMG_WIDTH; }

    }
    else {

      player.xPos = player.xPos - player.xPosDelta;
      player.xPosOverall = player.xPosOverall - player.xPosDelta;

    }

  }

  if (gameStateDetails.extArch == ARCH_RIGHT_HAND) {
    arduboy.drawCompressedMirror(gameStateDetails.archXPos, -2, arch_exterior_lh_mask, BLACK, false);
    arduboy.drawCompressedMirror(gameStateDetails.archXPos, -2, arch_exterior_lh, WHITE, false);
  }

  if (gameStateDetails.extArch == ARCH_LEFT_HAND) {
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, -2, arch_exterior_lh_mask, BLACK, true);
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, -2, arch_exterior_lh, WHITE, true);
  }

  if (gameStateDetails.intArch == ARCH_RIGHT_HAND) {
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 2, 7, arch_interior_rh_mask, BLACK, true);
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 2, 7, arch_interior_rh, WHITE, true);
  }

  if (enemy.xPosDelta != 0) { enemy.xPos = enemy.xPos + enemy.xPosDelta; }
  renderPlayerStance(arduboy, player.xPos, player.yPos, player.stance);
  if (gameStateDetails.enemyType != ENEMY_TYPE_NONE && enemy.xPos < 128) renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);
  if (gameStateDetails.prevState == GAME_STATE_GO_THROUGH_GATE) renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, STANCE_DEATH_6); 
  
  if (gameStateDetails.extArch == ARCH_RIGHT_HAND) {
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 17, -2, arch_exterior_rh_mask, BLACK, false);
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 17, -2, arch_exterior_rh, WHITE, false);
  }
  
  if (gameStateDetails.extArch == ARCH_LEFT_HAND) {
    arduboy.drawCompressedMirror(gameStateDetails.archXPos - 6, -2, arch_exterior_rh_mask, BLACK, true);
    arduboy.drawCompressedMirror(gameStateDetails.archXPos - 6, -2, arch_exterior_rh, WHITE, true);
  }
  
  if (gameStateDetails.intArch == ARCH_RIGHT_HAND) {
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, 3, arch_interior_lh_mask, BLACK, true);
    arduboy.drawCompressedMirror(gameStateDetails.archXPos + 16, 3, arch_interior_lh, WHITE, true);
  }



  //  If the player or enemy has previously been hit, then update their health and render ..

  if (playerHit > 0 || enemyHit > 0) {

    if (playerHit > 0) {

      uint8_t index = (playerHit > 3 ? 2 : playerHit - 1);

      player_BamX = (enemy.xPos + enemy_BamXPos[enemy.stance - STANCE_KICK_MED_END]); 
      if (player_BamX > 128) { player_BamX = 0; }
      player_BamY = enemy.yPos + both_BamYPos[enemy.stance - STANCE_KICK_MED_END]; 

      #ifndef DEBUG_HIDE_BAM

        #ifdef USE_DIFFERENT_BAMS
        arduboy.drawCompressedMirror(player_BamX, player_BamY, bam_masks[index], BLACK, false);
        arduboy.drawCompressedMirror(player_BamX, player_BamY, bam_images[index], WHITE, false);
        #endif

        #ifndef USE_DIFFERENT_BAMS
        arduboy.drawCompressedMirror(player_BamX, player_BamY, bam3_mask, BLACK, false);
        arduboy.drawCompressedMirror(player_BamX, player_BamY, bam3, WHITE, false);
        #endif

      #endif

      player.health = (player.health - playerHit < 10 ? 0 : player.health - playerHit);
      player.regain = 0;

    }
    
    if (enemyHit > 0) {

      uint8_t index = (enemyHit > 3 ? 2 : enemyHit - 1);
      enemy_BamX = player.xPos + player_BamXPos[player.stance - STANCE_KICK_MED_END] ; enemy_BamY = player.yPos + both_BamYPos[player.stance - STANCE_KICK_MED_END]; 

      #ifdef USE_DIFFERENT_BAMS
        arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam_masks[index], BLACK, false);
        arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam_images[index], WHITE, false);
      #endif

      #ifndef USE_DIFFERENT_BAMS
        arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam3_mask, BLACK, false);
        arduboy.drawCompressedMirror(enemy_BamX, enemy_BamY, bam3, WHITE, false);
      #endif

      if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {

        enemy.health = (enemy.health - enemyHit < 10 ? 0 : enemy.health - enemyHit);
        enemy.regain = 0;

        if (enemyStack.isEmpty()) {

            enemyImmediateAction = (random(0, 2) == 0);     // Should the enemy take an immediate action?
            enemyImmediateRetreat = (random(0, 3) == 0);    // Should the enemy retreat immediately?
          
        }
      
      }
      else {

        eagleMode = EAGLE_MODE_FLY_AWAY;

      }

    }

  }
  else {


    // Let the player and enemy regain some health if they haven't been hit in a while ..

    if (playerHit == 0) {

      player.regain++;

      if (player.regain >= HEALTH_REGAIN_LIMIT) {

        player.health = (player.health + HEALTH_REGAIN_POINTS < player.regainLimit ? player.health + HEALTH_REGAIN_POINTS : player.regainLimit);
        player.regain = 0;

      }
    
    }

    if (enemyHit == 0) {

      enemy.regain++;

      if (enemy.regain >= HEALTH_REGAIN_LIMIT) {

        enemy.health = (enemy.health + HEALTH_REGAIN_POINTS < enemy.regainLimit ? enemy.health + HEALTH_REGAIN_POINTS : enemy.regainLimit);
        enemy.regain = 0;
                
      }
    
    }

  }


  // Has the player died ?

  if (!player.dead && player.health == 0) {

    playerStack.clear();
    playerStack.push(STANCE_DEATH_6, STANCE_DEATH_5, STANCE_DEATH_4);
    playerStack.push(STANCE_DEATH_3, STANCE_DEATH_2, STANCE_DEATH_1);
    player.dead = true;

    if (eagleMode == EAGLE_MODE_NONE || eagleMode == EAGLE_MODE_FLY_INIT) {

      enemyStack.insert(STANCE_DEFAULT_LEAN_BACK);
      for (int i = 0; i < 20; i++) {
        enemyStack.insert(STANCE_STANDING_UPRIGHT);
      }

      enemy.xPosDelta = 0;
      
    }

    player.xPosDelta = 0;
    
  }


  // Has the enemy died ?

  if (!enemy.dead && enemy.health == 0) {
    
    enemyStack.clear();
    enemyStack.push(STANCE_DEATH_6, STANCE_DEATH_5, STANCE_DEATH_4);
    enemyStack.push(STANCE_DEATH_3, STANCE_DEATH_2, STANCE_DEATH_1);
    enemy.dead = true;

    playerStack.insert(STANCE_DEFAULT_LEAN_BACK);
    for (int i = 0; i < 15; i++) {
      playerStack.insert(STANCE_STANDING_UPRIGHT);
    }

    player.xPosDelta = 0;
    enemy.xPosDelta = 0;
    
  }


  if (gameStateDetails.enemyType != ENEMY_TYPE_NONE && playerStack.isEmpty() && enemyStack.isEmpty() && player.dead)    { gameStateDetails.setCurrState(GAME_STATE_THE_END); }
  if (gameStateDetails.enemyType == ENEMY_TYPE_PERSON && playerStack.isEmpty() && enemyStack.isEmpty() && enemy.dead)   { gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); }
  
  if (gameStateDetails.prevState == GAME_STATE_GO_THROUGH_GATE && player.xPos > 128 && playerStack.isEmpty()) {
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
  }

  #ifdef DEBUG_ONSCREEN
    int16_t distBetween = absT(enemy.xPos - player.xPos);
    arduboy.fillRect(0, 0, 75, 10, BLACK);
    arduboy.setCursor(1, 1);
    arduboy.print(distBetween);
    arduboy.setCursor(25, 1);
    arduboy.print(enemyHit);
    arduboy.setCursor(35, 1);
    arduboy.print(playerHit);
    arduboy.setCursor(45, 1);
    arduboy.print(player.stance);
    arduboy.setCursor(60, 1);
    arduboy.print(enemy.stance);
  #endif

  #ifdef DEBUG_HITS
    int16_t distBetween = absT(enemy.xPos - player.xPos);
    arduboy.fillRect(0, 0, WIDTH, 10, BLACK);
    arduboy.setCursor(1, 1);
    arduboy.print(_distBetween);
    arduboy.setCursor(15, 1);
    arduboy.print(_distTest);
    arduboy.setCursor(30, 1);
    arduboy.print(enemyHit);
    arduboy.setCursor(40, 1);
    arduboy.print(playerHit);
    arduboy.setCursor(50, 1);
    arduboy.print(_targetStance);
    arduboy.setCursor(65, 1);
    arduboy.print(_pos);
    arduboy.setCursor(85, 1);
    arduboy.print(_action);
  #endif

  #ifdef DEBUG_STRIKING_RANGE
    int16_t distBetween = absT(enemy.xPos - player.xPos);
    arduboy.fillRect(0, 0, WIDTH, 10, BLACK);
    arduboy.setCursor(1, 1);
    arduboy.print(_distBetween);
    arduboy.setCursor(15, 1);
    arduboy.print(_distTest);
    arduboy.setCursor(30, 1);
    arduboy.print(enemyHit);
    arduboy.setCursor(40, 1);
    arduboy.print(playerHit);
    arduboy.setCursor(50, 1);
    arduboy.print(_targetStance);
    arduboy.setCursor(65, 1);
    arduboy.print(_pos);
    arduboy.setCursor(85, 1);
    arduboy.print(_action);
  #endif
  
}

void eagleMovements() {

  if (enemyStack.isEmpty()) {

    switch (eagleMode) {

      case EAGLE_MODE_FLY_INIT:

        enemy.yPos = pgm_read_byte(&eagle_heights[ random(0, 3) ]);
        enemyStack.push(STANCE_EAGLE_1);
        enemy.xPosDelta = -MAIN_SCENE_X_EAGLE_FLYING_DELTA;
        enemy.xPos = 150;
        eagleMode++;
        break;

      case EAGLE_MODE_FLY_TOWARDS:
        {
          int16_t distanceBetween = absT(enemy.xPos - player.xPos);
          uint8_t testDistance = 20;
          
          switch (player.stance) {

            case STANCE_KICK_READY:

              switch (enemy.yPos) {

                case EAGLE_LEVEL_LOW:
                case EAGLE_LEVEL_MED:
                  testDistance = 40;
                  break;

                default:
                  testDistance = 24;
                  break;

              }

              break;
          
            case STANCE_STANDING_UPRIGHT:
            case STANCE_STANDING_UPRIGHT_REV:
              testDistance = 30;
              break;

            default:
              testDistance = 40;
              break;

          }
          
          if (distanceBetween > testDistance) {

            if (eagleWingUp) {
              enemyStack.push(STANCE_EAGLE_1);
            } 
            else {
              enemyStack.push(STANCE_EAGLE_2);
            }

            eagleWingUp = !eagleWingUp;
            enemy.xPosDelta = -MAIN_SCENE_X_EAGLE_FLYING_DELTA;
            
          }
          else {

            eagleMode++;
            
          }
        
        }
        
        break;

      case EAGLE_MODE_ATTACK_1 ... EAGLE_MODE_ATTACK_3:  
        {
          int16_t deltaX = 0;
          
          switch (player.stance) {

            case STANCE_KICK_READY:

              switch (enemy.yPos) {

                case EAGLE_LEVEL_LOW:
                case EAGLE_LEVEL_MED:
                  deltaX = 20;
                  break;

                case EAGLE_LEVEL_HIGH:
                  deltaX = 5;
                  break;

              }
              break;
          
            case STANCE_STANDING_UPRIGHT:
            case STANCE_STANDING_UPRIGHT_REV:
              deltaX = 9;
              break;

            default:
              deltaX = 20;
              break;

          }

          switch (enemy.yPos) {

            case EAGLE_LEVEL_LOW:
            case EAGLE_LEVEL_MED:
              enemy.xPos = player.xPos + deltaX; 
              enemyStack.push(STANCE_EAGLE_3);
              break;

            default:
              enemy.xPos = player.xPos + deltaX; 
              enemyStack.push(STANCE_EAGLE_4);
              break;

          } 
        
          if (eagleMode == EAGLE_MODE_ATTACK_2) {
          
            switch (enemy.yPos) {

              case EAGLE_LEVEL_LOW:
                player.health = (player.health > EAGLE_LEVEL_LOW_HEALTH ? player.health - EAGLE_LEVEL_LOW_HEALTH : 0);
                break;

              case EAGLE_LEVEL_MED:
                player.health = (player.health > EAGLE_LEVEL_MED_HEALTH ? player.health - EAGLE_LEVEL_MED_HEALTH : 0);
                break;

              default:
                player.health = (player.health > EAGLE_LEVEL_HIGH_HEALTH ? player.health - EAGLE_LEVEL_HIGH_HEALTH : 0);
                break;

            }

          }
          
        }

        enemy.xPosDelta = 0;
        eagleMode++; 
        break;
 
      case EAGLE_MODE_FLY_AWAY:
      
        for (int i = 0; i < 5; i++) {
          
          enemyStack.push(STANCE_EAGLE_1_REV);
          enemyStack.push(STANCE_EAGLE_2_REV);

        }
        
        enemy.xPosDelta = MAIN_SCENE_X_EAGLE_FLYING_DELTA;
        eagleMode++; 
        break;     

      default:
      
        eagleMode = EAGLE_MODE_FLY_INIT;
        if (playerStack.isEmpty() && enemyStack.isEmpty()) {
          gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
        }
        break;     
     
    }
  
  }
 
}

void emperor_loop() {

  if (arduboy.justPressed(A_BUTTON)) {
    
    enemyStack.clear();
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

  }
  else {
    

    // Draw background ..

    arduboy.drawFastHLine(0, 52, WIDTH);

    for (int i=54; i< 64; i+=2) {

      drawHorizontalDottedLine(i % 2, WIDTH, i);

    }
    
    if (enemyStack.isEmpty()) {

      switch (emperorMode) {

        case EMPEROR_MODE_INIT:
        
          enemyStack.push(STANCE_RUNNING_STRAIGHTEN_UP_REV);
          
          for (int i = 0; i < 2; i++) {
            
            enemyStack.push(STANCE_RUNNING_RF_END_REV, STANCE_RUNNING_8_REV, STANCE_RUNNING_5_REV);
            enemyStack.push(STANCE_RUNNING_4_REV, STANCE_RUNNING_LF_END_REV, STANCE_RUNNING_2_REV);
            enemyStack.push(STANCE_RUNNING_7_REV, STANCE_RUNNING_4_REV);
            
          }
          
          enemy.xPosDelta = 5;
          enemy.xPos = -48;
          
          break;

        case EMPEROR_MODE_PAUSE_1:
          for (int i = 0; i < 5; i++) {
            enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
          }
      
          break;

        case EMPEROR_LIFT_ARM:
        
          for (int i =0; i < 9; i++) {
            enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
          }
          
          break;
          
        case EMPEROR_MODE_PAUSE_2:
        case EMPEROR_MODE_PAUSE_3:
        
          for (int i = 0; i < 3; i++) {
            enemyStack.push(STANCE_STANDING_UPRIGHT_REV);
          }
      
          break;
        
        case EMPEROR_MODE_BOW:
        
          enemyStack.push(STANCE_BOW_1, STANCE_BOW_2);
          enemyStack.push(STANCE_BOW_2, STANCE_BOW_1);
          
          break;
          
        case EMPEROR_MODE_FIGHTER_LEAVE:
        
          for (int i = 0; i < 2; i++) {

            enemyStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
            enemyStack.push(STANCE_RUNNING_4, STANCE_RUNNING_LF_END, STANCE_RUNNING_2);
            enemyStack.push(STANCE_RUNNING_7, STANCE_RUNNING_4);
            
          }

          enemy.xPosDelta = -5;
          break;

      }

    }


    // Render screen ..
    
    if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
      
      enemy.stance = enemyStack.pop();
      enemy.xPos = enemy.xPos + enemy.xPosDelta;

      if (enemyStack.isEmpty()) {

        enemy.xPosDelta = 0;
        emperorMode++;

        if (emperorMode > EMPEROR_MODE_FIGHTER_LEAVE) { gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); }

      }
      
    }

    arduboy.drawCompressedMirror(92, 45, emperor_mask, BLACK, false);
    arduboy.drawCompressedMirror(87, 4, emperor, WHITE, false);

    if (emperorMode >= EMPEROR_LIFT_ARM && emperorMode < EMPEROR_MODE_FIGHTER_LEAVE) {

      arduboy.drawCompressedMirror(70, 19, emperor_arm_out, WHITE, false);
      
    }
    else {

      arduboy.drawCompressedMirror(87, 21, emperor_arm_normal, WHITE, false);
      
    }

    arduboy.drawCompressedMirror(16, 7, arch_interior_rh_mask, BLACK, false);
    arduboy.drawCompressedMirror(16, 7, arch_interior_rh, WHITE, false);
    renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);
    arduboy.drawCompressedMirror(0, 3, arch_interior_lh_mask, BLACK, false);
    arduboy.drawCompressedMirror(0, 3, arch_interior_lh, WHITE, false);
    
  }

}

void enemy_approach_init() {

  for (int i = 0; i < 2; i++) {

    enemyStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
    enemyStack.push(STANCE_RUNNING_4, STANCE_RUNNING_LF_END, STANCE_RUNNING_2);
    enemyStack.push(STANCE_RUNNING_7, STANCE_RUNNING_4);
    
  }

  player.xPosDelta = MAIN_SCENE_X_SIDLING_2_DELTA;
  gameStateDetails.setCurrState(GAME_STATE_ENEMY_APPROACH);

  enemy.xPos = 48;

}

// ---------------------------------------------------------------------------------------------------------------
//  Enemy movements ..

void enemyMovements() {

  int16_t distanceBetween = absT(enemy.xPos - player.xPos);

  if (enemyStack.isEmpty()) {

    enemy.xPosDelta = 0;

    switch (enemy.stance) {

      case STANCE_DEFAULT:

        switch (player.stance) {

          case STANCE_STANDING_UPRIGHT:
          
            if (distanceBetween < 17) {
              
              returnFromAction(ACTION_HIGH_KICK, ACTION_RETURN_TO_DEFAULT);
              enemyStack.push(STANCE_KICK_HIGH_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);

#ifdef DEBUG_HITS 
_action = 1; 
#endif            

            }
            else if (distanceBetween > 14) { // Otherwise creep forward ..
              
              enemyStack.push(STANCE_DEFAULT, STANCE_SIDLING_3, STANCE_SIDLING_1);
              enemy.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA;

#ifdef DEBUG_HITS 
_action = 2;              
#endif

            }

            break;

          case STANCE_RUNNING_1:
          case STANCE_RUNNING_2:
          case STANCE_RUNNING_3:
          case STANCE_RUNNING_4:
          case STANCE_RUNNING_5:
          case STANCE_RUNNING_6:
          case STANCE_RUNNING_7:
          case STANCE_RUNNING_8:
          case STANCE_RUNNING_STRAIGHTEN_UP:
          
            if (distanceBetween < 79) {
        
              returnFromAction(ACTION_HIGH_KICK, ACTION_RETURN_TO_DEFAULT);
              enemyStack.push(STANCE_KICK_HIGH_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);

#ifdef DEBUG_HITS 
_action = 3;              
#endif

            }
            else if (distanceBetween > 14) { // Otherwise creep forward ..
              
              enemyStack.push(STANCE_DEFAULT, STANCE_SIDLING_3, STANCE_SIDLING_1);
              enemy.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA;

#ifdef DEBUG_HITS 
_action = 4;
#endif

            }

            break;

          default:
            {

              uint8_t action = 0;
              uint8_t returnAction = 0;
              bool performActionRegardless = false;

              if (distanceBetween < 50) {

                if (enemyImmediateAction) {

                  action = ACTION_MED_KICK;

                }
                else {
                  action = random(ACTION_MIN_KICK, ACTION_MAX_PUNCH + gameStateDetails.activity);
                }

                returnAction = random(ACTION_RETURN_TO_DEFAULT, ACTION_RETURN_TO_ACTION_READY + 1);
                performActionRegardless = random(CHANCE_PERFORM_ACTION, CHANCE_PERFORM_ACTION_REGARDLESS_MAX) == CHANCE_PERFORM_ACTION;
                

                // If an action was selected then perform it if it will strike the player or we are doing it regardless ..

                if (action <= ACTION_MAX && (inStrikingRange(action, enemy.xPos, ENEMY_TYPE_PERSON, player.stance, player.xPos) > 0 || performActionRegardless)) {

                  returnFromAction(action, returnAction);

                  switch (action) {

                    case ACTION_HIGH_KICK:
                      enemyStack.push(STANCE_KICK_HIGH_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);
                      break;

                    case ACTION_MED_KICK:
                      enemyStack.push(STANCE_KICK_MED_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);
                      break;

                    case ACTION_LOW_KICK:
                      enemyStack.push(STANCE_KICK_LOW_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);
                      break;

                    case ACTION_HIGH_PUNCH:
                      enemyStack.push(STANCE_PUNCH_HIGH_RH_END, STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
                      break;

                    case ACTION_MED_PUNCH:
                      enemyStack.push(STANCE_PUNCH_MED_RH_END, STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
                      break;

                    case ACTION_LOW_PUNCH:
                      enemyStack.push(STANCE_PUNCH_LOW_RH_END, STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
                      break;
                      
                  }

#ifdef DEBUG_HITS 
_action = 5;                  
#endif

                }
                else {
                      
                    
                  // Is the enemy too close to the player?  If so, should he move back ..

                  bool moveEnemy = enemyImmediateRetreat || (random(CHANCE_PERFORM_ACTION, CHANCE_MOVE_ENEMY_MAX + 1) == CHANCE_PERFORM_ACTION);

                  if (moveEnemy && distanceBetween < DISTANCE_TOO_CLOSE) {

                    enemyStack.push(STANCE_DEFAULT, STANCE_SIDLING_7, STANCE_DEFAULT_LEAN_FORWARD);
                    enemyStack.push(STANCE_SIDLING_6, STANCE_SIDLING_5, STANCE_SIDLING_4);
                    enemy.xPosDelta = +MAIN_SCENE_X_SIDLING_1_DELTA;

#ifdef DEBUG_HITS 
_action = 6;
#endif

                  }


                  // Are the player and enemy too far apart?  If so, should he move forward ..
                  
                  else if (moveEnemy && distanceBetween > DISTANCE_TOO_FAR) {
                    
                    enemyStack.push(STANCE_DEFAULT, STANCE_SIDLING_3, STANCE_SIDLING_1);
                    enemy.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA;

#ifdef DEBUG_HITS 
_action = 7;
#endif

                  }

                }

              }
              else {

                enemyStack.push(STANCE_DEFAULT, STANCE_SIDLING_3, STANCE_SIDLING_1);
                enemy.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA;

#ifdef DEBUG_HITS 
_action = 8;
#endif

              }

            }

            break;
                        
        }

        break;
    
      case STANCE_KICK_READY:
        {
          
          uint8_t action = (enemyImmediateAction ? ACTION_MED_KICK : random(ACTION_MIN_KICK, ACTION_MAX_KICK + 1));
          uint8_t returnAction = random(ACTION_RETURN_TO_DEFAULT, ACTION_RETURN_TO_ACTION_READY + 1);
        
          if (inStrikingRange(action, enemy.xPos, ENEMY_TYPE_PERSON, player.stance, player.xPos) > 0) {

            returnFromAction(action, returnAction);
            
            switch (action) {

              case ACTION_HIGH_KICK:
                enemyStack.push(STANCE_KICK_READY, STANCE_KICK_HIGH_END);
                break;

              case ACTION_MED_KICK:
                enemyStack.push(STANCE_KICK_READY, STANCE_KICK_MED_END);
                break;

              case ACTION_LOW_KICK:
                enemyStack.push(STANCE_KICK_READY, STANCE_KICK_LOW_END);
                break;
                
            }

#ifdef DEBUG_HITS 
_action = 9;
#endif

          }
          else {


            // Should we return to the default standing position ?

            if (random(ACTION_RETURN_TO_DEFAULT, ACTION_RETURN_TO_DEFAULT + 1) == ACTION_RETURN_TO_DEFAULT) {
              enemyStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);
            }

#ifdef DEBUG_HITS 
_action = 10;           
#endif

          }

        }

        break;

      case STANCE_PUNCH_READY:
        {
          
          uint8_t action = (enemyImmediateAction ? ACTION_MED_PUNCH : random(ACTION_MIN_PUNCH, ACTION_MAX_PUNCH + 1));
          uint8_t returnAction = random(ACTION_RETURN_TO_DEFAULT, ACTION_RETURN_TO_ACTION_READY + 1);
        
          if (inStrikingRange(action, enemy.xPos, ENEMY_TYPE_PERSON, player.stance, player.xPos) > 0) {

            returnFromAction(action, returnAction);
            
            switch (action) {

              case ACTION_HIGH_PUNCH:
                enemyStack.push(STANCE_PUNCH_READY, (enemy.rightPunch ? STANCE_PUNCH_HIGH_RH_END : STANCE_PUNCH_HIGH_LH_END));
                break;

              case ACTION_MED_PUNCH:
                enemyStack.push(STANCE_PUNCH_READY, (enemy.rightPunch ? STANCE_PUNCH_MED_RH_END : STANCE_PUNCH_MED_LH_END));
                break;

              case ACTION_LOW_PUNCH:
                enemyStack.push(STANCE_PUNCH_READY, (enemy.rightPunch ? STANCE_PUNCH_LOW_RH_END : STANCE_PUNCH_LOW_LH_END));
                break;
                
            }
            
            enemy.rightPunch = !enemy.rightPunch;

#ifdef DEBUG_HITS 
_action = 11;
#endif

          }
          else {

            
            // Should we return to the default standing position ?

            if (random(ACTION_RETURN_TO_DEFAULT, ACTION_RETURN_TO_DEFAULT + 1) == ACTION_RETURN_TO_DEFAULT) {
              enemyStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);

#ifdef DEBUG_HITS 
_action = 12;
#endif

            }
            
          }

        }

        break;

    }

  }

  enemyImmediateAction = false;
  enemyImmediateRetreat = false;

}



// ---------------------------------------------------------------------------------------------------------------
//  Player movements ..

void playerMovements() {
  
  uint8_t currPressed = arduboy.curButtonState();
  uint16_t distBetween = (gameStateDetails.enemyType != ENEMY_TYPE_NONE ? absT(enemy.xPos - player.xPos) : 9999);

  if (playerStack.isEmpty()) {
  
    player.xPosDelta = 0;

    if ((currPressed & A_BUTTON_MASK)) {

      switch (player.stance) {

        case STANCE_DEFAULT:

          // Move to the Kick Ready position ..

          if (!(currPressed & UP_BUTTON_MASK) && !(currPressed & RIGHT_BUTTON_MASK) && !(currPressed & RIGHT_BUTTON_MASK) && !(currPressed & DOWN_BUTTON_MASK)) {

            playerStack.push(STANCE_KICK_READY, STANCE_DEFAULT_LEAN_BACK);
            
          }
          else {

            // Perform an immediate kick from the default position ..

            if (currPressed & RIGHT_BUTTON_MASK)  {             // Medium kick ..
              playerStack.push(STANCE_KICK_MED_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);
            }

            else if (currPressed & UP_BUTTON_MASK)  {           // High kick ..
              playerStack.push(STANCE_KICK_HIGH_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);
            }

            else if (currPressed & DOWN_BUTTON_MASK)  {         // Low kick ..
              playerStack.push(STANCE_KICK_LOW_END, STANCE_KICK_STANDING_TRANSITION, STANCE_DEFAULT_LEAN_BACK);
            }

          }
          break;

        case STANCE_KICK_HIGH_END:
        case STANCE_KICK_MED_END:
        case STANCE_KICK_LOW_END:                               // As the A button is still being held, return to the Kick Ready position ..
          playerStack.push(STANCE_KICK_READY);
          break;

        case STANCE_KICK_READY:

          if ((currPressed & UP_BUTTON_MASK) || (currPressed & RIGHT_BUTTON_MASK) || (currPressed & DOWN_BUTTON_MASK)) {

            player.activity++;

            if (player.activity > 4) {
              playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);
              player.activity = 0;
            }

            if (currPressed & RIGHT_BUTTON_MASK)  {               // Medium kick ..
              playerStack.push(STANCE_KICK_READY, STANCE_KICK_MED_END);
            }

            else if (currPressed & UP_BUTTON_MASK)  {             // High kick ..
              playerStack.push(STANCE_KICK_READY, STANCE_KICK_HIGH_END);
            }

            else if (currPressed & DOWN_BUTTON_MASK)  {           // Low kick ..
              playerStack.push(STANCE_KICK_READY, STANCE_KICK_LOW_END);
            }

          }

          break;

        case STANCE_PUNCH_READY:                                  // Really shouldn't happen but if B is pressed before A and both held ..
            
            if (currPressed & RIGHT_BUTTON_MASK)  {               // Medium punch ..
              playerStack.push(STANCE_PUNCH_READY, (player.rightPunch ? STANCE_PUNCH_MED_RH_END : STANCE_PUNCH_MED_LH_END));
            }

            else if (currPressed & UP_BUTTON_MASK)  {             // High punch ..
              playerStack.push(STANCE_PUNCH_READY, (player.rightPunch ? STANCE_PUNCH_HIGH_RH_END : STANCE_PUNCH_HIGH_LH_END));
            }

            else if (currPressed & DOWN_BUTTON_MASK)  {           // Low punch ..
              playerStack.push(STANCE_PUNCH_READY, (player.rightPunch ? STANCE_PUNCH_LOW_RH_END : STANCE_PUNCH_LOW_LH_END));
            }

            player.rightPunch = !player.rightPunch;
            break;

      }
              
    }
    else {


      // If the A Button is no longer being pressed ..

      switch (player.stance) {

        case STANCE_KICK_READY:
          player.activity = 0;
          playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);
          break;

        case STANCE_KICK_HIGH_END:
        case STANCE_KICK_MED_END:
        case STANCE_KICK_LOW_END:
          player.activity = 0;
          playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD, STANCE_KICK_READY);
          break;
        
      }


      // Is the B button being pressed ?

      if ((currPressed & B_BUTTON_MASK)) {

        switch (player.stance) {

          case STANCE_STANDING_UPRIGHT:                         // If standing upright, move to the fighting position ..
            playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_BACK);
            break;

          case STANCE_DEFAULT:                                  // Move to the Punch Ready position ..

            if (!(currPressed & UP_BUTTON_MASK) && !(currPressed & RIGHT_BUTTON_MASK) && !(currPressed & DOWN_BUTTON_MASK)) {

              playerStack.push(STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
              
            }
            else {


              // Perform a immediate punch from the standing position ..

              if (currPressed & RIGHT_BUTTON_MASK)  {           // Medium punch ..
                playerStack.push(STANCE_PUNCH_MED_RH_END, STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
              }

              else if (currPressed & UP_BUTTON_MASK)  {              // High punch ..
                playerStack.push(STANCE_PUNCH_HIGH_RH_END, STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
              }

              else if (currPressed & UP_BUTTON_MASK)  {              // Low punch ..
                playerStack.push(STANCE_PUNCH_LOW_RH_END, STANCE_PUNCH_READY, STANCE_DEFAULT_LEAN_BACK);
              }

            }
            
            break;

          case STANCE_PUNCH_HIGH_LH_END:                        // As the B button is still being held, return to the Puch Ready position ..
          case STANCE_PUNCH_HIGH_RH_END:
          case STANCE_PUNCH_MED_LH_END:
          case STANCE_PUNCH_MED_RH_END:
          case STANCE_PUNCH_LOW_LH_END:
          case STANCE_PUNCH_LOW_RH_END:

            playerStack.push(STANCE_PUNCH_READY);
            break;

          case STANCE_PUNCH_READY:
            
            if (currPressed & RIGHT_BUTTON_MASK)  {             // Medium punch ..
              playerStack.push(STANCE_PUNCH_READY, (player.rightPunch ? STANCE_PUNCH_MED_RH_END : STANCE_PUNCH_MED_LH_END));
            }

            else if (currPressed & UP_BUTTON_MASK)  {           // High punch ..
              playerStack.push(STANCE_PUNCH_READY, (player.rightPunch ? STANCE_PUNCH_HIGH_RH_END : STANCE_PUNCH_HIGH_LH_END));
            }

            else if (currPressed & DOWN_BUTTON_MASK)  {         // Low punch ..
              playerStack.push(STANCE_PUNCH_READY, (player.rightPunch ? STANCE_PUNCH_LOW_RH_END : STANCE_PUNCH_LOW_LH_END));
            }

            player.rightPunch = !player.rightPunch;
            break;
          
        }

      }
      else {


        switch (player.stance) {    // If the B Button is no longer being pressed return to the default stance ..

          case STANCE_PUNCH_READY:

            playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);
            break;

          case STANCE_PUNCH_HIGH_LH_END:
          case STANCE_PUNCH_HIGH_RH_END:
          case STANCE_PUNCH_MED_LH_END:
          case STANCE_PUNCH_MED_RH_END:
          case STANCE_PUNCH_LOW_LH_END:
          case STANCE_PUNCH_LOW_RH_END:

            playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD, STANCE_PUNCH_READY);
            break;

        }


        // Neither the A or B button has been pressed ..

        switch (player.stance) {

          case STANCE_DEFAULT:

            if (currPressed & UP_BUTTON_MASK)  { // Stand upright again ..
              playerStack.push(STANCE_STANDING_UPRIGHT, STANCE_DEFAULT_LEAN_BACK);
            }

            else if (currPressed & RIGHT_BUTTON_MASK)  { // Sidle forward ..
              
              if (distBetween > DISTANCE_BETWEEN_LRG || !gameStateDetails.enemyType == ENEMY_TYPE_PERSON) {

                player.xPosDelta = -MAIN_SCENE_X_SIDLING_2_DELTA; 
                playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_BACK, STANCE_SIDLING_3);
                playerStack.push(STANCE_SIDLING_2, STANCE_SIDLING_1, STANCE_DEFAULT_LEAN_FORWARD);
  
              }
              else if (distBetween > DISTANCE_BETWEEN_MED) { 
                
                player.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA; 
                playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_BACK, STANCE_SIDLING_3);
                playerStack.push(STANCE_SIDLING_2, STANCE_SIDLING_1, STANCE_DEFAULT_LEAN_FORWARD);
                
              }
              else if (distBetween > DISTANCE_BETWEEN_SML) { 
                
                player.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA; 
                playerStack.push(STANCE_DEFAULT, STANCE_SIDLING_2, STANCE_SIDLING_1);
                
              }
              else if (distBetween > DISTANCE_BETWEEN_TINY) { 

                player.xPosDelta = -MAIN_SCENE_X_SIDLING_1_DELTA; 
                playerStack.push(STANCE_DEFAULT, STANCE_SIDLING_1);
                
              }
                                            
            }

            else if (currPressed & LEFT_BUTTON_MASK && player.xPosOverall > 12)  { // Sidle backward ..
              
              player.xPosDelta = MAIN_SCENE_X_SIDLING_1_DELTA; 
              playerStack.push(STANCE_DEFAULT, STANCE_SIDLING_7, STANCE_DEFAULT_LEAN_FORWARD);
              playerStack.push(STANCE_SIDLING_6, STANCE_SIDLING_5, STANCE_SIDLING_4);
              
            }

            break;

          case STANCE_STANDING_UPRIGHT:

            if (currPressed & DOWN_BUTTON_MASK)  { // If standing upright, move to the fighting position ..
              playerStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_BACK);
            }
            
            else if (currPressed & RIGHT_BUTTON_MASK && distBetween > 100)  { // Start running ..
              player.rightFoot = true;
              player.xPosDelta = -MAIN_SCENE_X_SIDLING_2_DELTA;            
              playerStack.push(STANCE_RUNNING_6, STANCE_RUNNING_8, STANCE_RUNNING_5);
//              playerStack.push(STANCE_RUNNING_4, STANCE_RUNNING_3, STANCE_RUNNING_2);
              playerStack.push(STANCE_RUNNING_1);
            }
            break;

          case STANCE_RUNNING_LF_END:
          case STANCE_RUNNING_RF_END:
            
            if (currPressed & RIGHT_BUTTON_MASK && distBetween > 100)  { // Continue running ..

              if (!player.rightFoot) {
                player.xPosDelta = -MAIN_SCENE_X_SIDLING_2_DELTA;            
                playerStack.push(STANCE_RUNNING_LF_END, STANCE_RUNNING_2, STANCE_RUNNING_7);
                playerStack.push(STANCE_RUNNING_4);
              }
              else {
                player.xPosDelta = -MAIN_SCENE_X_SIDLING_2_DELTA;            
                playerStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
                playerStack.push(STANCE_RUNNING_4);
              }

              player.rightFoot = !player.rightFoot;

            }
            else {
              player.xPosDelta = -MAIN_SCENE_X_SIDLING_2_DELTA;            
              playerStack.push(STANCE_STANDING_UPRIGHT, STANCE_RUNNING_STRAIGHTEN_UP);
            }
            break;
                    
        }

      }
      
    }

  }
    
}


void drawPrincessBackground_1() {

  uint8_t x = 72;

  for (int y = 50; y < 64; y = y + 2) {
    arduboy.drawFastHLine(x, y, WIDTH - x);    
    x = x + (y < 56 ? -20 : 20);
  }

  arduboy.drawCompressedMirror(16, 7, arch_interior_rh_mask, BLACK, false);
  arduboy.drawCompressedMirror(16, 7, arch_interior_rh, WHITE, false);
  arduboy.drawCompressedMirror(89, 19, princess_seat_mask, BLACK, false);
  arduboy.drawCompressedMirror(89, 19, princess_seat, WHITE, false);

}

void drawPrincessBackground_2() {
  
  arduboy.drawCompressedMirror(0, 3, arch_interior_lh_mask, BLACK, false);
  arduboy.drawCompressedMirror(0, 3, arch_interior_lh, WHITE, false);

}


void showScene() {

  if (arduboy.justPressed(A_BUTTON)) {
    
    if (gameStateDetails.getCurrState() == GAME_STATE_THE_END) {
      gameStateDetails.sequence = 0;
    }     

    enemyStack.clear();
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

  }
  else {
    
    switch (gameStateDetails.getCurrState()) {

      case GAME_STATE_TITLE_SCENE:
        arduboy.drawCompressedMirror(0, 0, title, WHITE, false);
        break;

#ifdef JORDAN_MECHNER
      case GAME_STATE_JM_SCENE:
        arduboy.drawCompressedMirror(0, 0, jordanMechner, WHITE, false);
        if (gameStateDetails.delayInterval > 8) {
          arduboy.fillRect(0, 40, WIDTH, 24, BLACK);
        }
        break;
#endif      

      case GAME_STATE_CASTLE_SCENE:
        arduboy.drawCompressedMirror(0, 0, castle, WHITE, false);
        break;
        
      case GAME_STATE_THE_END:
        arduboy.drawCompressedMirror(34, 26, theend, WHITE, false);
        break;
        
      case GAME_STATE_PRINCESS:

        drawPrincessBackground_1();
        drawPrincessBackground_2();
        arduboy.drawCompressedMirror(93, 19, princess_sitting_mask, BLACK, false);
        arduboy.drawCompressedMirror(93, 19, princess_sitting, WHITE, false);
        
        break;

      case GAME_STATE_ENEMY_APPROACH:

        draw_background();
    
        //  Update player and enemy positions and stances ..
    
        if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
          
          if (!enemyStack.isEmpty()) {
            enemy.stance = enemyStack.pop();
          }
          else {
            player.xPosDelta = 0;
            gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);
          }
          
        }
    
        
        // Move scenery if needed ..
    
        if (player.xPosDelta != 0) {
    
          mainSceneX = mainSceneX + player.xPosDelta;
    
          if (mainSceneX < -MAIN_SCENE_IMG_WIDTH) { mainSceneX = 0; }
          if (mainSceneX > 0) { mainSceneX = mainSceneX - MAIN_SCENE_IMG_WIDTH; }
    
        }
    
        if (enemy.xPos < 128) renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);

      default:
        break;

    }
    
    if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {

      if (gameStateDetails.delayInterval > 0) {

        gameStateDetails.delayInterval--;

        if (gameStateDetails.delayInterval == 0) { 

          // if (gameStateDetails.getCurrState() == GAME_STATE_THE_END) {  // Not relevant for 'The End' scene.
          //   gameStateSeq = 0;
          // }            
          gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE); 

        }

      }

    }
  
  }

}



void finalScene() {
  
  if (arduboy.justPressed(A_BUTTON)) {
    
    enemyStack.clear();
    gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

  }
  else {
    

    // Draw background ..

    drawPrincessBackground_1();
    
    if (enemyStack.isEmpty() && playerStack.isEmpty()) {

      finalSceneMode++;

      switch (finalSceneMode) {

        case FINAL_SCENE_INIT:
        
          for (int i = 0; i < 3; i++) {
            
            playerStack.push(STANCE_RUNNING_LF_END, STANCE_RUNNING_2, STANCE_RUNNING_7);
            playerStack.push(STANCE_RUNNING_4);

            playerStack.push(STANCE_RUNNING_RF_END, STANCE_RUNNING_8, STANCE_RUNNING_5);
            playerStack.push(STANCE_RUNNING_4);

          }
          
          player.xPosDelta = 5;
          player.xPos = -48;
          
          enemyStack.push(STANCE_PRINCESS_STANDING);
          for (int i = 0; i < 12; i++) {
            enemyStack.push(STANCE_PRINCESS_SITTING);
          }
          enemy.xPos = 93;
          enemy.yPos = 14;

          break;

        case FINAL_SCENE_PAUSE:
          playerStack.push(STANCE_STANDING_UPRIGHT);
          playerStack.push(STANCE_RUNNING_STRAIGHTEN_UP);
          break;

        case FINAL_SCENE_PAUSE_2:
          player.xPosDelta = 0;
          enemy.xPosDelta = 0;
          for (int i = 0; i < 4; i++) {
            playerStack.push(STANCE_STANDING_UPRIGHT);
          }
          break;

        case FINAL_SCENE_KISSING:
          enemy.xPos = 90;
          enemy.yPos = 15;
          enemyStack.push(STANCE_PRINCESS_KISSING, STANCE_PRINCESS_KISSING, STANCE_PRINCESS_KISSING);
          break;

        case FINAL_SCENE_BREAK_UP:
          enemy.xPos = 93;
          enemy.yPos = 15;
          for (int i = 0; i < 18; i++) {
            enemyStack.push(STANCE_PRINCESS_STANDING);
          }
          break;
                    
      }

    }


    // Render screen ..
    
    if (arduboy.everyXFrames(ANIMATION_NUMBER_OF_FRAMES)) {
      
      if (!enemyStack.isEmpty()) {
        enemy.stance = enemyStack.pop();
        enemy.xPos = enemy.xPos + enemy.xPosDelta;
      }
           
      if (!playerStack.isEmpty()) {
        player.stance = playerStack.pop();
        player.xPos = player.xPos + player.xPosDelta;
        }

    }

    renderPlayerStance(arduboy, player.xPos, player.yPos, player.stance);
    renderEnemyStance(arduboy, enemy.xPos, enemy.yPos, enemy.stance);

    drawPrincessBackground_2();
    
    if (finalSceneMode == FINAL_SCENE_BREAK_UP && enemyStack.isEmpty()) {

      gameStateDetails.setCurrState(GAME_STATE_FOLLOW_SEQUENCE);

    }
    
  }

}



#define ACTIONS_NUMBER_OF_STANCES 42

const uint8_t PROGMEM actions_distance[] = {


  /* ACTION_MED_KICK ------------------------------------------------------------------------------------- */

/* 000  ACTION_MED_KICK,  STANCE_DEFAULT,                   */ (4 * 30) + DAMAGE_2_POINT,
/* 001  ACTION_MED_KICK,  STANCE_DEFAULT_LEAN_BACK          */ (4 * 30) + DAMAGE_2_POINT,
/* 002  ACTION_MED_KICK,  STANCE_KICK_READY                 */ (4 * 26) + DAMAGE_2_POINT, 
/* 003  ACTION_MED_KICK,  STANCE_DEFAULT_LEAN_FORWARD       */ (4 * 30) + DAMAGE_2_POINT,
/* 004  ACTION_MED_KICK,  STANCE_KICK_STANDING_TRANSITION   */ (4 * 30) + DAMAGE_2_POINT, 
/* 005  ACTION_MED_KICK,  STANCE_KICK_MED_END               */ (4 * 23) + DAMAGE_2_POINT,
/* 006  ACTION_MED_KICK,  STANCE_KICK_LOW_END               */ (4 * 30) + DAMAGE_2_POINT,
/* 007  ACTION_MED_KICK,  STANCE_KICK_HIGH_END              */ (4 * 44) + DAMAGE_2_POINT,
/* 008  ACTION_MED_KICK,  STANCE_PUNCH_MED_RH_END           */ (4 * 30) + DAMAGE_2_POINT, 
/* 009  ACTION_MED_KICK,  STANCE_PUNCH_MED_LH_END           */ (4 * 30) + DAMAGE_2_POINT,
/* 010  ACTION_MED_KICK,  STANCE_PUNCH_HIGH_RH_END          */ (4 * 30) + DAMAGE_2_POINT,
/* 011  ACTION_MED_KICK,  STANCE_PUNCH_HIGH_LH_END          */ (4 * 30) + DAMAGE_2_POINT, 
/* 012  ACTION_MED_KICK,  STANCE_PUNCH_LOW_RH_END           */ (4 * 30) + DAMAGE_2_POINT,
/* 013  ACTION_MED_KICK,  STANCE_PUNCH_LOW_LH_END           */ (4 * 30) + DAMAGE_2_POINT,
/* 014  ACTION_MED_KICK,  STANCE_PUNCH_READY                */ (4 * 23) + DAMAGE_2_POINT, 
/* 015  ACTION_MED_KICK,  STANCE_STANDING_UPRIGHT           */ (4 * 30) + DAMAGE_2_POINT,
/* 016  ACTION_MED_KICK,  STANCE_SIDLING_1                  */ (4 * 30) + DAMAGE_2_POINT,
/* 017  ACTION_MED_KICK,  STANCE_SIDLING_2                  */ (4 * 30) + DAMAGE_2_POINT, 
/* 018  ACTION_MED_KICK,  STANCE_SIDLING_3                  */ (4 * 30) + DAMAGE_2_POINT,
/* 019  ACTION_MED_KICK,  STANCE_SIDLING_4                  */ (4 * 30) + DAMAGE_2_POINT,
/* 020  ACTION_MED_KICK,  STANCE_SIDLING_5                  */ (4 * 30) + DAMAGE_2_POINT, 
/* 021  ACTION_MED_KICK,  STANCE_SIDLING_6                  */ (4 * 30) + DAMAGE_2_POINT,
/* 022  ACTION_MED_KICK,  STANCE_SIDLING_7                  */ (4 * 30) + DAMAGE_2_POINT,
/* 023  ACTION_MED_KICK,  STANCE_RUNNING_1                  */ (4 * 30) + DAMAGE_2_POINT, 
/* 024  ACTION_MED_KICK,  STANCE_RUNNING_2                  */ (4 * 30) + DAMAGE_2_POINT,
/* 025  ACTION_MED_KICK,  STANCE_RUNNING_3                  */ (4 * 30) + DAMAGE_2_POINT,
/* 026  ACTION_MED_KICK,  STANCE_RUNNING_4                  */ (4 * 30) + DAMAGE_2_POINT, 
/* 027  ACTION_MED_KICK,  STANCE_RUNNING_5                  */ (4 * 30) + DAMAGE_2_POINT,
/* 028  ACTION_MED_KICK,  STANCE_RUNNING_6                  */ (4 * 30) + DAMAGE_2_POINT,
/* 029  ACTION_MED_KICK,  STANCE_RUNNING_7                  */ (4 * 30) + DAMAGE_2_POINT, 
/* 030  ACTION_MED_KICK,  STANCE_RUNNING_8                  */ (4 * 30) + DAMAGE_2_POINT, 
/* 031  ACTION_MED_KICK,  STANCE_RUNNING_STRAIGHTEN_UP      */ (4 * 30) + DAMAGE_2_POINT,
/* 032  ACTION_MED_KICK,  STANCE_STANDING_UPRIGHT_REV       */ (4 * 30) + DAMAGE_2_POINT, 
/* 033  ACTION_MED_KICK,  STANCE_RUNNING_1_REV              */ (4 * 30) + DAMAGE_2_POINT,
/* 034  ACTION_MED_KICK,  STANCE_RUNNING_2_REV              */ (4 * 30) + DAMAGE_2_POINT,
/* 035  ACTION_MED_KICK,  STANCE_RUNNING_3_REV              */ (4 * 30) + DAMAGE_2_POINT, 
/* 036  ACTION_MED_KICK,  STANCE_RUNNING_4_REV              */ (4 * 30) + DAMAGE_2_POINT,
/* 037  ACTION_MED_KICK,  STANCE_RUNNING_5_REV              */ (4 * 30) + DAMAGE_2_POINT,
/* 038  ACTION_MED_KICK,  STANCE_RUNNING_6_REV              */ (4 * 30) + DAMAGE_2_POINT, 
/* 039  ACTION_MED_KICK,  STANCE_RUNNING_7_REV              */ (4 * 30) + DAMAGE_2_POINT,
/* 040  ACTION_MED_KICK,  STANCE_RUNNING_8_REV              */ (4 * 30) + DAMAGE_2_POINT,
/* 041  ACTION_MED_KICK,  STANCE_RUNNING_STRAIGHTEN_UP_REV  */ (4 * 30) + DAMAGE_2_POINT, 


/* ACTION_LOW_KICK ----------------------------------------------------------- */

/* 042  ACTION_LOW_KICK,  STANCE_DEFAULT,                   */ (4 * 35) + DAMAGE_1_POINT,
/* 043  ACTION_LOW_KICK,  STANCE_DEFAULT_LEAN_BACK          */ (4 * 35) + DAMAGE_1_POINT,
/* 044  ACTION_LOW_KICK,  STANCE_KICK_READY                 */ (4 * 23) + DAMAGE_1_POINT,
/* 045  ACTION_LOW_KICK,  STANCE_DEFAULT_LEAN_FORWARD       */ (4 * 35) + DAMAGE_1_POINT,
/* 046  ACTION_LOW_KICK,  STANCE_KICK_STANDING_TRANSITION   */ (4 * 32) + DAMAGE_1_POINT,
/* 047  ACTION_LOW_KICK,  STANCE_KICK_MED_END               */ (4 * 42) + DAMAGE_1_POINT,
/* 048  ACTION_LOW_KICK,  STANCE_KICK_LOW_END               */ (4 * 35) + DAMAGE_1_POINT,
/* 049  ACTION_LOW_KICK,  STANCE_KICK_HIGH_END              */ (4 * 42) + DAMAGE_1_POINT,
/* 050  ACTION_LOW_KICK,  STANCE_PUNCH_MED_RH_END           */ (4 * 35) + DAMAGE_1_POINT,
/* 051  ACTION_LOW_KICK,  STANCE_PUNCH_MED_LH_END           */ (4 * 35) + DAMAGE_1_POINT,
/* 052  ACTION_LOW_KICK,  STANCE_PUNCH_HIGH_RH_END          */ (4 * 35) + DAMAGE_1_POINT,
/* 053  ACTION_LOW_KICK,  STANCE_PUNCH_HIGH_LH_END          */ (4 * 35) + DAMAGE_1_POINT,
/* 054  ACTION_LOW_KICK,  STANCE_PUNCH_LOW_RH_END           */ (4 * 35) + DAMAGE_1_POINT,
/* 055  ACTION_LOW_KICK,  STANCE_PUNCH_LOW_LH_END           */ (4 * 35) + DAMAGE_1_POINT,
/* 056  ACTION_LOW_KICK,  STANCE_PUNCH_READY                */ (4 * 35) + DAMAGE_1_POINT,
/* 057  ACTION_LOW_KICK,  STANCE_STANDING_UPRIGHT           */ (4 * 35) + DAMAGE_1_POINT,
/* 058  ACTION_LOW_KICK,  STANCE_SIDLING_1                  */ (4 * 35) + DAMAGE_1_POINT,
/* 059  ACTION_LOW_KICK,  STANCE_SIDLING_2                  */ (4 * 35) + DAMAGE_1_POINT,
/* 060  ACTION_LOW_KICK,  STANCE_SIDLING_3                  */ (4 * 35) + DAMAGE_1_POINT,
/* 061  ACTION_LOW_KICK,  STANCE_SIDLING_4                  */ (4 * 35) + DAMAGE_1_POINT,
/* 062  ACTION_LOW_KICK,  STANCE_SIDLING_5                  */ (4 * 35) + DAMAGE_1_POINT,
/* 063  ACTION_LOW_KICK,  STANCE_SIDLING_6                  */ (4 * 35) + DAMAGE_1_POINT,
/* 064  ACTION_LOW_KICK,  STANCE_SIDLING_7                  */ (4 * 35) + DAMAGE_1_POINT,
/* 065  ACTION_LOW_KICK,  STANCE_RUNNING_1                  */ (4 * 35) + DAMAGE_1_POINT,
/* 066  ACTION_LOW_KICK,  STANCE_RUNNING_2                  */ (4 * 35) + DAMAGE_1_POINT,
/* 067  ACTION_LOW_KICK,  STANCE_RUNNING_3                  */ (4 * 35) + DAMAGE_1_POINT,
/* 068  ACTION_LOW_KICK,  STANCE_RUNNING_4                  */ (4 * 35) + DAMAGE_1_POINT,
/* 069  ACTION_LOW_KICK,  STANCE_RUNNING_5                  */ (4 * 35) + DAMAGE_1_POINT,
/* 070  ACTION_LOW_KICK,  STANCE_RUNNING_6                  */ (4 * 35) + DAMAGE_1_POINT,
/* 071  ACTION_LOW_KICK,  STANCE_RUNNING_7                  */ (4 * 35) + DAMAGE_1_POINT,
/* 072  ACTION_LOW_KICK,  STANCE_RUNNING_8                  */ (4 * 35) + DAMAGE_1_POINT,
/* 073  ACTION_LOW_KICK,  STANCE_RUNNING_STRAIGHTEN_UP      */ (4 * 35) + DAMAGE_1_POINT,
/* 074  ACTION_LOW_KICK,  STANCE_STANDING_UPRIGHT_REV       */ (4 * 35) + DAMAGE_1_POINT,
/* 075  ACTION_LOW_KICK,  STANCE_RUNNING_1_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 076  ACTION_LOW_KICK,  STANCE_RUNNING_2_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 077  ACTION_LOW_KICK,  STANCE_RUNNING_3_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 078  ACTION_LOW_KICK,  STANCE_RUNNING_4_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 079  ACTION_LOW_KICK,  STANCE_RUNNING_5_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 080  ACTION_LOW_KICK,  STANCE_RUNNING_6_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 081  ACTION_LOW_KICK,  STANCE_RUNNING_7_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 082  ACTION_LOW_KICK,  STANCE_RUNNING_8_REV              */ (4 * 35) + DAMAGE_1_POINT,
/* 083  ACTION_LOW_KICK,  STANCE_RUNNING_STRAIGHTEN_UP_REV  */ (4 * 37) + DAMAGE_1_POINT,


/* ACTION_HIGH_KICK ------------------------------------------------------------------------------------- */

/* 084  ACTION_HIGH_KICK, STANCE_DEFAULT,                   */ (4 * 26) + DAMAGE_3_POINT,
/* 085  ACTION_HIGH_KICK, STANCE_DEFAULT_LEAN_BACK          */ (4 * 27) + DAMAGE_3_POINT,
/* 086  ACTION_HIGH_KICK, STANCE_KICK_READY                 */ (4 * 25) + DAMAGE_3_POINT,
/* 087  ACTION_HIGH_KICK, STANCE_DEFAULT_LEAN_FORWARD       */ (4 * 27) + DAMAGE_3_POINT,
/* 088  ACTION_HIGH_KICK, STANCE_KICK_STANDING_TRANSITION   */ (4 * 27) + DAMAGE_3_POINT,
/* 089  ACTION_HIGH_KICK, STANCE_KICK_MED_END               */ (4 * 44) + DAMAGE_3_POINT,
/* 090  ACTION_HIGH_KICK, STANCE_KICK_LOW_END               */ (4 * 27) + DAMAGE_3_POINT,
/* 091  ACTION_HIGH_KICK, STANCE_KICK_HIGH_END              */ (4 * 44) + DAMAGE_3_POINT,
/* 092  ACTION_HIGH_KICK, STANCE_PUNCH_MED_RH_END           */ (4 * 27) + DAMAGE_3_POINT,
/* 093  ACTION_HIGH_KICK, STANCE_PUNCH_MED_LH_END           */ (4 * 27) + DAMAGE_3_POINT,
/* 094  ACTION_HIGH_KICK, STANCE_PUNCH_HIGH_RH_END          */ (4 * 27) + DAMAGE_3_POINT,
/* 095  ACTION_HIGH_KICK, STANCE_PUNCH_HIGH_LH_END          */ (4 * 27) + DAMAGE_3_POINT,
/* 096  ACTION_HIGH_KICK, STANCE_PUNCH_LOW_RH_END           */ (4 * 27) + DAMAGE_3_POINT,
/* 097  ACTION_HIGH_KICK, STANCE_PUNCH_LOW_LH_END           */ (4 * 27) + DAMAGE_3_POINT,
/* 098  ACTION_HIGH_KICK, STANCE_PUNCH_READY                */ (4 * 27) + DAMAGE_3_POINT,
/* 090  ACTION_HIGH_KICK, STANCE_STANDING_UPRIGHT           */ (4 * 14) + DAMAGE_MAX_POINTS,
/* 100  ACTION_HIGH_KICK, STANCE_SIDLING_1                  */ (4 * 26) + DAMAGE_3_POINT,
/* 101  ACTION_HIGH_KICK, STANCE_SIDLING_2                  */ (4 * 26) + DAMAGE_3_POINT,
/* 102  ACTION_HIGH_KICK, STANCE_SIDLING_3                  */ (4 * 26) + DAMAGE_3_POINT,
/* 103  ACTION_HIGH_KICK, STANCE_SIDLING_4                  */ (4 * 26) + DAMAGE_3_POINT,
/* 104  ACTION_HIGH_KICK, STANCE_SIDLING_5                  */ (4 * 26) + DAMAGE_3_POINT,
/* 105  ACTION_HIGH_KICK, STANCE_SIDLING_6                  */ (4 * 26) + DAMAGE_3_POINT,
/* 106  ACTION_HIGH_KICK, STANCE_SIDLING_7                  */ (4 * 26) + DAMAGE_3_POINT,
/* 107  ACTION_HIGH_KICK, STANCE_RUNNING_1                  */ (4 * 49) + DAMAGE_3_POINT,
/* 108  ACTION_HIGH_KICK, STANCE_RUNNING_2                  */ (4 * 49) + DAMAGE_3_POINT,
/* 109  ACTION_HIGH_KICK, STANCE_RUNNING_3                  */ (4 * 49) + DAMAGE_3_POINT,
/* 110  ACTION_HIGH_KICK, STANCE_RUNNING_4                  */ (4 * 49) + DAMAGE_3_POINT,
/* 111  ACTION_HIGH_KICK, STANCE_RUNNING_5                  */ (4 * 49) + DAMAGE_3_POINT,
/* 112  ACTION_HIGH_KICK, STANCE_RUNNING_6                  */ (4 * 49) + DAMAGE_3_POINT,
/* 113  ACTION_HIGH_KICK, STANCE_RUNNING_7                  */ (4 * 49) + DAMAGE_3_POINT,
/* 114  ACTION_HIGH_KICK, STANCE_RUNNING_8                  */ (4 * 49) + DAMAGE_3_POINT,
/* 115  ACTION_HIGH_KICK, STANCE_RUNNING_STRAIGHTEN_UP      */ (4 * 49) + DAMAGE_3_POINT,
/* 116  ACTION_HIGH_KICK, STANCE_STANDING_UPRIGHT_REV       */ (4 * 27) + DAMAGE_3_POINT,
/* 117  ACTION_HIGH_KICK, STANCE_RUNNING_1_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 118  ACTION_HIGH_KICK, STANCE_RUNNING_2_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 119  ACTION_HIGH_KICK, STANCE_RUNNING_3_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 120  ACTION_HIGH_KICK, STANCE_RUNNING_4_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 121  ACTION_HIGH_KICK, STANCE_RUNNING_5_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 122  ACTION_HIGH_KICK, STANCE_RUNNING_6_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 123  ACTION_HIGH_KICK, STANCE_RUNNING_7_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 124  ACTION_HIGH_KICK, STANCE_RUNNING_8_REV              */ (4 * 27) + DAMAGE_3_POINT,
/* 125  ACTION_HIGH_KICK, STANCE_RUNNING_STRAIGHTEN_UP_REV  */ (4 * 27) + DAMAGE_3_POINT,

/* ACTION_MED_PUNCH ------------------------------------------------------------------------------------- */

/* 126  ACTION_MED_PUNCH,   STANCE_DEFAULT,                   */ (4 * 31) + DAMAGE_2_POINT,
/* 127  ACTION_MED_PUNCH,   STANCE_DEFAULT_LEAN_BACK          */ (4 * 31) + DAMAGE_2_POINT,
/* 128  ACTION_MED_PUNCH,   STANCE_KICK_READY                 */ (4 * 25) + DAMAGE_2_POINT,
/* 129  ACTION_MED_PUNCH,   STANCE_DEFAULT_LEAN_FORWARD       */ (4 * 31) + DAMAGE_2_POINT,
/* 130  ACTION_MED_PUNCH,   STANCE_KICK_STANDING_TRANSITION   */ (4 * 31) + DAMAGE_2_POINT,
/* 131  ACTION_MED_PUNCH,   STANCE_KICK_MED_END               */ (4 * 44) + DAMAGE_2_POINT,
/* 132  ACTION_MED_PUNCH,   STANCE_KICK_LOW_END               */ (4 * 31) + DAMAGE_2_POINT,
/* 133  ACTION_MED_PUNCH,   STANCE_KICK_HIGH_END              */ (4 * 44) + DAMAGE_2_POINT,
/* 134  ACTION_MED_PUNCH,   STANCE_PUNCH_MED_RH_END           */ (4 * 31) + DAMAGE_2_POINT,
/* 135  ACTION_MED_PUNCH,   STANCE_PUNCH_MED_LH_END           */ (4 * 31) + DAMAGE_2_POINT,
/* 136  ACTION_MED_PUNCH,   STANCE_PUNCH_HIGH_RH_END          */ (4 * 31) + DAMAGE_2_POINT,
/* 137  ACTION_MED_PUNCH,   STANCE_PUNCH_HIGH_LH_END          */ (4 * 31) + DAMAGE_2_POINT,
/* 138  ACTION_MED_PUNCH,   STANCE_PUNCH_LOW_RH_END           */ (4 * 31) + DAMAGE_2_POINT,
/* 139  ACTION_MED_PUNCH,   STANCE_PUNCH_LOW_LH_END           */ (4 * 31) + DAMAGE_2_POINT,
/* 140  ACTION_MED_PUNCH,   STANCE_PUNCH_READY                */ (4 * 26) + DAMAGE_2_POINT,
/* 141  ACTION_MED_PUNCH,   STANCE_STANDING_UPRIGHT           */ (4 * 31) + DAMAGE_2_POINT,
/* 142  ACTION_MED_PUNCH,   STANCE_SIDLING_1                  */ (4 * 31) + DAMAGE_2_POINT,
/* 143  ACTION_MED_PUNCH,   STANCE_SIDLING_2                  */ (4 * 31) + DAMAGE_2_POINT,
/* 144  ACTION_MED_PUNCH,   STANCE_SIDLING_3                  */ (4 * 31) + DAMAGE_2_POINT,
/* 145  ACTION_MED_PUNCH,   STANCE_SIDLING_4                  */ (4 * 31) + DAMAGE_2_POINT,
/* 146  ACTION_MED_PUNCH,   STANCE_SIDLING_5                  */ (4 * 31) + DAMAGE_2_POINT,
/* 147  ACTION_MED_PUNCH,   STANCE_SIDLING_6                  */ (4 * 31) + DAMAGE_2_POINT,
/* 148  ACTION_MED_PUNCH,   STANCE_SIDLING_7                  */ (4 * 31) + DAMAGE_2_POINT,
/* 149  ACTION_MED_PUNCH,   STANCE_RUNNING_1                  */ (4 * 31) + DAMAGE_2_POINT,
/* 150  ACTION_MED_PUNCH,   STANCE_RUNNING_2                  */ (4 * 31) + DAMAGE_2_POINT,
/* 151  ACTION_MED_PUNCH,   STANCE_RUNNING_3                  */ (4 * 31) + DAMAGE_2_POINT,
/* 152  ACTION_MED_PUNCH,   STANCE_RUNNING_4                  */ (4 * 31) + DAMAGE_2_POINT,
/* 153  ACTION_MED_PUNCH,   STANCE_RUNNING_5                  */ (4 * 31) + DAMAGE_2_POINT,
/* 154  ACTION_MED_PUNCH,   STANCE_RUNNING_6                  */ (4 * 31) + DAMAGE_2_POINT,
/* 155  ACTION_MED_PUNCH,   STANCE_RUNNING_7                  */ (4 * 31) + DAMAGE_2_POINT,
/* 156  ACTION_MED_PUNCH,   STANCE_RUNNING_8                  */ (4 * 31) + DAMAGE_2_POINT,
/* 157  ACTION_MED_PUNCH,   STANCE_RUNNING_STRAIGHTEN_UP      */ (4 * 31) + DAMAGE_2_POINT,
/* 158  ACTION_MED_PUNCH,   STANCE_STANDING_UPRIGHT_REV       */ (4 * 31) + DAMAGE_2_POINT,
/* 159  ACTION_MED_PUNCH,   STANCE_RUNNING_1_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 160  ACTION_MED_PUNCH,   STANCE_RUNNING_2_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 161  ACTION_MED_PUNCH,   STANCE_RUNNING_3_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 162  ACTION_MED_PUNCH,   STANCE_RUNNING_4_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 163  ACTION_MED_PUNCH,   STANCE_RUNNING_5_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 164  ACTION_MED_PUNCH,   STANCE_RUNNING_6_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 165  ACTION_MED_PUNCH,   STANCE_RUNNING_7_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 166  ACTION_MED_PUNCH,   STANCE_RUNNING_8_REV              */ (4 * 31) + DAMAGE_2_POINT,
/* 167  ACTION_MED_PUNCH,   STANCE_RUNNING_STRAIGHTEN_UP_REV  */ (4 * 31) + DAMAGE_2_POINT,


/* ACTION_HIGH_PUNCH ------------------------------------------------------------------------------------- */

/* 168  ACTION_HIGH_PUNCH,  STANCE_DEFAULT,                   */ (4 * 30) + DAMAGE_3_POINT,
/* 169  ACTION_HIGH_PUNCH,  STANCE_DEFAULT_LEAN_BACK          */ (4 * 31) + DAMAGE_3_POINT,
/* 170  ACTION_HIGH_PUNCH,  STANCE_KICK_READY                 */ (4 * 17) + DAMAGE_3_POINT,
/* 171  ACTION_HIGH_PUNCH,  STANCE_DEFAULT_LEAN_FORWARD       */ (4 * 31) + DAMAGE_3_POINT,
/* 172  ACTION_HIGH_PUNCH,  STANCE_KICK_STANDING_TRANSITION   */ (4 * 31) + DAMAGE_3_POINT,
/* 173  ACTION_HIGH_PUNCH,  STANCE_KICK_MED_END               */ (4 * 44) + DAMAGE_3_POINT,
/* 174  ACTION_HIGH_PUNCH,  STANCE_KICK_LOW_END               */ (4 * 31) + DAMAGE_3_POINT,
/* 175  ACTION_HIGH_PUNCH,  STANCE_KICK_HIGH_END              */ (4 * 44) + DAMAGE_3_POINT,
/* 176  ACTION_HIGH_PUNCH,  STANCE_PUNCH_MED_RH_END           */ (4 * 31) + DAMAGE_3_POINT,
/* 177  ACTION_HIGH_PUNCH,  STANCE_PUNCH_MED_LH_END           */ (4 * 31) + DAMAGE_3_POINT,
/* 178  ACTION_HIGH_PUNCH,  STANCE_PUNCH_HIGH_RH_END          */ (4 * 31) + DAMAGE_3_POINT,
/* 179  ACTION_HIGH_PUNCH,  STANCE_PUNCH_HIGH_LH_END          */ (4 * 31) + DAMAGE_3_POINT,
/* 180  ACTION_HIGH_PUNCH,  STANCE_PUNCH_LOW_RH_END           */ (4 * 31) + DAMAGE_3_POINT,
/* 181  ACTION_HIGH_PUNCH,  STANCE_PUNCH_LOW_LH_END           */ (4 * 31) + DAMAGE_3_POINT,
/* 182  ACTION_HIGH_PUNCH,  STANCE_PUNCH_READY                */ (4 * 28) + DAMAGE_3_POINT,
/* 183  ACTION_HIGH_PUNCH,  STANCE_STANDING_UPRIGHT           */ (4 * 31) + DAMAGE_3_POINT,
/* 184  ACTION_HIGH_PUNCH,  STANCE_SIDLING_1                  */ (4 * 31) + DAMAGE_3_POINT,
/* 185  ACTION_HIGH_PUNCH,  STANCE_SIDLING_2                  */ (4 * 31) + DAMAGE_3_POINT,
/* 186  ACTION_HIGH_PUNCH,  STANCE_SIDLING_3                  */ (4 * 31) + DAMAGE_3_POINT,
/* 187  ACTION_HIGH_PUNCH,  STANCE_SIDLING_4                  */ (4 * 31) + DAMAGE_3_POINT,
/* 188  ACTION_HIGH_PUNCH,  STANCE_SIDLING_5                  */ (4 * 31) + DAMAGE_3_POINT,
/* 189  ACTION_HIGH_PUNCH,  STANCE_SIDLING_6                  */ (4 * 31) + DAMAGE_3_POINT,
/* 190  ACTION_HIGH_PUNCH,  STANCE_SIDLING_7                  */ (4 * 31) + DAMAGE_3_POINT,
/* 191  ACTION_HIGH_PUNCH,  STANCE_RUNNING_1                  */ (4 * 31) + DAMAGE_3_POINT,
/* 192  ACTION_HIGH_PUNCH,  STANCE_RUNNING_2                  */ (4 * 31) + DAMAGE_3_POINT,
/* 193  ACTION_HIGH_PUNCH,  STANCE_RUNNING_3                  */ (4 * 31) + DAMAGE_3_POINT,
/* 194  ACTION_HIGH_PUNCH,  STANCE_RUNNING_4                  */ (4 * 31) + DAMAGE_3_POINT,
/* 195  ACTION_HIGH_PUNCH,  STANCE_RUNNING_5                  */ (4 * 31) + DAMAGE_3_POINT,
/* 196  ACTION_HIGH_PUNCH,  STANCE_RUNNING_6                  */ (4 * 31) + DAMAGE_3_POINT,
/* 197  ACTION_HIGH_PUNCH,  STANCE_RUNNING_7                  */ (4 * 31) + DAMAGE_3_POINT,
/* 198  ACTION_HIGH_PUNCH,  STANCE_RUNNING_8                  */ (4 * 31) + DAMAGE_3_POINT,
/* 199  ACTION_HIGH_PUNCH,  STANCE_RUNNING_STRAIGHTEN_UP      */ (4 * 31) + DAMAGE_3_POINT,
/* 200  ACTION_HIGH_PUNCH,  STANCE_STANDING_UPRIGHT_REV       */ (4 * 31) + DAMAGE_3_POINT,
/* 201  ACTION_HIGH_PUNCH,  STANCE_RUNNING_1_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 202  ACTION_HIGH_PUNCH,  STANCE_RUNNING_2_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 203  ACTION_HIGH_PUNCH,  STANCE_RUNNING_3_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 204  ACTION_HIGH_PUNCH,  STANCE_RUNNING_4_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 205  ACTION_HIGH_PUNCH,  STANCE_RUNNING_5_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 206  ACTION_HIGH_PUNCH,  STANCE_RUNNING_6_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 207  ACTION_HIGH_PUNCH,  STANCE_RUNNING_7_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 208  ACTION_HIGH_PUNCH,  STANCE_RUNNING_8_REV              */ (4 * 31) + DAMAGE_3_POINT,
/* 209  ACTION_HIGH_PUNCH,  STANCE_RUNNING_STRAIGHTEN_UP_REV  */ (4 * 31) + DAMAGE_3_POINT,


/* ACTION_LOW_PUNCH ------------------------------------------------------------------------------------- */

/* 210  ACTION_LOW_PUNCH,   STANCE_DEFAULT,                   */ (4 * 29) + DAMAGE_1_POINT,
/* 211  ACTION_LOW_PUNCH,   STANCE_DEFAULT_LEAN_BACK          */ (4 * 29) + DAMAGE_1_POINT,
/* 212  ACTION_LOW_PUNCH,   STANCE_KICK_READY                 */ (4 * 27) + DAMAGE_1_POINT,
/* 213  ACTION_LOW_PUNCH,   STANCE_DEFAULT_LEAN_FORWARD       */ (4 * 29) + DAMAGE_1_POINT,
/* 214  ACTION_LOW_PUNCH,   STANCE_KICK_STANDING_TRANSITION   */ (4 * 29) + DAMAGE_1_POINT,
/* 215  ACTION_LOW_PUNCH,   STANCE_KICK_MED_END               */ (4 * 44) + DAMAGE_1_POINT,
/* 216  ACTION_LOW_PUNCH,   STANCE_KICK_LOW_END               */ (4 * 29) + DAMAGE_1_POINT,
/* 217  ACTION_LOW_PUNCH,   STANCE_KICK_HIGH_END              */ (4 * 44) + DAMAGE_1_POINT,
/* 218  ACTION_LOW_PUNCH,   STANCE_PUNCH_MED_RH_END           */ (4 * 29) + DAMAGE_1_POINT,
/* 219  ACTION_LOW_PUNCH,   STANCE_PUNCH_MED_LH_END           */ (4 * 29) + DAMAGE_1_POINT,
/* 220  ACTION_LOW_PUNCH,   STANCE_PUNCH_HIGH_RH_END          */ (4 * 29) + DAMAGE_1_POINT,
/* 221  ACTION_LOW_PUNCH,   STANCE_PUNCH_HIGH_LH_END          */ (4 * 29) + DAMAGE_1_POINT,
/* 222  ACTION_LOW_PUNCH,   STANCE_PUNCH_LOW_RH_END           */ (4 * 29) + DAMAGE_1_POINT,
/* 223  ACTION_LOW_PUNCH,   STANCE_PUNCH_LOW_LH_END           */ (4 * 29) + DAMAGE_1_POINT,
/* 224  ACTION_LOW_PUNCH,   STANCE_PUNCH_READY                */ (4 * 25) + DAMAGE_1_POINT,
/* 225  ACTION_LOW_PUNCH,   STANCE_STANDING_UPRIGHT           */ (4 * 29) + DAMAGE_1_POINT,
/* 226  ACTION_LOW_PUNCH,   STANCE_SIDLING_1                  */ (4 * 29) + DAMAGE_1_POINT,
/* 227  ACTION_LOW_PUNCH,   STANCE_SIDLING_2                  */ (4 * 29) + DAMAGE_1_POINT,
/* 228  ACTION_LOW_PUNCH,   STANCE_SIDLING_3                  */ (4 * 29) + DAMAGE_1_POINT,
/* 229  ACTION_LOW_PUNCH,   STANCE_SIDLING_4                  */ (4 * 29) + DAMAGE_1_POINT,
/* 230  ACTION_LOW_PUNCH,   STANCE_SIDLING_5                  */ (4 * 29) + DAMAGE_1_POINT,
/* 231  ACTION_LOW_PUNCH,   STANCE_SIDLING_6                  */ (4 * 29) + DAMAGE_1_POINT,
/* 232  ACTION_LOW_PUNCH,   STANCE_SIDLING_7                  */ (4 * 29) + DAMAGE_1_POINT,
/* 233  ACTION_LOW_PUNCH,   STANCE_RUNNING_1                  */ (4 * 29) + DAMAGE_1_POINT,
/* 234  ACTION_LOW_PUNCH,   STANCE_RUNNING_2                  */ (4 * 29) + DAMAGE_1_POINT,
/* 235  ACTION_LOW_PUNCH,   STANCE_RUNNING_3                  */ (4 * 29) + DAMAGE_1_POINT,
/* 236  ACTION_LOW_PUNCH,   STANCE_RUNNING_4                  */ (4 * 29) + DAMAGE_1_POINT,
/* 237  ACTION_LOW_PUNCH,   STANCE_RUNNING_5                  */ (4 * 29) + DAMAGE_1_POINT,
/* 238  ACTION_LOW_PUNCH,   STANCE_RUNNING_6                  */ (4 * 29) + DAMAGE_1_POINT,
/* 239  ACTION_LOW_PUNCH,   STANCE_RUNNING_7                  */ (4 * 29) + DAMAGE_1_POINT,
/* 240  ACTION_LOW_PUNCH,   STANCE_RUNNING_8                  */ (4 * 29) + DAMAGE_1_POINT,
/* 241  ACTION_LOW_PUNCH,   STANCE_RUNNING_STRAIGHTEN_UP      */ (4 * 29) + DAMAGE_1_POINT,
/* 242  ACTION_LOW_PUNCH,   STANCE_STANDING_UPRIGHT_REV       */ (4 * 29) + DAMAGE_1_POINT,
/* 243  ACTION_LOW_PUNCH,   STANCE_RUNNING_1_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 244  ACTION_LOW_PUNCH,   STANCE_RUNNING_2_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 245  ACTION_LOW_PUNCH,   STANCE_RUNNING_3_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 246  ACTION_LOW_PUNCH,   STANCE_RUNNING_4_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 247  ACTION_LOW_PUNCH,   STANCE_RUNNING_5_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 248  ACTION_LOW_PUNCH,   STANCE_RUNNING_6_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 249  ACTION_LOW_PUNCH,   STANCE_RUNNING_7_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 250  ACTION_LOW_PUNCH,   STANCE_RUNNING_8_REV              */ (4 * 29) + DAMAGE_1_POINT,
/* 251  ACTION_LOW_PUNCH,   STANCE_RUNNING_STRAIGHTEN_UP_REV  */ (4 * 29) + DAMAGE_1_POINT,


};


template<typename T> T absT(const T & v) {
  
    return (v < 0) ? -v : v;
  
  }

/* ----------------------------------------------------------------------------
 *  Draw a horizontal dotted line. 
 *  
 *  So much nicer than a solid line!
 */
void drawHorizontalDottedLine(int x1, int x2, int y) {
    
  for (int z = x1; z <= x2; z+=2) {
    arduboy.drawPixel(z, y, WHITE);
  }
  
}


uint8_t inStrikingRange(uint8_t action, int16_t attackerXPos, uint8_t targetType, uint8_t targetStance, int16_t targetXPos) {

  int16_t distBetween = absT(targetXPos - attackerXPos);

  if (targetType == ENEMY_TYPE_PERSON) {

    uint16_t pos = (action * ACTIONS_NUMBER_OF_STANCES) + targetStance;
    uint8_t distance = pgm_read_byte(&actions_distance[ pos ]) >> 2;
    uint8_t damage = pgm_read_byte(&actions_distance[ pos ]) & 0x03;

    #ifdef DEBUG_STRIKING_RANGE
      _distBetween = distBetween;
      _distTest = distance;
      _pos = pos;
      _targetStance = targetStance;
      _action = action;
    #endif

    #ifdef DEBUG_HITS
      _distBetween = distBetween;
      _distTest = distance;
      _pos = pos;
      _targetStance = targetStance;
    #endif

    #ifdef DEBUG_SERIAL
      Serial.print("inStrikingRange: "); 
      Serial.print(pos); 
      Serial.print(" "); 
      Serial.print(distance); 
      Serial.print(" "); 
      Serial.print(distBetween); 
      Serial.print(" "); 
      Serial.println(damage); 
    #endif

    return ((distBetween <= distance) ? (damage == 0 ? 255 : damage) : 0);
    
  }
  else {
    

    // Attacking eagle ..

    if (action != ACTION_LOW_PUNCH) {
      return (distBetween > 35 && distBetween < 43 ? DAMAGE_3_POINT : 0); 
    }
    else {
      return (distBetween > 30 && distBetween < 36 ? DAMAGE_3_POINT : 0); 
    }


  }

  return 0;

}


void returnFromAction(uint8_t action, uint8_t returnAction) {

  switch (returnAction) {  // Return to the default or kick ready position?
      
    case ACTION_RETURN_TO_DEFAULT:
    
      switch (action) {
        
        case ACTION_HIGH_KICK:
        case ACTION_MED_KICK:
        case ACTION_LOW_KICK:
          enemyStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD);
          break;

        case ACTION_HIGH_PUNCH:
        case ACTION_MED_PUNCH:
        case ACTION_LOW_PUNCH:
          enemyStack.push(STANCE_DEFAULT, STANCE_DEFAULT_LEAN_FORWARD, STANCE_PUNCH_READY);
          break;

      }

      break;

    case ACTION_RETURN_TO_ACTION_READY:

      switch (action) {
      
        case ACTION_HIGH_KICK:
        case ACTION_MED_KICK:
        case ACTION_LOW_KICK:
          enemyStack.push(STANCE_KICK_READY);
          break;

        case ACTION_HIGH_PUNCH:
        case ACTION_MED_PUNCH:
        case ACTION_LOW_PUNCH:
          enemyStack.push(STANCE_PUNCH_READY);
          break;

      }

      break;

  }

}


uint8_t getActionFromStance(uint8_t stance) {
  
  switch (stance) {

    case STANCE_KICK_MED_END:
    case STANCE_KICK_HIGH_END:
    case STANCE_KICK_LOW_END:
      return stance - STANCE_KICK_MED_END;

    case STANCE_PUNCH_MED_RH_END:
    case STANCE_PUNCH_MED_LH_END:
      return ACTION_MED_PUNCH;

    case STANCE_PUNCH_HIGH_RH_END:
    case STANCE_PUNCH_HIGH_LH_END:
      return ACTION_HIGH_PUNCH;

    case STANCE_PUNCH_LOW_RH_END:
    case STANCE_PUNCH_LOW_LH_END:
      return ACTION_LOW_PUNCH;

    default:
      return 9;

  }

}