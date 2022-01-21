#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "LightShow.h"
#include "PinballMachineBase.h"
#include "SelfTestAndAudit.h"
#include <EEPROM.h>

#define PINBALL_MACHINE_BASE_MAJOR_VERSION  2022
#define PINBALL_MACHINE_BASE_MINOR_VERSION  1
#define DEBUG_MESSAGES  1


/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - self-test modes
//  positive - game play
char MachineState = 0;
boolean MachineStateChanged = true;
#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110

// The lower 4 bits of the Game Mode are modes, the upper 4 are for frenzies
// and other flags that carry through different modes
#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 4
#define GAME_MODE_WIZARD_START      12
#define GAME_MODE_WIZARD            13
#define GAME_MODE_WIZARD_FINISHED   14
#define GAME_BASE_MODE              0x0F

#define EEPROM_BALL_SAVE_BYTE           100
#define EEPROM_FREE_PLAY_BYTE           101
#define EEPROM_MUSIC_LEVEL_BYTE         102
#define EEPROM_SKILL_SHOT_BYTE          103
#define EEPROM_TILT_WARNING_BYTE        104
#define EEPROM_AWARD_OVERRIDE_BYTE      105
#define EEPROM_BALLS_OVERRIDE_BYTE      106
#define EEPROM_TOURNAMENT_SCORING_BYTE  107
#define EEPROM_SCROLLING_SCORES_BYTE    110
#define EEPROM_DIM_LEVEL_BYTE           113
#define EEPROM_EXTRA_BALL_SCORE_BYTE    140
#define EEPROM_SPECIAL_SCORE_BYTE       144

#define SOUND_EFFECT_NONE                 0
#define SOUND_EFFECT_BONUS_COUNT          1
#define SOUND_EFFECT_INLANE_UNLIT         3
#define SOUND_EFFECT_OUTLANE_UNLIT        4
#define SOUND_EFFECT_INLANE_LIT           5
#define SOUND_EFFECT_OUTLANE_LIT          6
#define SOUND_EFFECT_BUMPER_HIT           7
#define SOUND_EFFECT_ADD_CREDIT           10
#define SOUND_EFFECT_BALL_OVER            19
#define SOUND_EFFECT_GAME_OVER            20
#define SOUND_EFFECT_EXTRA_BALL           23
#define SOUND_EFFECT_MACHINE_START        24
#define SOUND_EFFECT_SKILL_SHOT           25
#define SOUND_EFFECT_TILT_WARNING         28
#define SOUND_EFFECT_MATCH_SPIN           30
#define SOUND_EFFECT_SPINNER_HIGH         32
#define SOUND_EFFECT_SPINNER_LOW          33
#define SOUND_EFFECT_SLING_SHOT           34
#define SOUND_EFFECT_ROLLOVER             35
#define SOUND_EFFECT_10PT_SWITCH          36
#define SOUND_EFFECT_ADD_PLAYER_1         50
#define SOUND_EFFECT_ADD_PLAYER_2         51
#define SOUND_EFFECT_ADD_PLAYER_3         52
#define SOUND_EFFECT_ADD_PLAYER_4         53
#define SOUND_EFFECT_PLAYER_1_UP          54
#define SOUND_EFFECT_PLAYER_2_UP          55
#define SOUND_EFFECT_PLAYER_3_UP          56
#define SOUND_EFFECT_PLAYER_4_UP          56
#define SOUND_EFFECT_SHOOT_AGAIN          60
#define SOUND_EFFECT_TILT                 61
#define SOUND_EFFECT_VOICE_EXTRA_BALL     81
#define SOUND_EFFECT_WIZARD_MODE_START    88
#define SOUND_EFFECT_WIZARD_MODE_FINISHED 89
#define SOUND_EFFECT_BACKGROUND_1         90
#define SOUND_EFFECT_BACKGROUND_2         91
#define SOUND_EFFECT_BACKGROUND_3         92
#define SOUND_EFFECT_BACKGROUND_WIZ       93

