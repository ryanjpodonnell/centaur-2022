#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "PinballMachineBase.h"
#include "SelfTestAndAudit.h"

//============================== OPERATOR GAME ADJUSTMENTS ==============================
boolean FreePlay = true;            // false = coin drop, true = free play

#define MAX_CREDITS          25     // (25) max credits
#define BALLS_PER_GAME       3      // (3) balls per game
#define ATTRACT_SPEECH       1      // 0 = no attract speech, 1 = attract speect "Emperor Ming awaits!"
#define ATTRACT_SPEECH_TIMER 300000 // (300000) Ammount of time between attract mode speech callouts. 60000 = 1 minute
#define USE_STROBE           0      // Strobe use is currently not recommended. Please message me if you would like to help! 0 = don't use backglass strobe, 1 = use backglass strobe
#define DEBUG_MESSAGES       1      // 0 = no serial monitor, 1 = serial monitor for testing
//=====================================================================================

#define VERSION_NUMBER 2022

int MachineState            = -1;
boolean MachineStateChanged = true;

#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_SKILL_SHOT      3
#define MACHINE_STATE_NORMAL_GAMEPLAY 4

unsigned long CurrentTime       = 0;
unsigned long InitGameStartTime = 0;

byte Credits             = 0;
byte ScoreAwardStates[4] = {0, 0, 0, 0};

unsigned long HighScore = 0;

byte WholeCredit = 0;

byte CurrentPlayer     = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;

unsigned long CurrentScores[4];

boolean BallDrained    = false;

byte NumTiltWarnings = 0;
#define MAX_TILT_WARNINGS 1

byte SkillShotHits = 0;
byte SkillShotState = 0; // 0 not started, 1 started, 2 collecting, 3 collected/fail

#define TIME_TO_WAIT_FOR_BALL 1000
unsigned long BallTimeInTrough       = 0;
unsigned long BallFirstSwitchHitTime = 0;
boolean PFValidated                  = false;
boolean BallSaveUsed                 = false;
unsigned long BallSaveNumSeconds     = 0;

unsigned long InlaneLeftBlip = 0;
unsigned long InlaneRightBlip = 0;
unsigned long TargetUpperBlip = 0;
unsigned long TargetLowerTopBlip = 0;
unsigned long TargetLowerBottomBlip = 0;

byte DTarget3Lights[3] = {0, 0, 0}; // A, B, C // 0 off, 1 flashing, 2 hurry up, 3 collected
byte DTarget3Flashing  = 0;

byte DTarget4Light[4] = {0, 0, 0, 0}; // A, B, C, D // 0 off, 1 flashing, 2 hurry up, 3 collected
#define DTARGET_4_HURRY_UP_TIMER 12500;
unsigned long WhiteHurryUpStart = 0;
unsigned long AmberHurryUpStart = 0;

byte DropTargetCount      = 0;
boolean DTargetInlineGoal = false;
boolean DTarget3Goal      = false;
boolean DTarget4Goal      = false;
byte DTarget3Completions  = 0;

boolean LeftSpinnerHurryUpLit          = false;
unsigned long LeftSpinnerHurryUpTimer  = 0;
boolean RightSpinnerHurryUpLit         = false;
unsigned long RightSpinnerHurryUpTimer = 0;
boolean LeftSpinnerLit                 = false;
boolean RightSpinnerLit                = false;
boolean TopPopLit                      = false;
boolean TopSpecialLit                  = false;

boolean PlayerShootsAgain   = false;
byte WoodBeastXBallState[4] = {0, 0, 0, 0}; // 0 off, 1 lit, 2 collected, 3 used
byte SaucerXBallState[4]    = {0, 0, 0, 0}; // ^

byte SaucerLamps = 1;
unsigned long SaucerHitTime = 0;
byte SaucerDirection = 0;
boolean BallInSaucer = false;

byte BonusXState = 1;
#define PLAYFIELD_X_TIMER 15000;
byte Playfield2xState = 0; // 0 off, 1 lit, 2 started
byte Playfield3xState = 0; // ^
unsigned long Playfield2XStart = 0;
unsigned long Playfield3XStart = 0;

unsigned long CurrentSoundEffectTimer = 0;


