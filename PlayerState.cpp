#include "SharedVariables.h"

PlayerState::PlayerState() {
}

PlayerState::PlayerState(byte displayNumber) {
  modeMultiplierQualified_ = false;

  bonusMultiplier_         = 1;
  bonus_                   = 0;
  displayNumber_           = displayNumber;
  modeMultiplier_          = 1;
  score_                   = 0;
  selectedMode_            = 0;
  tempScore_               = 0;

  resetGuardianRollovers();
  resetTopRollovers();

  resetOrbsDropTargets(false);
  resetRightDropTargets(false);

  resetModeStatus();
}

boolean PlayerState::allModesQualified() {
  return modeStatus_[0] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[1] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[2] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[3] != MODE_STATUS_NOT_QUALIFIED;
}

boolean PlayerState::allowRightDropTargetProgress() {
  return modeMultiplier_ != MAX_MODE_MULTIPLIER && !modeMultiplierQualified_;
}

boolean PlayerState::anyModeQualified() {
  return modeStatus_[0] == MODE_STATUS_QUALIFIED ||
         modeStatus_[1] == MODE_STATUS_QUALIFIED ||
         modeStatus_[2] == MODE_STATUS_QUALIFIED ||
         modeStatus_[3] == MODE_STATUS_QUALIFIED;
}

boolean PlayerState::guardianRolloversCompleted() {
  return guardianLights_[0] == true &&
         guardianLights_[1] == true &&
         guardianLights_[2] == true &&
         guardianLights_[3] == true;
}

boolean PlayerState::modeMultiplierQualified() {
  return modeMultiplierQualified_;
}

boolean PlayerState::orbsDropTargetsCompleted() {
  return orbsDropTargets_[0] == true &&
         orbsDropTargets_[1] == true &&
         orbsDropTargets_[2] == true &&
         orbsDropTargets_[3] == true;
}

boolean PlayerState::qualifyMode() {
  if (allModesQualified()) return false;

  modeStatus_[selectedMode_] = MODE_STATUS_QUALIFIED;

  return true;
}

boolean PlayerState::qualifyModeMultiplier() {
  if (modeMultiplier_ == MAX_MODE_MULTIPLIER) return false;

  modeMultiplierQualified_ = true;

  return true;
}

boolean PlayerState::rightDropTargetsCompleted() {
  return rightDropTargets_[0] == true &&
         rightDropTargets_[1] == true &&
         rightDropTargets_[2] == true &&
         rightDropTargets_[3] == true;
}

boolean PlayerState::scoreIncreasing() {
  return score_ < tempScore_;
}

boolean PlayerState::topRolloversCompleted() {
  return topLaneLights_[0] == true &&
         topLaneLights_[1] == true &&
         topLaneLights_[2] == true;
}

byte PlayerState::bonus() {
  return bonus_;
}

byte PlayerState::bonusMultiplier() {
  return bonusMultiplier_;
}

byte PlayerState::startQualifiedMode() {
  launchLockedBallsIntoPlay();

  modeStatus_[selectedMode_] = MODE_STATUS_STARTED;
  if (selectedMode_ == 0) return GAME_MODE_ORBS_1;
  if (selectedMode_ == 1) return GAME_MODE_ORBS_2;
  if (selectedMode_ == 2) return GAME_MODE_ORBS_3;
  if (selectedMode_ == 3) return GAME_MODE_ORBS_4;
}

unsigned long PlayerState::score() {
  return score_;
}

void PlayerState::decreaseBonus(byte amountToSubtract) {
  int currentBonus = bonus();

  if ((currentBonus - amountToSubtract) < 0) {
    setBonus(0);
  } else {
    setBonus(currentBonus - amountToSubtract);
  }
}

void PlayerState::decreaseModeMultiplier() {
  if (modeMultiplier_ == 1) return;

  modeMultiplier_ -= 1;
}

void PlayerState::flashOrbsDropTargetLamps() {
  g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW, true);
  g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW, true);
  g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW, true);
  g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW, true);
}

void PlayerState::flashRightDropTargetLamps() {
  g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW, true);
  g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW, true);
  g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW, true);
  g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW, true);
}

void PlayerState::increaseBonus(byte amountToAdd) {
  bonus_ += amountToAdd;

  if (bonus_ >= MAX_DISPLAY_BONUS) bonus_ = MAX_DISPLAY_BONUS;
}