#define MAX_DISPLAY_BONUS          55
#define TILT_WARNING_DEBOUNCE_TIME 1000


/*********************************************************************

    Machine state and options

*********************************************************************/
unsigned long HighScore = 0;
byte Credits = 0;
boolean FreePlayMode = true;
byte BallSaveNumSeconds = 5;
unsigned long CurrentTime = 0;
byte MaximumCredits = 99;
byte BallsPerGame = 3;
boolean ScrollingScores = true;


/*********************************************************************

    Game State

*********************************************************************/
byte CurrentPlayer = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;
byte Bonus[4];
byte CurrentBonus;
byte BonusX[4];
byte GameMode = GAME_MODE_SKILL_SHOT;
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;

boolean SamePlayerShootsAgain = false;
boolean BallSaveUsed = false;
boolean CurrentlyShowingBallSave = false;
boolean ExtraBallCollected = false;
boolean SpecialCollected = false;
boolean ShowingModeStats = false;

unsigned long CurrentScores[4];
unsigned long BallFirstSwitchHitTime = 0;
unsigned long BallTimeInTrough = 0;
unsigned long GameModeStartTime = 0;
unsigned long GameModeEndTime = 0;
unsigned long LastTiltWarningTime = 0;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;
unsigned long LastRemainingAnimatedScoreShown;
unsigned long ScoreMultiplier;


/*********************************************************************

    Game Specific State Variables

*********************************************************************/
byte LanePhase;
byte RolloverPhase;
byte TenPointPhase;
byte LastAwardShotCalloutPlayed;
byte LastWizardTimer;

boolean WizardScoring;

unsigned long LastInlaneHitTime;
unsigned long BonusXAnimationStart;
unsigned long LastSpinnerHit;

#define WIZARD_START_DURATION             5000
#define WIZARD_DURATION                   39000
#define WIZARD_DURATION_SECONDS           39
#define WIZARD_FINISHED_DURATION          5000
#define WIZARD_SWITCH_SCORE               5000
#define WIZARD_MODE_REWARD_SCORE          250000

#define SPINNER_MAX_GOAL                  100

void ReadStoredParameters() {
  HighScore = BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, 10000);
  Credits = BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE);
  if (Credits > MaximumCredits) Credits = MaximumCredits;
}

struct PlayfieldAndCabinetSwitch SolenoidAssociatedSwitches[] = {
  { SW_RIGHT_SLINGSHOT, SOL_RIGHT_SLINGSHOT, 4},
  { SW_LEFT_SLINGSHOT, SOL_LEFT_SLINGSHOT, 4},
  { SW_LEFT_THUMPER_BUMPER, SOL_LEFT_THUMPER_BUMPER, 3},
  { SW_RIGHT_THUMPER_BUMPER, SOL_RIGHT_THUMPER_BUMPER, 3}
};

void setup() {
  if (DEBUG_MESSAGES) {
    Serial.begin(57600);
  }


  // Tell the OS about game-specific lights and switches
  BSOS_SetupGameSwitches(
      NUM_SWITCHES_WITH_TRIGGERS,
      NUM_PRIORITY_SWITCHES_WITH_TRIGGERS,
      SolenoidAssociatedSwitches
      );

  // Set up the chips and interrupts
  BSOS_InitializeMPU();
  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);

  // Read parameters from EEProm
  ReadStoredParameters();
  BSOS_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  CurrentScores[0] = PINBALL_MACHINE_BASE_MAJOR_VERSION;
  CurrentScores[1] = PINBALL_MACHINE_BASE_MINOR_VERSION;
  CurrentScores[2] = BALLY_STERN_OS_MAJOR_VERSION;
  CurrentScores[3] = BALLY_STERN_OS_MINOR_VERSION;

  CurrentTime = millis();
}


