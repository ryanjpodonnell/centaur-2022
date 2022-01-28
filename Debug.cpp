#include "SharedVariables.h"

Debug::Debug() {
  stateStatedTime_ = 0;
}

byte Debug::run(byte curState, boolean curStateChanged) {
  unsigned long currentTime = g_machineState.currentTime();
  if (stateStatedTime_ == 0) stateStatedTime_ = currentTime;
  byte returnState = curState;

  if (curStateChanged) {
    ShowAllLamps();
    g_machineState.setScore(PINBALL_MACHINE_BASE_MAJOR_VERSION, 0);
    g_machineState.setScore(PINBALL_MACHINE_BASE_MINOR_VERSION, 1);
    g_machineState.setScore(BALLY_STERN_OS_MAJOR_VERSION, 2);
    g_machineState.setScore(BALLY_STERN_OS_MINOR_VERSION, 3);
    g_displayHelper.showPlayerScores(0xFF, false, false);
  }

  unsigned long timeSinceStateStarted = currentTime - stateStatedTime_;
  if (timeSinceStateStarted >= 5000) returnState = MACHINE_STATE_ATTRACT;

  return returnState;
}