// #################### SETUP ####################
void setup() {
  if (DEBUG_MESSAGES) {
    Serial.begin(115200);
  }

  // read analog pin for match sequence
  randomSeed(analogRead(A6));

  // Tell the OS about game-specific lights and switches
  BSOS_SetupGameSwitches(NUM_SWITCHES_WITH_TRIGGERS, NUM_PRIORITY_SWITCHES_WITH_TRIGGERS, SolenoidAssociatedSwitches);

  if (DEBUG_MESSAGES) {
    Serial.write("Attempting to initialize the MPU\n");
  }

  // Set up the chips and interrupts
  BSOS_InitializeMPU();
  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);

  byte dipBank = BSOS_GetDipSwitches(0);

  // Use dip switches to set up game variables
  if (DEBUG_MESSAGES) {
    char buf[32];
    sprintf(buf, "DipBank 0 = 0x%02X\n", dipBank);
    Serial.write(buf);
  }

  BSOS_SetDisplay(0, VERSION_NUMBER, true, 2);
  BSOS_SetDisplayBlank(1, 0x00);
  BSOS_SetDisplayBlank(2, 0x00);
  BSOS_SetDisplayBlank(3, 0x00);

  delay(4000);

  Credits                          = BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE);
  if (Credits>MAX_CREDITS) Credits = MAX_CREDITS;
  HighScore                        = BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, 0);

  if (DEBUG_MESSAGES) {
    Serial.write("Done with setup\n");
  }
}


// #################### PLAY SOUND ####################
void PlaySound(byte soundByte) {
  // don't interupt currently playing sound
  if (CurrentTime<CurrentSoundEffectTimer) return;

  // if no timed sound is being played, play triggered sound
  if (SoundTimings[soundByte]) CurrentSoundEffectTimer = CurrentTime + ((unsigned long)SoundTimings[soundByte] * 250); // 1000 = 1 sec

  BSOS_PlaySoundSquawkAndTalk(soundByte);
}


// #################### ADD CREDIT ####################
void AddCredit(byte numToAdd=1) {
  if (Credits<MAX_CREDITS) {
    WholeCredit = WholeCredit + numToAdd;
    if (WholeCredit==1) {
      PlaySound(32);
    } else if (WholeCredit==2) {
      PlaySound(33);
    } else if (WholeCredit==3) {
      PlaySound(34);
    } else if (WholeCredit>=4) {
      PlaySound(35);
      Credits = Credits + 1;
      BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
      WholeCredit = 0;
    }
  }

  BSOS_SetCoinLockout((Credits<MAX_CREDITS)?false:true);
}


// #################### ADD PLAYER ####################
boolean AddPlayer(boolean resetNumPlayers = false) {
  if (Credits<1 && !FreePlay) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers>=4) return false;

  CurrentNumPlayers += 1;
  BSOS_SetDisplay(CurrentNumPlayers-1, 0);
  BSOS_SetDisplayBlank(CurrentNumPlayers-1, 0x60);
  if (CurrentNumPlayers >= 2) {
    PlaySound(18); // saucer 0
  }

  if (!FreePlay) {
    Credits -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
  }

  return true;
}


// #################### INIT NEW BALL ####################
int InitNewBall(bool curStateChanged, int ballNum) {
  return MACHINE_STATE_INIT_NEW_BALL;
}


// #################### RUN SELF TEST ####################
int RunSelfTest(int curState, boolean curStateChanged) {
  int returnState = curState;
  CurrentNumPlayers = 0;
  // Any state that's greater than CHUTE_3 is handled by the Base Self-test code
  // Any that's less, is machine specific, so we handle it here.
  if (curState>=MACHINE_STATE_TEST_SCORE_LEVEL_3) {
    returnState = RunBaseSelfTest(returnState, curStateChanged, CurrentTime, SW_CREDIT_BUTTON, SW_LEFT_FLIPPER_BUTTON);
  } else {
    returnState = MACHINE_STATE_ATTRACT;
  }
  return returnState;
}