////////////////////////////////////////////////////////////////////////////
//
//  Lamp Management functions
//
////////////////////////////////////////////////////////////////////////////
void ShowLampsTest() {
  int* lampsPointer = allBonusLights();

  for(int i = 0; lampsPointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(lampsPointer[i], 1);
  }
}


////////////////////////////////////////////////////////////////////////////
//
//  Display Management functions
//
////////////////////////////////////////////////////////////////////////////
unsigned long LastTimeScoreChanged = 0;
unsigned long LastTimeOverrideAnimated = 0;
unsigned long LastFlashOrDash = 0;
byte LastScrollPhase = 0;


byte MagnitudeOfScore(unsigned long score) {
  if (score == 0) return 0;

  byte retval = 0;
  while (score > 0) {
    score = score / 10;
    retval += 1;
  }
  return retval;
}


byte GetDisplayMask(byte numDigits) {
  byte displayMask = 0;
  for (byte digitCount = 0; digitCount < numDigits; digitCount++) {
    displayMask |= (0x20 >> digitCount);
  }
  return displayMask;
}


void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0) {
  byte displayMask = 0x3F;
  unsigned long displayScore = 0;

  boolean updateLastTimeAnimated = false;
  for (byte scoreCount = 0; scoreCount < 4; scoreCount++) {
    if (allScoresShowValue == 0) displayScore = CurrentScores[scoreCount];
    else displayScore = allScoresShowValue;

    // If we're updating all displays, or the one currently matching the loop
    if (displayToUpdate == 0xFF || displayToUpdate == scoreCount) {

      // Don't show this score if it's not a current player score (even if it's scrollable)
      if (displayToUpdate == 0xFF && (scoreCount >= CurrentNumPlayers && CurrentNumPlayers != 0) && allScoresShowValue == 0) {
        BSOS_SetDisplayBlank(scoreCount, 0x00);
        continue;
      }

      if (flashCurrent) {
        unsigned long flashSeed = CurrentTime / 250;
        if (flashSeed != LastFlashOrDash) {
          LastFlashOrDash = flashSeed;
          if (((CurrentTime / 250) % 2) == 0) BSOS_SetDisplayBlank(scoreCount, 0x00);
          else BSOS_SetDisplay(scoreCount, displayScore, true, 2);
        }
      } else if (dashCurrent) {
        unsigned long dashSeed = CurrentTime / 50;
        if (dashSeed != LastFlashOrDash) {
          LastFlashOrDash = dashSeed;
          byte dashPhase = (CurrentTime / 60) % 36;
          byte numDigits = MagnitudeOfScore(displayScore);
          if (dashPhase < 12) {
            displayMask = GetDisplayMask((numDigits == 0) ? 2 : numDigits);
            if (dashPhase < 7) {
              for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
                displayMask &= ~(0x01 << maskCount);
              }
            } else {
              for (byte maskCount = 12; maskCount > dashPhase; maskCount--) {
                displayMask &= ~(0x20 >> (maskCount - dashPhase - 1));
              }
            }
            BSOS_SetDisplay(scoreCount, displayScore);
            BSOS_SetDisplayBlank(scoreCount, displayMask);
          } else {
            BSOS_SetDisplay(scoreCount, displayScore, true, 2);
          }
        }
      } else {
        BSOS_SetDisplay(scoreCount, displayScore, true, 2);
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////
//
//  Machine State Helper functions
//
////////////////////////////////////////////////////////////////////////////
boolean AddPlayer(boolean resetNumPlayers = false) {
  if (Credits < 1 && !FreePlayMode) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers >= 4) return false;

  CurrentNumPlayers += 1;
  BSOS_SetDisplay(CurrentNumPlayers - 1, 0);
  BSOS_SetDisplayBlank(CurrentNumPlayers - 1, 0x30);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}


void AddCoinToAudit(byte switchHit) {
  unsigned short coinAuditStartByte = 0;

  switch (switchHit) {
    case SW_COIN_3: coinAuditStartByte = BSOS_CHUTE_3_COINS_START_BYTE; break;
    case SW_COIN_2: coinAuditStartByte = BSOS_CHUTE_2_COINS_START_BYTE; break;
    case SW_COIN_1: coinAuditStartByte = BSOS_CHUTE_1_COINS_START_BYTE; break;
  }

  if (coinAuditStartByte) {
    BSOS_WriteULToEEProm(coinAuditStartByte, BSOS_ReadULFromEEProm(coinAuditStartByte) + 1);
  }

}


void AddCredit(boolean playSound = false, byte numToAdd = 1) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
    BSOS_SetDisplayCredits(Credits);
    BSOS_SetCoinLockout(false);
  } else {
    BSOS_SetDisplayCredits(Credits);
    BSOS_SetCoinLockout(true);
  }

}