void PlayerState::increaseBonusMultiplier() {
  byte value = bonusMultiplier_ + 1;
  if      (value == 2) g_soundHelper.playSound(SOUND_2X_COMPLETED);
  else if (value == 3) g_soundHelper.playSound(SOUND_3X_COMPLETED);
  else if (value == 4) g_soundHelper.playSound(SOUND_4X_COMPLETED);
  else if (value == 5) g_soundHelper.playSound(SOUND_5X_COMPLETED);

  setBonusMultiplier(value);
}

void PlayerState::increaseModeMultiplier() {
  modeMultiplierQualified_ = false;
  modeMultiplier_ += 1;
}

void PlayerState::increaseScore(unsigned long amountToAdd) {
  tempScore_ += amountToAdd;
}

void PlayerState::overridePlayerScore(unsigned long value) {
  g_displayHelper.overrideScoreDisplay(displayNumber_, value);
}

void PlayerState::registerGuardianRollover(byte switchHit) {
  if (switchHit == SW_LEFT_OUTLANE)      guardianLights_[0] = true;
  if (switchHit == SW_LEFT_RETURN_LANE)  guardianLights_[1] = true;
  if (switchHit == SW_RIGHT_RETURN_LANE) guardianLights_[2] = true;
  if (switchHit == SW_RIGHT_OUTLANE)     guardianLights_[3] = true;
}

void PlayerState::registerOrbsDropTarget(byte switchHit) {
  if (switchHit == SW_O_DROP_TARGET) orbsDropTargets_[0] = true;
  if (switchHit == SW_R_DROP_TARGET) orbsDropTargets_[1] = true;
  if (switchHit == SW_B_DROP_TARGET) orbsDropTargets_[2] = true;
  if (switchHit == SW_S_DROP_TARGET) orbsDropTargets_[3] = true;
}

void PlayerState::registerRightDropTarget(byte switchHit) {
  if (switchHit == SW_RIGHT_4_DROP_TARGET_1) rightDropTargets_[0] = true;
  if (switchHit == SW_RIGHT_4_DROP_TARGET_2) rightDropTargets_[1] = true;
  if (switchHit == SW_RIGHT_4_DROP_TARGET_3) rightDropTargets_[2] = true;
  if (switchHit == SW_RIGHT_4_DROP_TARGET_4) rightDropTargets_[3] = true;

  if (switchHit == 0xFF) {
    if (rightDropTargets_[0] == false) {
      BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_1, 4, g_machineState.currentTime() + 500);
      return;
    }

    if (rightDropTargets_[1] == false) {
      BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_2, 4, g_machineState.currentTime() + 500);
      return;
    }

    if (rightDropTargets_[2] == false) {
      BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_3, 4, g_machineState.currentTime() + 500);
      return;
    }

    if (rightDropTargets_[3] == false) {
      BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_4, 4, g_machineState.currentTime() + 500);
      return;
    }
  }
}

void PlayerState::registerTopRollover(byte switchHit) {
  if (switchHit == SW_TOP_LEFT_LANE)   topLaneLights_[0]  = true;
  if (switchHit == SW_TOP_MIDDLE_LANE) topLaneLights_[1]  = true;
  if (switchHit == SW_TOP_RIGHT_LANE)  topLaneLights_[2]  = true;
}

void PlayerState::resetGuardianRollovers() {
  guardianLights_[0] = false;
  guardianLights_[1] = false;
  guardianLights_[2] = false;
  guardianLights_[3] = false;
}

void PlayerState::resetModeStatus() {
  modeStatus_[0] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[1] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[2] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[3] = MODE_STATUS_NOT_QUALIFIED;
}

void PlayerState::resetOrbsDropTargets(boolean activateSolenoid) {
  if (activateSolenoid) {
    BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 8, g_machineState.currentTime() + 500);
  }

  orbsDropTargets_[0] = false;
  orbsDropTargets_[1] = false;
  orbsDropTargets_[2] = false;
  orbsDropTargets_[3] = false;
}

