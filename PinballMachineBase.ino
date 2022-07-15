#include "SharedVariables.h"

Attract          g_attract;
Base             g_base;
BonusLightShow   g_bonusLightShow;
CountdownBonus   g_countdownBonus;
Debug            g_debug;
DestroyCentaur   g_destroyCentaur;
DisplayHelper    g_displayHelper;
GameMode         g_gameMode;
HertzLogging     g_hertzLogging;
LampsHelper      g_lampsHelper;
MachineState     g_machineState;
MatchSequence    g_matchSequence;
OrbMode1         g_orbMode1;
OrbMode2         g_orbMode2;
OrbMode3         g_orbMode3;
OrbMode4         g_orbMode4;
RestartGame      g_restartGame;
SelfTestAndAudit g_selfTestAndAudit;
SkillShot        g_skillShot;
SoundHelper      g_soundHelper;
UnstructuredPlay g_unstructuredPlay;

struct PlayfieldAndCabinetSwitch g_solenoidAssociatedSwitches[] = {
  { SW_RIGHT_SLINGSHOT,      SOL_RIGHT_SLINGSHOT,      SOL_SLINGSHOT_STRENGTH},
  { SW_LEFT_SLINGSHOT,       SOL_LEFT_SLINGSHOT,       SOL_SLINGSHOT_STRENGTH},
  { SW_LEFT_THUMPER_BUMPER,  SOL_LEFT_THUMPER_BUMPER,  SOL_THUMPER_BUMPER_STRENGTH},
  { SW_RIGHT_THUMPER_BUMPER, SOL_RIGHT_THUMPER_BUMPER, SOL_THUMPER_BUMPER_STRENGTH}
};

void setup() {
  if (DEBUG_MESSAGES) Serial.begin(57600);

  BSOS_SetupGameSwitches(
      NUM_SWITCHES_WITH_TRIGGERS,
      NUM_PRIORITY_SWITCHES_WITH_TRIGGERS,
      g_solenoidAssociatedSwitches
      );

  // Set up the chips and interrupts
  BSOS_InitializeMPU();
  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);
  BSOS_SetCoinLockout(false);
}

void loop() {
  BSOS_DataRead(0);

  unsigned long currentTime = millis();
  g_machineState.setCurrentTime(currentTime);

  if (DEBUG_MESSAGES && DEBUG_HERTZ) g_hertzLogging.run();

  int machineState = g_machineState.machineState();
  boolean machineStateChanged = g_machineState.machineStateChanged();

  if (machineState < 0) {
    machineState = g_selfTestAndAudit.run(machineState, machineStateChanged);

  } else if (machineState == MACHINE_STATE_DEBUG) {
    machineState = g_debug.run(machineStateChanged);

  } else if (machineState == MACHINE_STATE_ATTRACT) {
    machineState = g_attract.run(machineStateChanged);

  } else if (machineState == MACHINE_STATE_RESTART_GAME) {
    machineState = g_restartGame.run(machineStateChanged);

  } else if (machineState == MACHINE_STATE_INIT_GAMEPLAY) {
    machineState = g_machineState.initGamePlay();

  } else if (machineState == MACHINE_STATE_INIT_NEW_BALL) {
    machineState = g_machineState.initNewBall(machineStateChanged);

  } else if (machineState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    machineState = g_gameMode.run(machineStateChanged);

  } else if (machineState == MACHINE_STATE_COUNTDOWN_BONUS) {
    machineState = g_countdownBonus.run(machineStateChanged);

  } else if (machineState == MACHINE_STATE_MATCH_MODE) {
    machineState = g_matchSequence.run(machineStateChanged);

  } else if (machineState == MACHINE_STATE_BALL_OVER && g_machineState.samePlayerShootsAgain()) {
    machineState = g_machineState.initNewBall(machineStateChanged);

  } else if (machineState == MACHINE_STATE_BALL_OVER && !g_machineState.samePlayerShootsAgain()) {
    if (DEBUG_MESSAGES) Serial.write("Increasing Current Player\n\r");
    machineState = g_machineState.increaseCurrentPlayer();
  }

  g_machineState.setMachineState(machineState);

  BSOS_ApplyFlashToLamps(currentTime);
  BSOS_UpdateTimedSolenoidStack(currentTime);
}