void AwardExtraBall() {
  if (ExtraBallCollected) return;

  ExtraBallCollected = true;
  SamePlayerShootsAgain = true;
  BSOS_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
}


////////////////////////////////////////////////////////////////////////////
//
//  Test functions
//
////////////////////////////////////////////////////////////////////////////
#define ADJ_TYPE_LIST                 1
#define ADJ_TYPE_MIN_MAX              2
#define ADJ_TYPE_MIN_MAX_DEFAULT      3
#define ADJ_TYPE_SCORE                4
#define ADJ_TYPE_SCORE_WITH_DEFAULT   5
#define ADJ_TYPE_SCORE_NO_DEFAULT     6
byte AdjustmentType = 0;
byte NumAdjustmentValues = 0;
byte AdjustmentValues[8];
unsigned long AdjustmentScore;
byte *CurrentAdjustmentByte = NULL;
unsigned long *CurrentAdjustmentUL = NULL;
byte CurrentAdjustmentStorageByte = 0;
byte TempValue = 0;


int RunSelfTest(int curState, boolean curStateChanged) {
  int returnState = curState;
  CurrentNumPlayers = 0;

  if (curState >= MACHINE_STATE_TEST_CHUTE_3_COINS) {
    returnState = RunBaseSelfTest(returnState, curStateChanged, CurrentTime, SW_CREDIT_BUTTON, SW_SLAM);
  }

  if (returnState == MACHINE_STATE_ATTRACT) {
    BSOS_SetDisplayCredits(Credits, true);
    ReadStoredParameters();
  }

  return returnState;
}


////////////////////////////////////////////////////////////////////////////
//
//  Audio Output functions
//
////////////////////////////////////////////////////////////////////////////
void PlaySound(byte soundEffectNum) {
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }
  BSOS_PlaySoundSquawkAndTalk(soundEffectNum);
}


void StopAudio() {
  BSOS_PlaySoundSquawkAndTalk(5);
}


////////////////////////////////////////////////////////////////////////////
//
//  Attract Mode
//
////////////////////////////////////////////////////////////////////////////
int RunAttractMode(int curState, boolean curStateChanged) {
  int returnState = curState;

  if (curStateChanged) {
    BSOS_DisableSolenoidStack();
    BSOS_TurnOffAllLamps();
    BSOS_SetDisableFlippers(true);
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract Mode\n\r");
    }
    BSOS_SetDisplayCredits(Credits, true);
  }

  byte switchHit;

  switchHit = BSOS_PullFirstFromSwitchStack();
  while (switchHit != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
    case(SW_CREDIT_BUTTON):
      if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      AddCoinToAudit(switchHit);
      AddCredit(true, 1);
      break;
    case SW_SELF_TEST_SWITCH:
      if (CurrentTime - GetLastSelfTestChangedTime() > 250) {
        returnState = MACHINE_STATE_TEST_LIGHTS;
        SetLastSelfTestChangedTime(CurrentTime);
      }
      break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return returnState;
}