void PlayerState::resetPlayerState() {
  modeMultiplierQualified_ = false;

  bonusMultiplier_         = 1;
  bonus_                   = 0;
  modeMultiplier_          = 1;
  score_                   = 0;
  selectedMode_            = 0;
  tempScore_               = 0;

  resetGuardianRollovers();
  resetTopRollovers();

  resetOrbsDropTargets(false);
  resetRightDropTargets(false);

  resetModeStatus();
}

void PlayerState::resetRightDropTargets(boolean activateSolenoid) {
  if (activateSolenoid) {
    BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 8, g_machineState.currentTime() + 500);
  }

  rightDropTargets_[0] = false;
  rightDropTargets_[1] = false;
  rightDropTargets_[2] = false;
  rightDropTargets_[3] = false;

  modeMultiplierQualified_ = false;
}

void PlayerState::resetTopRollovers() {
  topLaneLights_[0] = false;
  topLaneLights_[1] = false;
  topLaneLights_[2] = false;
}

void PlayerState::rotatePlayerLamps() {
  bool tempLight     = guardianLights_[0];
  guardianLights_[0] = guardianLights_[1];
  guardianLights_[1] = guardianLights_[2];
  guardianLights_[2] = guardianLights_[3];
  guardianLights_[3] = tempLight;

  tempLight         = topLaneLights_[2];
  topLaneLights_[2] = topLaneLights_[1];
  topLaneLights_[1] = topLaneLights_[0];
  topLaneLights_[0] = tempLight;
}

void PlayerState::rotateQualifiedMode() {
  byte moddedModeIterator = 0;
  for (byte modeIterator = (selectedMode_ + 1); modeIterator <= (selectedMode_ + 4); modeIterator++) {
    moddedModeIterator = modeIterator % 4;

    if (modeStatus_[moddedModeIterator] == MODE_STATUS_NOT_QUALIFIED) {
      modeStatus_[moddedModeIterator] = MODE_STATUS_QUALIFIED;
      modeStatus_[selectedMode_]      = MODE_STATUS_NOT_QUALIFIED;
      selectedMode_                   = moddedModeIterator;

      return;
    }
  }
}

void PlayerState::setBonus(byte value) {
  bonus_ = value;
}

void PlayerState::setBonusMultiplier(byte value) {
  if (value > MAX_BONUS_MULTIPLIER) value = MAX_BONUS_MULTIPLIER;
  bonusMultiplier_ = value;
}

void PlayerState::setScore(unsigned long value) {
  score_     = value;
  tempScore_ = value;
}

void PlayerState::updateBonusLamps() {
  g_lampsHelper.showBonusLamps(bonus_);
  g_lampsHelper.showBonusMultiplierLamps(bonusMultiplier_);
}

void PlayerState::updateCaptiveOrbsLamps() {
  for (byte modeIterator = 0; modeIterator < 4; modeIterator++) {
    if (modeStatus_[modeIterator] == MODE_STATUS_NOT_QUALIFIED) g_lampsHelper.hideLamp(captiveOrbsLamps_[modeIterator]);
    if (modeStatus_[modeIterator] == MODE_STATUS_QUALIFIED)     g_lampsHelper.showLamp(captiveOrbsLamps_[modeIterator], true);
    if (modeStatus_[modeIterator] == MODE_STATUS_STARTED)       g_lampsHelper.showLamp(captiveOrbsLamps_[modeIterator]);
  }
}

void PlayerState::updateGuardianRolloverLamps() {
  guardianLights_[0] ? g_lampsHelper.showLamp(LAMP_LEFT_OUT_ROLLOVER)     : g_lampsHelper.hideLamp(LAMP_LEFT_OUT_ROLLOVER);
  guardianLights_[1] ? g_lampsHelper.showLamp(LAMP_LEFT_RETURN_ROLLOVER)  : g_lampsHelper.hideLamp(LAMP_LEFT_RETURN_ROLLOVER);
  guardianLights_[2] ? g_lampsHelper.showLamp(LAMP_RIGHT_RETURN_ROLLOVER) : g_lampsHelper.hideLamp(LAMP_RIGHT_RETURN_ROLLOVER);
  guardianLights_[3] ? g_lampsHelper.showLamp(LAMP_RIGHT_OUT_ROLLOVER)    : g_lampsHelper.hideLamp(LAMP_RIGHT_OUT_ROLLOVER);
}

