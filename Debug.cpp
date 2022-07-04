#include "SharedVariables.h"

Debug::Debug() {
  stateStartedTime_ = 0;
}

int Debug::run(boolean curStateChanged) {
  if (curStateChanged) manageNewState();

  if ((g_machineState.currentTime() - stateStartedTime_) >= 3000) {
    g_machineState.readStoredParameters();
    return MACHINE_STATE_ATTRACT;
  }

  return MACHINE_STATE_DEBUG;
}

void Debug::manageNewState() {
  stateStartedTime_ = g_machineState.currentTime();

  if (DEBUG_MESSAGES) {
    Serial.write("Entering Debug State\n\r");
  }

  g_soundHelper.stopAudio();
  g_lampsHelper.showAllLamps();
  g_machineState.setScore(PINBALL_MACHINE_BASE_MAJOR_VERSION, 0);
  g_machineState.setScore(PINBALL_MACHINE_BASE_MINOR_VERSION, 1);
  g_machineState.setScore(BALLY_STERN_OS_MAJOR_VERSION, 2);
  g_machineState.setScore(BALLY_STERN_OS_MINOR_VERSION, 3);
  g_displayHelper.showPlayerScores(0xFF);
}