////////////////////////////////////////////////////////////////////////////
//
//  Game Play functions
//
////////////////////////////////////////////////////////////////////////////
void AddToBonus(byte amountToAdd = 1) {
  CurrentBonus += amountToAdd;
  if (CurrentBonus >= MAX_DISPLAY_BONUS) {
    CurrentBonus = MAX_DISPLAY_BONUS;
  }
}


void SetGameMode(byte newGameMode) {
  GameMode = newGameMode | (GameMode & ~GAME_BASE_MODE);
  GameModeStartTime = 0;
  GameModeEndTime = 0;
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Game mode set to %d\n", newGameMode);
    Serial.write(buf);
  }
}


void StartScoreAnimation(unsigned long scoreToAnimate) {
  if (ScoreAdditionAnimation != 0) {
    CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
  }
  ScoreAdditionAnimation = scoreToAnimate;
  ScoreAdditionAnimationStartTime = CurrentTime;
  LastRemainingAnimatedScoreShown = 0;
}


void IncreaseBonusX() {
  boolean soundPlayed = false;
  if (BonusX[CurrentPlayer] < 5) {
    BonusX[CurrentPlayer] += 1;
    BonusXAnimationStart = CurrentTime;

    if (BonusX[CurrentPlayer] == 4) {
      BonusX[CurrentPlayer] += 1;
    }
  }
}


int InitGamePlay() {
  if (DEBUG_MESSAGES) {
    Serial.write("Starting game\n\r");
  }

  BSOS_SetDisableFlippers(false);
  BSOS_EnableSolenoidStack();
  BSOS_TurnOffAllLamps();

  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, CurrentTime + 500);
  BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 10, CurrentTime + 500);
  BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 10, CurrentTime + 500);

  for (int count = 0; count < 4; count++) {
    BonusX[count] = 1;
    Bonus[count] = 0;
  }
  memset(CurrentScores, 0, 4 * sizeof(unsigned long));

  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
  CurrentPlayer = 0;
  ShowPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}


int InitNewBall(bool curStateChanged, byte playerNum, int ballNum) {
  if (curStateChanged) {
    BSOS_TurnOffAllLamps();
    ShowLampsTest();

    BallFirstSwitchHitTime = 0;

    BSOS_SetDisplayCredits(Credits, true);
    SamePlayerShootsAgain = false;

    BSOS_SetDisplayBallInPlay(ballNum);
    BSOS_SetLampState(LAMP_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      BSOS_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    BallSaveUsed = false;
    BallTimeInTrough = 0;
    NumTiltWarnings = 0;
    LastTiltWarningTime = 0;

    // Initialize game-specific start-of-ball lights & variables
    GameModeStartTime = 0;
    GameModeEndTime = 0;
    GameMode = GAME_MODE_SKILL_SHOT;

    ExtraBallCollected = false;
    SpecialCollected = false;

    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
      BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, CurrentTime + 600);
    }

    // Reset progress unless holdover awards
    Bonus[CurrentPlayer] = 0;
    BonusX[CurrentPlayer] = 1;

    ScoreMultiplier = 1;
    LanePhase = 0;
    LastInlaneHitTime = 0;
    CurrentBonus = Bonus[CurrentPlayer];
    ScoreAdditionAnimation = 0;
    ScoreAdditionAnimationStartTime = 0;
    BonusXAnimationStart = 0;
    LastSpinnerHit = 0;
    //    PlayBackgroundSongBasedOnLevel(StarLevel[CurrentPlayer]);
    TenPointPhase = 0;
    WizardScoring = false;
  }

  // We should only consider the ball initialized when
  // the ball is no longer triggering the SW_OUTHOLE
  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

}


