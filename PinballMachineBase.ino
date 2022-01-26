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
unsigned long BonusXAnimationStart;


/*********************************************************************
    Display Variables
*********************************************************************/
unsigned long LastFlashOrDash = 0;
unsigned long LastRemainingAnimatedScoreShown;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;


/*********************************************************************
    Machine State Variables
*********************************************************************/
MachineState GlobalMachineState(MACHINE_STATE_ATTRACT);


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

  GlobalMachineState.SetScore(PINBALL_MACHINE_BASE_MAJOR_VERSION, 0);
  GlobalMachineState.SetScore(PINBALL_MACHINE_BASE_MINOR_VERSION, 1);
  GlobalMachineState.SetScore(BALLY_STERN_OS_MAJOR_VERSION, 2);
  GlobalMachineState.SetScore(BALLY_STERN_OS_MINOR_VERSION, 3);
}

void loop() {
  BSOS_DataRead(0);

  unsigned long currentTime = millis();
  GlobalMachineState.SetCurrentTime(currentTime);

  byte machineState = GlobalMachineState.GetMachineState();
  byte newMachineState = machineState;
  boolean machineStateChanged = GlobalMachineState.GetMachineStateChanged();

  if (machineState < 0) {
    newMachineState = RunSelfTest(machineState, machineStateChanged);
  } else if (machineState == MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractState(machineState, machineStateChanged);
  } else {
    newMachineState = GlobalGameMode.RunGamePlayState(machineState, machineStateChanged);
  }

  GlobalMachineState.SetMachineState(newMachineState);

  BSOS_ApplyFlashToLamps(currentTime);
  BSOS_UpdateTimedSolenoidStack(currentTime);
}
