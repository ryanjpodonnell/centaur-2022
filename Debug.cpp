#include "SharedVariables.h"

Debug::Debug() {
  stateStatedTime_ = 0;
}

byte Debug::run(byte curState, boolean curStateChanged) {
  if (curStateChanged) handleNewState();

  unsigned long timeSinceStateStarted = g_machineState.currentTime() - stateStatedTime_;
  if (timeSinceStateStarted >= 5000) return MACHINE_STATE_ATTRACT;

  return curState;
}

void Debug::handleNewState() {
  stateStatedTime_ = g_machineState.currentTime();

  if (DEBUG_MESSAGES) {
    Serial.write("Entering Debug State\n\r");
  }

  ShowAllLamps();

  g_machineState.setScore(PINBALL_MACHINE_BASE_MAJOR_VERSION, 0);
  g_machineState.setScore(PINBALL_MACHINE_BASE_MINOR_VERSION, 1);
  g_machineState.setScore(BALLY_STERN_OS_MAJOR_VERSION, 2);
  g_machineState.setScore(BALLY_STERN_OS_MINOR_VERSION, 3);
  g_displayHelper.showPlayerScores(0xFF, false, false);
}
