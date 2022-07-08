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
  if (g_gameMode.gameMode() == GAME_MODE_UNSTRUCTURED_PLAY) {
    g_lampsHelper.hideLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS);
    g_lampsHelper.hideLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS);
    g_machineState.updateOrbsDropTargetsLamps();
    g_machineState.updateRightDropTargetsLamps();
  }
}

void BonusLightShow::reset() {
  ended_   = false;
  running_ = false;

  endTime_   = 0;
  lastFlash_ = 0;
}

void BonusLightShow::run() {
  if (endTime_ && (g_machineState.currentTime() > endTime_)) {
    end();
    return;
  }

  unsigned long seed = g_machineState.currentTime() / 100;   // .10 seconds
  if (seed == lastFlash_) return;

  lastFlash_ = seed;
  switch (lightShowId_) {
    case BONUS_LIGHT_SHOW_ORBS_AND_RIGHT_DROPS_ARROW:
      manageOrbsAndRightDropsArrowShow();
      break;

    case BONUS_LIGHT_SHOW_ORBS_DROPS_ARROW:
      manageOrbsDropsArrowShow();
      break;

    case BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW:
      manageResetArrowShow();
      break;

    case BONUS_LIGHT_SHOW_RIGHT_DROPS_ARROW:
      manageRightDropsArrowShow();
      break;

    case BONUS_LIGHT_SHOW_SPIN:
      manageSpinShow();
      break;
  }
}

void BonusLightShow::start(byte lightShowId) {
  if (running_) return;

  ended_       = false;
  running_     = true;
  lightShowId_ = lightShowId;
  endTime_     = g_machineState.currentTime() + showDurations_[lightShowId_];

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  g_soundHelper.playSoundWithoutInterruptions(SOUND_BONUS);
}

/*********************************************************************
    Private
*********************************************************************/
void BonusLightShow::manageOrbsAndRightDropsArrowShow() {
  byte currentStep = lastFlash_ % 11;

  if (currentStep == 0)  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 1)  g_lampsHelper.showLamp(LAMP_60K_BONUS);
  if (currentStep == 2)  g_lampsHelper.showLamp(LAMP_40K_BONUS);
  if (currentStep == 3)  g_lampsHelper.showLamp(LAMP_20K_BONUS);
  if (currentStep == 4)  g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW);
  if (currentStep == 5)  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 6)  g_lampsHelper.showLamp(LAMP_7K_BONUS);
  if (currentStep == 7)  g_lampsHelper.showLamp(LAMP_4X_BONUS);
  if (currentStep == 8)  g_lampsHelper.showLamp(LAMP_40K_BONUS);
  if (currentStep == 9)  g_lampsHelper.showLamp(LAMP_3X_BONUS);
  if (currentStep == 10) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW);
}

void BonusLightShow::manageOrbsDropsArrowShow() {
  byte currentStep = lastFlash_ % 5;

  if (currentStep == 0) g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 1) g_lampsHelper.showLamp(LAMP_60K_BONUS);
  if (currentStep == 2) g_lampsHelper.showLamp(LAMP_40K_BONUS);
  if (currentStep == 3) g_lampsHelper.showLamp(LAMP_20K_BONUS);
  if (currentStep == 4) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW);
}

void BonusLightShow::manageResetArrowShow() {
  byte currentStep = lastFlash_ % 6;

  if (currentStep == 0) g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 1) g_lampsHelper.showLamp(LAMP_4K_BONUS);
  if (currentStep == 2) g_lampsHelper.showLamp(LAMP_5X_BONUS);
  if (currentStep == 3) g_lampsHelper.showLamp(LAMP_40K_BONUS);
  if (currentStep == 4) g_lampsHelper.showLamp(LAMP_2X_BONUS);
  if (currentStep == 5) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_RESET_ARROW);
}

void BonusLightShow::manageRightDropsArrowShow() {
  byte currentStep = lastFlash_ % 6;

  if (currentStep == 0) g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 1) g_lampsHelper.showLamp(LAMP_7K_BONUS);
  if (currentStep == 2) g_lampsHelper.showLamp(LAMP_4X_BONUS);
  if (currentStep == 3) g_lampsHelper.showLamp(LAMP_40K_BONUS);
  if (currentStep == 4) g_lampsHelper.showLamp(LAMP_3X_BONUS);
  if (currentStep == 5) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW);
}

void BonusLightShow::manageSpinShow() {
  byte currentStep = lastFlash_ % 5;

  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  if (currentStep == 0) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_1);
  if (currentStep == 1) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_2);
  if (currentStep == 2) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_3);
  if (currentStep == 3) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_4);
  if (currentStep == 4) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_5);
}
