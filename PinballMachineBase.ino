#include "SharedVariables.h"


/*********************************************************************
    Machine Options Variables
*********************************************************************/
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


/*********************************************************************
    Bonus Variables
*********************************************************************/
unsigned long BonusXAnimationStart;


/*********************************************************************
    Display Variables
*********************************************************************/
unsigned long LastRemainingAnimatedScoreShown;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;


Attract          g_attract;
Debug            g_debug;
DisplayHelper    g_displayHelper;
GameMode         g_gameMode(GAME_MODE_INITIALIZE);
MachineState     g_machineState(MACHINE_STATE_DEBUG);
SelfTestAndAudit g_selfTestAndAudit;


struct PlayfieldAndCabinetSwitch g_solenoidAssociatedSwitches[] = {
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
      g_solenoidAssociatedSwitches
      );

  // Set up the chips and interrupts
  BSOS_InitializeMPU();
  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);

  // Read parameters from EEProm
  ReadStoredParameters();
  BSOS_SetCoinLockout((Credits >= MaximumCredits) ? true : false);
}

void loop() {
  BSOS_DataRead(0);

  unsigned long currentTime = millis();
  g_machineState.setCurrentTime(currentTime);

  byte machineState = g_machineState.machineState();
  byte newMachineState = machineState;
  boolean machineStateChanged = g_machineState.machineStateChanged();

  if (machineState < 0) {
    newMachineState = g_selfTestAndAudit.run(machineState, machineStateChanged);
  } else if (machineState == MACHINE_STATE_DEBUG) {
    newMachineState = g_debug.run(machineState, machineStateChanged);
  } else if (machineState == MACHINE_STATE_ATTRACT) {
    newMachineState = g_attract.run(machineState, machineStateChanged);
  } else {
    newMachineState = g_gameMode.runGamePlayState(machineState, machineStateChanged);
  }

  g_machineState.SetmachineState(newMachineState);

  BSOS_ApplyFlashToLamps(currentTime);
  BSOS_UpdateTimedSolenoidStack(currentTime);
}