// #################### TURN OFF ATTRACT LAMPS ####################
void TurnOffAttractLamps() {
  for (int count=0; count<BSOS_MAX_LAMPS; count++) {
    if (count==40) {
      count = 42;
    } else if (count==43) {
      count = 44;
    } else if (count==48) {
      count = 52;
    }
    BSOS_SetLampState(count, 0, 0, 0);
  }
}


// #################### RUN ATTRACT MODE ####################
int RunAttractMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  return returnState;
}


// #################### NORMAL GAMEPLAY ####################
int NormalGamePlay(boolean curStateChanged) {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  return returnState;
}


// #################### INIT GAMEPLAY ####################
int InitGamePlay(boolean curStateChanged) {
  int returnState = MACHINE_STATE_INIT_GAMEPLAY;
  return returnState;
}


// #################### RUN GAMEPLAY MODE ####################
int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  return returnState;
}


// #################### LOOP ####################
void loop() {
  // This line has to be in the main loop
  BSOS_DataRead(0);

  CurrentTime = millis();
  int newMachineState = MachineState;

  // Machine state is self-test/attract/game play
  if (MachineState<0) {
    newMachineState = RunSelfTest(MachineState, MachineStateChanged);
  } else if (MachineState==MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractMode(MachineState, MachineStateChanged);
  } else {
    newMachineState = RunGamePlayMode(MachineState, MachineStateChanged);
  }

  if (newMachineState!=MachineState) {
    MachineState = newMachineState;
    MachineStateChanged = true;
  } else {
    MachineStateChanged = false;
  }

  BSOS_ApplyFlashToLamps(CurrentTime);
  BSOS_UpdateTimedSolenoidStack(CurrentTime);
}


// #################### LAMP ANIMATIONS ####################
// ########## BACKGLASS LAMPS CLEAR ##########
/* void BackglassLampsClear() { */
/*   BSOS_SetLampState(LAMP_FLASH_GORDON_6, 0); */
/*   BSOS_SetLampState(LAMP_FLASH_GORDON_1, 0); */
/*   BSOS_SetLampState(LAMP_FLASH_GORDON_5, 0); */
/*   BSOS_SetLampState(LAMP_FLASH_GORDON_2, 0); */
/*   BSOS_SetLampState(LAMP_FLASH_GORDON_4, 0); */
/*   BSOS_SetLampState(LAMP_FLASH_GORDON_3, 0); */
/*   BSOS_SetLampState(LAMP_FLASH_STROBE, 0); */
/* } */


// ########## BACKGLASS LAMPS CENTER OUT ##########
/* void backglassLampsCenterOut() { */
/*   if (USE_STROBE) { */
/*     byte lampPhase = (CurrentTime/115)%4; */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_6, lampPhase==3||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_1, lampPhase==3||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_5, lampPhase==2||lampPhase==3, lampPhase==3); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_2, lampPhase==2||lampPhase==3, lampPhase==3); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_4, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_3, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     if (MachineState!=MACHINE_STATE_NORMAL_GAMEPLAY || (MachineState==MACHINE_STATE_NORMAL_GAMEPLAY && BallInSaucer==true)) BSOS_SetLampState(LAMP_FLASH_STROBE, lampPhase<2, lampPhase==1); */
/*   } else { */
/*     byte lampPhase = (CurrentTime/115)%3; */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_6, lampPhase==2||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_1, lampPhase==2||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_5, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_2, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_4, lampPhase<2, lampPhase==1); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_3, lampPhase<2, lampPhase==1); */
/*   } */
/* } */


// ########## BACKGLASS LAMPS KNIGHT RIDER ##########
/* void BackglassLampsKnightRider() { */
/*   if (USE_STROBE) { */
/*     byte lampPhase = (CurrentTime/80)%14; */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_6, lampPhase==6||lampPhase==7, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_5, lampPhase==5||lampPhase==8, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_4, lampPhase==4||lampPhase==9, 0, 0); */
/*     if (MachineState!=MACHINE_STATE_NORMAL_GAMEPLAY) BSOS_SetLampState(LAMP_FLASH_STROBE, lampPhase==3||lampPhase==10, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_3, lampPhase==2||lampPhase==11, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_2, lampPhase==1||lampPhase==12, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_1, lampPhase==0||lampPhase==13, 0, 0); */
/*   } else { */
/*     byte lampPhase = (CurrentTime/80)%12; */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_6, lampPhase==5||lampPhase==6, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_5, lampPhase==4||lampPhase==7, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_4, lampPhase==3||lampPhase==8, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_3, lampPhase==2||lampPhase==9, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_2, lampPhase==1||lampPhase==10, 0, 0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_1, lampPhase==0||lampPhase==11, 0, 0); */
/*   } */
/* } */


