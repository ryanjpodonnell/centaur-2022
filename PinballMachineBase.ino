#include "SharedVariables.h"

#define PINBALL_MACHINE_BASE_MAJOR_VERSION  2022
#define PINBALL_MACHINE_BASE_MINOR_VERSION  1


/*********************************************************************
    Machine Options Variables
*********************************************************************/
boolean FreePlayMode = true;
byte BallSaveNumSeconds = 5;
byte BallsPerGame = 3;
byte Credits = 0;
byte MaximumCredits = 99;
unsigned long HighScore = 0;


/*********************************************************************
    Machine State Variables
*********************************************************************/
boolean MachineStateChanged = true;
boolean SamePlayerShootsAgain;
byte CurrentBallInPlay;
byte CurrentNumPlayers;
byte CurrentPlayer;
char MachineState = 0;
unsigned long CurrentScores[4];
unsigned long CurrentTime = 0;
unsigned long LastTiltWarningTime = 0;
unsigned long ScoreMultiplier = 1;


/*********************************************************************
    Ball State Variables
*********************************************************************/
boolean BallSaveUsed = false;
boolean ExtraBallCollected = false;
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;


/*********************************************************************
    Attract State Variables
*********************************************************************/
unsigned long BonusCountDownEndTime = 0;
unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long LastFlash = 0;


/*********************************************************************
    Bonus Variables
*********************************************************************/
byte BonusX[4];
byte Bonus[4];
byte CurrentBonus;
unsigned long BonusXAnimationStart;


/*********************************************************************
    Display Variables
*********************************************************************/
unsigned long LastFlashOrDash = 0;
unsigned long LastRemainingAnimatedScoreShown;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;


/*********************************************************************
    Game Mode Variables
*********************************************************************/
GameMode GlobalGameMode(GAME_MODE_INITIALIZE);


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

void loop() {
  BSOS_DataRead(0);

  CurrentTime = millis();
  int newMachineState = MachineState;

  if (MachineState < 0) {
    newMachineState = RunSelfTest(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractState(MachineState, MachineStateChanged);
  } else {
    newMachineState = GlobalGameMode.RunGamePlayMode(MachineState, MachineStateChanged);
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