// This function manages all timers, flags, and lights
int ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  switch ( (GameMode & GAME_BASE_MODE) ) {
    case GAME_MODE_SKILL_SHOT:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
      }

      if (BallFirstSwitchHitTime != 0) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }

      if (GameModeEndTime != 0 && CurrentTime > GameModeEndTime) {
        ShowPlayerScores(0xFF, false, false);
      }
      break;
  }

  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    if (BallTimeInTrough == 0) {
      BallTimeInTrough = CurrentTime;
    } else {
      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing
      if ((CurrentTime - BallTimeInTrough) > 500) {
        if (BallFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings) {
          // Nothing hit yet, so return the ball to the player
          BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, CurrentTime);
          BallTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
          ScoreAdditionAnimationStartTime = 0;
          ScoreAdditionAnimation = 0;
          ShowPlayerScores(0xFF, false, false);
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (!BallSaveUsed && ((CurrentTime - BallFirstSwitchHitTime)) < ((unsigned long)BallSaveNumSeconds * 1000)) {
            BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, CurrentTime + 100);
            BallSaveUsed = true;
            BSOS_SetLampState(LAMP_SHOOT_AGAIN, 0);
            BallTimeInTrough = CurrentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
          } else {
            ShowPlayerScores(0xFF, false, false);

            returnState = MACHINE_STATE_COUNTDOWN_BONUS;
          }
        }
      }
    }
  } else {
    BallTimeInTrough = 0;
  }

  return returnState;
}


unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long BonusCountDownEndTime = 0;

