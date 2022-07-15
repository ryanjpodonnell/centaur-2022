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

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
      case(SW_CREDIT_BUTTON):
        return g_machineState.manageCreditButton(MACHINE_STATE_COUNTDOWN_BONUS);
        break;
      case SW_COIN_1:
      case SW_COIN_2:
      case SW_COIN_3:
        g_machineState.manageCoinDrop(switchHit);
        break;
      case SW_SELF_TEST_SWITCH:
        return MACHINE_STATE_TEST_LIGHTS;
        break;
    }
  }

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

  if (g_bonusLightShow.running()) {
    g_bonusLightShow.run();
  } else {
    g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  }

  if (g_bonusLightShow.ended()) return MACHINE_STATE_BALL_OVER;

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

      g_soundHelper.playSoundWithoutInterruptions(SOUND_BONUS);
      g_machineState.decreaseBonus();
      g_machineState.updateBonusLamps();
      g_machineState.updatePlayerScore();
    }
  }
}

void CountdownBonus::manageNewState() {
  if (DEBUG_MESSAGES) Serial.write("Entering Countdown Bonus State\n\r");

  g_bonusLightShow.reset();
  g_lampsHelper.hideAllLamps();
  g_lampsHelper.showLamp(LAMP_BALL_IN_PLAY);
  g_soundHelper.stopAudio();

  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);
  BSOS_SetDisplayCredits(g_machineState.credits());

  lastDecrease_            = 0;
  lastFlash_               = 0;
  stateStartedTime_        = g_machineState.currentTime();
  stepDurationChangedTime_ = g_machineState.currentTime();
  stepDuration_            = 100;
}