// ########## BACKGLASS LAMPS LEFT TO RIGHT ##########
/* void BackglassLampsLeft2Right() { */
/*   if (USE_STROBE) { */
/*     byte lampPhase = (CurrentTime/85)%4; */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_6, lampPhase==3||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_3, lampPhase==3||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_5, lampPhase==2||lampPhase==3, lampPhase==3); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_2, lampPhase==2||lampPhase==3, lampPhase==3); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_4, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_1, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     if (MachineState!=MACHINE_STATE_NORMAL_GAMEPLAY) BSOS_SetLampState(LAMP_FLASH_STROBE, lampPhase<2, lampPhase==1); */
/*   } else { */
/*     byte lampPhase = (CurrentTime/95)%3; */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_6, lampPhase==2||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_3, lampPhase==2||lampPhase==0, lampPhase==0); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_5, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_2, lampPhase==1||lampPhase==2, lampPhase==2); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_4, lampPhase<2, lampPhase==1); */
/*     BSOS_SetLampState(LAMP_FLASH_GORDON_1, lampPhase<2, lampPhase==1); */
/*   } */
/* } */


// ########## ATTRACT RETRO ##########
/* void AttractRetro() { */
/*   // bonus */
/*   byte attractBonus; */
/*   for (attractBonus=0; attractBonus<10; attractBonus++){ */
/*     if ( attractBonus % 2 == 0) { // even but odd on pf */
/*       BSOS_SetLampState(LAMP_BONUS_MINI_1K+attractBonus, 1, 0, 100); */
/*       BSOS_SetLampState(LAMP_BONUS_SUPER_1K+attractBonus, 1, 0, 100); */
/*     } else { // odd even on pf */
/*       BSOS_SetLampState(LAMP_BONUS_MINI_1K+attractBonus, 1, 0, 200); */
/*       BSOS_SetLampState(LAMP_BONUS_SUPER_1K+attractBonus, 1, 0, 200); */
/*     } */
/*   } */

/*   // sweep 4 */
/*   byte lampPhase1 = (CurrentTime/115)%4; // 125 */
/*   BSOS_SetLampState(LAMP_SAUCER_XBALL, lampPhase1==3||lampPhase1==0, lampPhase1==0); // SAUCER */
/*   BSOS_SetLampState(LAMP_SAUCER_30K, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_SAUCER_20K, lampPhase1==1||lampPhase1==2, lampPhase1==2); */
/*   BSOS_SetLampState(LAMP_SAUCER_10K, lampPhase1<2, lampPhase1==1); */
/*   BSOS_SetLampState(LAMP_DTARGET_4_A, lampPhase1==3||lampPhase1==0, lampPhase1==0); // DTARGET 4 */
/*   BSOS_SetLampState(LAMP_DTARGET_4_B, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_DTARGET_4_C, lampPhase1==1||lampPhase1==2, lampPhase1==2); */
/*   BSOS_SetLampState(LAMP_DTARGET_4_D, lampPhase1<2, lampPhase1==1); */
/*   BSOS_SetLampState(LAMP_BONUS_5X, lampPhase1==3||lampPhase1==0, lampPhase1==0); // BONUS X */
/*   BSOS_SetLampState(LAMP_BONUS_4X, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_BONUS_3X, lampPhase1==1||lampPhase1==2, lampPhase1==2); */
/*   BSOS_SetLampState(LAMP_BONUS_2X, lampPhase1<2, lampPhase1==1); */
/*   BSOS_SetLampState(LAMP_OUTLANE_RIGHT_SPECIAL, lampPhase1==3||lampPhase1==0, lampPhase1==0); // OUTLANES & PFX */
/*   BSOS_SetLampState(LAMP_SAUCER_ARROW_3X, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_CLOCK_15_SECONDS_2X, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_OUTLANE_LEFT_SPECIAL, lampPhase1==1||lampPhase1==2, lampPhase1==2); */
/*   BSOS_SetLampState(LAMP_SAUCER_ARROW_2X, lampPhase1<2, lampPhase1==1); */
/*   BSOS_SetLampState(LAMP_CLOCK_15_SECONDS_3X, lampPhase1<2, lampPhase1==1); */
/*   BSOS_SetLampState(LAMP_INLANE_LEFT, lampPhase1==3||lampPhase1==0, lampPhase1==0); // INLANES & SPINNERS */
/*   BSOS_SetLampState(LAMP_SPINNER_RIGHT, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_INLANE_RIGHT, lampPhase1==1||lampPhase1==2, lampPhase1==2); */
/*   BSOS_SetLampState(LAMP_SPINNER_LEFT, lampPhase1<2, lampPhase1==1); */
/*   BSOS_SetLampState(LAMP_TARGET_LRIGHT_BOTTOM, lampPhase1==3||lampPhase1==0, lampPhase1==0); // REMAINDER */
/*   BSOS_SetLampState(LAMP_TARGET_WOOD_BEAST_XBALL, lampPhase1==2||lampPhase1==3, lampPhase1==3); */
/*   BSOS_SetLampState(LAMP_TARGET_LRIGHT_TOP, lampPhase1==1||lampPhase1==2, lampPhase1==2); */
/*   BSOS_SetLampState(LAMP_DTARGET_BONUS_5X, lampPhase1<2, lampPhase1==1); */