void PlayerState::updateModeMultiplierLamps() {
  boolean flashLamps = true;
  if (anyModeStarted()) flashLamps = false;

  if (modeMultiplier_ == 1) {
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_2X);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_3X);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_4X);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_5X);
  } else if (modeMultiplier_ == 2) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X, flashLamps);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_3X);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_4X);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_5X);
  } else if (modeMultiplier_ == 3) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X, flashLamps);
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_3X, flashLamps);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_4X);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_5X);
  } else if (modeMultiplier_ == 4) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X, flashLamps);
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_3X, flashLamps);
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X, flashLamps);
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_5X);
  } else if (modeMultiplier_ == 5) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X, flashLamps);
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_3X, flashLamps);
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X, flashLamps);
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_5X, flashLamps);
  }
}

void PlayerState::updateOrbsDropTargetLamps() {
  orbsDropTargets_[0] ? g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_O_DROP_TARGET_ARROW);
  orbsDropTargets_[1] ? g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_R_DROP_TARGET_ARROW);
  orbsDropTargets_[2] ? g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_B_DROP_TARGET_ARROW);
  orbsDropTargets_[3] ? g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_S_DROP_TARGET_ARROW);
}

void PlayerState::updatePlayerScore(boolean flashCurrent, boolean dashCurrent) {
  unsigned long seed = g_machineState.currentTime() / 50; // .05 seconds

  if (seed != lastFlash_) {
    lastFlash_ = seed;

    if (tempScore_ > score_) {
      unsigned long diff = tempScore_ - score_;
      unsigned long valueToIncrease = 1;

      while((valueToIncrease * 10) < diff) {
        valueToIncrease *= 10;
      }
      score_ += valueToIncrease;
    }
  }

  g_displayHelper.showPlayerScores(displayNumber_, flashCurrent, dashCurrent);
}

void PlayerState::updateRightDropTargetLamps() {
  rightDropTargets_[0] ? g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW) : g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
  rightDropTargets_[1] ? g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW) : g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
  rightDropTargets_[2] ? g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW) : g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
  rightDropTargets_[3] ? g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW) : g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
}

void PlayerState::updateRightDropTargetResetLamp() {
  if (modeMultiplierQualified_) {
    g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW, true);
  } else {
    g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
  }
}

void PlayerState::updateRightDropTargetSpotLamp() {
  if (allowRightDropTargetProgress() && !rightDropTargetsCompleted()) {
    g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);
  } else {
    g_lampsHelper.hideLamp(LAMP_SPOT_1_THROUGH_4);
  }
}

void PlayerState::updateRightOrbsReleaseLamp() {
  if (anyModeQualified()) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_RELEASE_ORBS, true);
  } else {
    g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_RELEASE_ORBS);
  }
}

void PlayerState::updateSelectedMode() {
  if (allModesQualified()) return;

  for (byte modeIterator = 0; modeIterator < 4; modeIterator++) {
    if (modeStatus_[modeIterator] == MODE_STATUS_NOT_QUALIFIED) {
      selectedMode_ = modeIterator;

      return;
    }
  }
}

void PlayerState::updateTopRolloverLamps() {
  topLaneLights_[0] ? g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER)   : g_lampsHelper.hideLamp(LAMP_TOP_LEFT_ROLLOVER);
  topLaneLights_[1] ? g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER) : g_lampsHelper.hideLamp(LAMP_TOP_MIDDLE_ROLLOVER);
  topLaneLights_[2] ? g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER)  : g_lampsHelper.hideLamp(LAMP_TOP_RIGHT_ROLLOVER);
}

/*********************************************************************
    Private
*********************************************************************/
boolean PlayerState::anyModeStarted() {
  return modeStatus_[0] == MODE_STATUS_STARTED ||
         modeStatus_[1] == MODE_STATUS_STARTED ||
         modeStatus_[2] == MODE_STATUS_STARTED ||
         modeStatus_[3] == MODE_STATUS_STARTED;
}

void PlayerState::launchLockedBallsIntoPlay() {
  int lag = 0;

  for (byte iterator = 1; iterator < modeMultiplier_; iterator++) {
    BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE, 4, g_machineState.currentTime() + 100 + lag);
    BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, 6, g_machineState.currentTime() + 1000 + lag);
    lag += 2000;

    g_machineState.increaseNumberOfBallsInPlay();
  }
}
