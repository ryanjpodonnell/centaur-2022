#include "SharedVariables.h"

CountdownBonus::CountdownBonus() {
  countdownEndTime_ = 0;
  lastDecrease_     = 0;
  stateStartedTime_ = 0;
}

int CountdownBonus::run(boolean curStateChanged) {
  if (curStateChanged) handleNewState();

  unsigned long timeSinceStateStarted = g_machineState.currentTime() - stateStartedTime_;
  if (timeSinceStateStarted > 1000) countdownBonusStep();

  g_displayHelper.showPlayerScores(g_machineState.currentPlayerNumber());
  g_lampsHelper.showBonusLamps(g_machineState.currentPlayerNumber());

  if (countdownEndTime_ && g_machineState.currentTime() > countdownEndTime_) return MACHINE_STATE_BALL_OVER;
  return MACHINE_STATE_COUNTDOWN_BONUS;
}

void CountdownBonus::addBonusToScore() {
  unsigned long bonusMultiplier = (unsigned long)g_machineState.bonusMultiplier();
  unsigned long bonusValue = bonusMultiplier * 1000;
  g_machineState.increaseScore(bonusValue);
}

void CountdownBonus::countdownBonusStep() {
  unsigned long seed = g_machineState.currentTime() / 100;   // .10 seconds
  if (seed != lastDecrease_) {
    lastDecrease_ = seed;

    if (g_machineState.bonus()) {
      if (!g_machineState.currentPlayerTilted()) addBonusToScore();
      g_machineState.decreaseBonus(1);
    }
  }

  if (!g_machineState.bonus() && !countdownEndTime_) {
    countdownEndTime_ = g_machineState.currentTime() + 1000;
  }
}

void CountdownBonus::handleNewState() {
  if (DEBUG_MESSAGES) Serial.write("Entering Countdown Bonus State\n\r");

  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);
  g_lampsHelper.hideAllLamps();

  countdownEndTime_ = 0;
  lastDecrease_     = 0;
  stateStartedTime_ = g_machineState.currentTime();
}
