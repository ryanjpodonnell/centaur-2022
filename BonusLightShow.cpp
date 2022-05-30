#include "SharedVariables.h"

BonusLightShow::BonusLightShow() {
  ended_   = false;
  running_ = false;

  endTime_   = 0;
  lastFlash_ = 0;
}

boolean BonusLightShow::ended() {
  return ended_;
}

boolean BonusLightShow::running() {
  return running_;
}

void BonusLightShow::end() {
  endTime_ = 0;
  ended_   = true;
  running_ = false;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS);
  g_machineState.updateOrbsDropTargetLamps();
  g_machineState.updateRightDropTargetLamps();
}

void BonusLightShow::reset() {
  ended_   = false;
  running_ = false;

  endTime_   = 0;
  lastFlash_ = 0;
}

void BonusLightShow::run() {
  if (endTime_ && g_machineState.currentTime() > endTime_) end();

  unsigned long seed = g_machineState.currentTime() / 100;   // .10 seconds
  if (seed == lastFlash_) return;

  switch (lightShowId_) {
    case BONUS_LIGHT_SHOW_ORBS_AND_RIGHT_DROPS_ARROW:
      manageOrbsAndRightDropsArrowShow(seed);
      break;

    case BONUS_LIGHT_SHOW_ORBS_DROPS_ARROW:
      manageOrbsDropsArrowShow(seed);
      break;

    case BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW:
      manageResetArrowShow(seed);
      break;

    case BONUS_LIGHT_SHOW_RIGHT_DROPS_ARROW:
      manageRightDropsArrowShow(seed);
      break;

    case BONUS_LIGHT_SHOW_SPIN:
      manageSpinShow(seed);
      break;
  }
}

void BonusLightShow::start(byte lightShowId) {
  ended_       = false;
  running_     = true;
  lightShowId_ = lightShowId;
  endTime_     = g_machineState.currentTime() + showDurations_[lightShowId_];

  if (lightShowId_ == BONUS_LIGHT_SHOW_SPIN) g_soundHelper.stopAudio();
  g_soundHelper.playSound(SOUND_BONUS);
}

/*********************************************************************
    Private
*********************************************************************/
void BonusLightShow::manageOrbsAndRightDropsArrowShow(unsigned long seed) {
  lastFlash_          = seed;
  byte numberOfSteps  = 11;
  byte currentStep    = seed % numberOfSteps;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 0 || currentStep == 5) {
    return;
  } else if (currentStep < 4) {
    g_lampsHelper.showLampsWithSeed(LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW, currentStep);
  } else if (currentStep == 4) {
    g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW);
  } else if (currentStep < 10) {
    g_lampsHelper.showLampsWithSeed(LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW, currentStep - 5);
  } else {
    g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW);
  }
}

void BonusLightShow::manageOrbsDropsArrowShow(unsigned long seed) {
  lastFlash_          = seed;
  byte numberOfSteps  = 5;
  byte currentStep    = seed % numberOfSteps;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 0) {
    return;
  } else if (currentStep < 4) {
    g_lampsHelper.showLampsWithSeed(LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW, currentStep);
  } else {
    g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW);
  }
}

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

void BonusLightShow::manageRightDropsArrowShow(unsigned long seed) {
  lastFlash_          = seed;
  byte numberOfSteps  = 6;
  byte currentStep    = seed % numberOfSteps;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 0) {
    return;
  } else if (currentStep < 5) {
    g_lampsHelper.showLampsWithSeed(LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW, currentStep);
  } else {
    g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW);
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