/*   // sweep 5 */
/*   byte lampPhase2 = (CurrentTime/95)%5; */
/*   BSOS_SetLampState(LAMP_STAR_SHOOTER_BOTTOM, lampPhase2==4||lampPhase2==0, lampPhase2==0); */
/*   BSOS_SetLampState(LAMP_STAR_SHOOTER_MIDDLE, lampPhase2==3||lampPhase2==4, lampPhase2==4); */
/*   BSOS_SetLampState(LAMP_STAR_SHOOTER_TOP, lampPhase2==2||lampPhase2==3, lampPhase2==3); */
/*   BSOS_SetLampState(LAMP_STAR_PFIELD_TOP, lampPhase2==1||lampPhase2==2, lampPhase2==2); */
/*   BSOS_SetLampState(LAMP_STAR_PFIELD_BOTTOM, lampPhase2<2, lampPhase2==1); */

/*   // sweep 4 */
/*   byte lampPhase3 = (CurrentTime/75)%6; */
/*   BSOS_SetLampState(LAMP_DTARGET_ARROW_3, lampPhase3==5||lampPhase3==0, lampPhase3==0); */
/*   BSOS_SetLampState(LAMP_DTARGET_ARROW_2, lampPhase3==4||lampPhase3==5, lampPhase3==5); */
/*   BSOS_SetLampState(LAMP_DTARGET_ARROW_1, lampPhase3==3, lampPhase3==4, lampPhase3==4); */
/*   BSOS_SetLampState(LAMP_TARGET_UPPER_SPECIAL, lampPhase3==2||lampPhase3==3, lampPhase3==3); */
/*   BSOS_SetLampState(LAMP_TARGET_UPPER_COLLECT_BONUS, lampPhase3==1||lampPhase3==2, lampPhase3==2); */
/*   BSOS_SetLampState(LAMP_DTARGET_BONUS_4X, lampPhase3<2, lampPhase3==1); */

/*   // remainder */
/*   BSOS_DataRead(0); */
/*   BSOS_SetLampState(LAMP_POP_TOP, 1, 0, 250); */
/*   BSOS_SetLampState(LAMP_BONUS_MINI_50K, 1, 0, 250); */
/*   BSOS_SetLampState(LAMP_BONUS_SUPER_100K, 1, 0, 250); */
/*   BSOS_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 250); */
/*   BSOS_SetLampState(LAMP_MING_TOP, 1, 0, 250); */
/*   BSOS_SetLampState(LAMP_MING_BOTTOM, 1); */

/*   BackglassLampsLeft2Right(); */
/* } */
