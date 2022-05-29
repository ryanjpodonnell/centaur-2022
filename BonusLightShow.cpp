#include "SharedVariables.h"

BonusLightShow::BonusLightShow() {
  running_ = false;

  endTime_   = 0;
  lastFlash_ = 0;
}

boolean BonusLightShow::ended() {
  return endTime_ && g_machineState.currentTime() > endTime_;
}

boolean BonusLightShow::running() {
  return running_;
}

void BonusLightShow::end() {
  endTime_ = 0;
  running_ = false;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
}

void BonusLightShow::run() {
  unsigned long seed = g_machineState.currentTime() / 100;   // .10 seconds
  if (seed == lastFlash_) return;

  switch (lightShowId_) {
    case BONUS_LIGHT_SHOW_SPIN:
      manageSpinShow(seed);
      break;

    case BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW:
      manageResetArrowShow(seed);
      break;
  }
}

void BonusLightShow::start(byte lightShowId) {
  running_     = true;
  lightShowId_ = lightShowId;
  endTime_     = g_machineState.currentTime() + showDurations_[lightShowId_];

  if (lightShowId_ == BONUS_LIGHT_SHOW_SPIN) g_soundHelper.stopAudio();
  g_soundHelper.playSound(SOUND_BONUS);
}

/*********************************************************************
    Private
*********************************************************************/
void BonusLightShow::manageResetArrowShow(unsigned long seed) {
  lastFlash_          = seed;
  byte numberOfSteps  = 6;
  byte currentStep    = seed % numberOfSteps;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 0) {
    return;
  } else if (currentStep < 5) {
    g_lampsHelper.showLampsWithSeed(LAMP_COLLECTION_BONUS_RESET_ARROW, currentStep);
  } else {
    g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_RESET_ARROW);
  }
}

void BonusLightShow::manageSpinShow(unsigned long seed) {
  lastFlash_          = seed;
  byte numberOfSteps  = 5;
  byte currentStep    = seed % numberOfSteps;

  g_lampsHelper.hideAllLamps();
  if (currentStep == 0) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_1);
  if (currentStep == 1) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_2);
  if (currentStep == 2) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_3);
  if (currentStep == 3) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_4);
  if (currentStep == 4) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_5);
}
