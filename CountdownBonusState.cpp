#include "SharedVariables.h"

CountdownBonus::CountdownBonus() {
  lastDecrease_     = 0;
  lastFlash_        = 0;
  stateStartedTime_ = 0;
  stateStartedTime_ = 0;
  stepDuration_     = 100; // .10 seconds
}

int CountdownBonus::run(boolean curStateChanged) {
  if (curStateChanged) manageNewState();

  unsigned long timeSinceStateStarted         = g_machineState.currentTime() - stateStartedTime_;
  unsigned long timeSinceStepDuractionChanged = g_machineState.currentTime() - stepDurationChangedTime_;

  if (timeSinceStepDuractionChanged > 3000) {
    stepDuration_ = stepDuration_ / 2;
    stepDurationChangedTime_ = g_machineState.currentTime();
  }

  if (timeSinceStateStarted < 1000) return MACHINE_STATE_COUNTDOWN_BONUS;

  if (g_machineState.bonus()) {
    countdownBonusStep();
    return MACHINE_STATE_COUNTDOWN_BONUS;
  }

  if (g_bonusLightShow.ended()) {
    g_bonusLightShow.end();
    return MACHINE_STATE_BALL_OVER;
  }

  if (g_bonusLightShow.running()) {
    g_bonusLightShow.run();
  } else {
    g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}

/*********************************************************************
    Private
*********************************************************************/
void CountdownBonus::addBonusToScore() {
  unsigned long bonusMultiplier = (unsigned long)g_machineState.bonusMultiplier();
  unsigned long bonusValue = bonusMultiplier * 1000;

  g_machineState.setScore(g_machineState.score() + bonusValue);
}

void CountdownBonus::countdownBonusStep() {
  unsigned long seed = g_machineState.currentTime() / stepDuration_;

  if (seed != lastDecrease_) {
    lastDecrease_ = seed;

    if (g_machineState.bonus()) {
      if (!g_machineState.currentPlayerTilted()) addBonusToScore();

      g_soundHelper.playSound(SOUND_BONUS);
      g_machineState.decreaseBonus(1);
      g_machineState.updateBonusLamps();
      g_machineState.updatePlayerScore();
    }
  }
}

void CountdownBonus::manageNewState() {
  if (DEBUG_MESSAGES) Serial.write("Entering Countdown Bonus State\n\r");

  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_lampsHelper.hideAllLamps();

  lastDecrease_            = 0;
  lastFlash_               = 0;
  stateStartedTime_        = g_machineState.currentTime();
  stepDurationChangedTime_ = g_machineState.currentTime();
  stepDuration_            = 100;
}
