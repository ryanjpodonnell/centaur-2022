#include "SharedVariables.h"

Debug::Debug() {
  stateStartedTime_ = 0;
}

int Debug::run(int curState, boolean curStateChanged) {
  if (curStateChanged) handleNewState();

  unsigned long timeSinceStateStarted = g_machineState.currentTime() - stateStartedTime_;
  if (timeSinceStateStarted >= 5000) return MACHINE_STATE_ATTRACT;

  return curState;
}

void Debug::handleNewState() {
  stateStartedTime_ = g_machineState.currentTime();

  if (DEBUG_MESSAGES) {
    Serial.write("Entering Debug State\n\r");
  }

  g_lampsHelper.showAllLamps();
  g_machineState.setScore(PINBALL_MACHINE_BASE_MAJOR_VERSION, 0);
  g_machineState.setScore(PINBALL_MACHINE_BASE_MINOR_VERSION, 1);
  g_machineState.setScore(BALLY_STERN_OS_MAJOR_VERSION, 2);
  g_machineState.setScore(BALLY_STERN_OS_MINOR_VERSION, 3);
  g_displayHelper.showPlayerScores(0xFF, false, false);
}