int CountdownBonus(boolean curStateChanged) {

  // If this is the first time through the countdown loop
  if (curStateChanged) {

    Bonus[CurrentPlayer] = CurrentBonus;
    CountdownStartTime = CurrentTime;

    LastCountdownReportTime = CountdownStartTime;
    BonusCountDownEndTime = 0xFFFFFFFF;
  }

  unsigned long countdownDelayTime = 250 - (CurrentBonus * 3);

  if ((CurrentTime - LastCountdownReportTime) > countdownDelayTime) {

    if (CurrentBonus) {

      // Only give sound & score if this isn't a tilt
      if (NumTiltWarnings <= MaxTiltWarnings) {
        CurrentScores[CurrentPlayer] += 1000 * ((unsigned long)BonusX[CurrentPlayer]);
      }

      CurrentBonus -= 1;

    } else if (BonusCountDownEndTime == 0xFFFFFFFF) {
      BonusCountDownEndTime = CurrentTime + 1000;
    }
    LastCountdownReportTime = CurrentTime;
  }

  if (CurrentTime > BonusCountDownEndTime) {

    // Reset any lights & variables of goals that weren't completed
    BonusCountDownEndTime = 0xFFFFFFFF;
    return MACHINE_STATE_BALL_OVER;
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}



unsigned long MatchSequenceStartTime = 0;
unsigned long MatchDelay = 150;
byte MatchDigit = 0;
byte NumMatchSpins = 0;
byte ScoreMatches = 0;



int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = InitGamePlay();
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = InitNewBall(curStateChanged, CurrentPlayer, CurrentBallInPlay);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    returnState = CountdownBonus(curStateChanged);
    ShowPlayerScores(0xFF, false, false);
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    BSOS_SetDisplayCredits(Credits);

    if (SamePlayerShootsAgain) {
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {
      CurrentPlayer += 1;
      if (CurrentPlayer >= CurrentNumPlayers) {
        CurrentPlayer = 0;
        CurrentBallInPlay += 1;
      }

      if (CurrentBallInPlay > BallsPerGame) {
        for (int count = 0; count < CurrentNumPlayers; count++) {
          BSOS_SetDisplay(count, CurrentScores[count], true, 2);
        }

        returnState = MACHINE_STATE_INIT_GAMEPLAY;
      }
      else returnState = MACHINE_STATE_INIT_NEW_BALL;
    }
  } else if (curState == MACHINE_STATE_MATCH_MODE) {
  }

  byte switchHit;
  if (NumTiltWarnings <= MaxTiltWarnings) {
    while ( (switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {

      if (DEBUG_MESSAGES) {
        char buf[128];
        sprintf(buf, "Switch Hit = %d\n", switchHit);
        Serial.write(buf);
      }

      /* switch (switchHit) { */
      /*   case SW_TILT: */
      /*     // This should be debounced */
      /*     if ((CurrentTime - LastTiltWarningTime) > TILT_WARNING_DEBOUNCE_TIME) { */
      /*       LastTiltWarningTime = CurrentTime; */
      /*       NumTiltWarnings += 1; */
      /*       if (NumTiltWarnings > MaxTiltWarnings) { */
      /*         BSOS_DisableSolenoidStack(); */
      /*         BSOS_SetDisableFlippers(true); */
      /*         BSOS_TurnOffAllLamps(); */
      /*         BSOS_SetLampState(LAMP_TILT, 1); */
      /*       } */
      /*     } */
      /*     break; */
      /*   case SW_SELF_TEST_SWITCH: */
      /*     returnState = MACHINE_STATE_TEST_LIGHTS; */
      /*     SetLastSelfTestChangedTime(CurrentTime); */
      /*     break; */
      /*   case SW_LEFT_SLINGSHOT: */
      /*   case SW_RIGHT_SLINGSHOT: */
      /*     CurrentScores[CurrentPlayer] += 10; */
      /*     if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime; */
      /*     break; */
      /*   case SW_COIN_1: */
      /*   case SW_COIN_2: */
      /*   case SW_COIN_3: */
      /*     AddCoinToAudit(switchHit); */
      /*     AddCredit(true, 1); */
      /*     break; */
      /*   case SW_CREDIT_BUTTON: */
      /*     if (CurrentBallInPlay < 2) { */
      /*       // If we haven't finished the first ball, we can add players */
      /*       AddPlayer(); */
      /*     } else { */
      /*       // If the first ball is over, pressing start again resets the game */
      /*       if (Credits >= 1 || FreePlayMode) { */
      /*         if (!FreePlayMode) { */
      /*           Credits -= 1; */
      /*           BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits); */
      /*           BSOS_SetDisplayCredits(Credits); */
      /*         } */
      /*         returnState = MACHINE_STATE_INIT_GAMEPLAY; */
      /*       } */
      /*     } */
      /*     if (DEBUG_MESSAGES) { */
      /*       Serial.write("Start game button pressed\n\r"); */
      /*     } */
      /*     break; */
      /* } */
    /* } */
  /* } else { */
    /* // We're tilted, so just wait for outhole */
    /* switchHit = BSOS_PullFirstFromSwitchStack(); */

    /* while (switchHit != SWITCH_STACK_EMPTY) { */
      /* switch (switchHit) { */
      /*   case SW_SELF_TEST_SWITCH: */
      /*     returnState = MACHINE_STATE_TEST_LIGHTS; */
      /*     SetLastSelfTestChangedTime(CurrentTime); */
      /*     break; */
      /*   case SW_COIN_1: */
      /*   case SW_COIN_2: */
      /*   case SW_COIN_3: */
      /*     AddCoinToAudit(switchHit); */
      /*     AddCredit(true, 1); */
      /*     break; */
      /* } */

      /* switchHit = BSOS_PullFirstFromSwitchStack(); */
    }
  }

  return returnState;
}


void loop() {
  BSOS_DataRead(0);

  CurrentTime = millis();
  int newMachineState = MachineState;

  if (MachineState < 0) {
    newMachineState = RunSelfTest(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractMode(MachineState, MachineStateChanged);
  } else {
    newMachineState = RunGamePlayMode(MachineState, MachineStateChanged);
  }

  if (newMachineState != MachineState) {
    MachineState = newMachineState;
    MachineStateChanged = true;
  } else {
    MachineStateChanged = false;
  }

  BSOS_ApplyFlashToLamps(CurrentTime);
  BSOS_UpdateTimedSolenoidStack(CurrentTime);
}
