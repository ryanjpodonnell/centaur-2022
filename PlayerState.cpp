#include "SharedVariables.h"

PlayerState::PlayerState() {
}

PlayerState::PlayerState(byte displayNumber) {
  displayNumber_ = displayNumber;

  resetPlayerState();
}

boolean PlayerState::allModesQualified() {
  return modeStatus_[0] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[1] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[2] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[3] != MODE_STATUS_NOT_QUALIFIED;
}

boolean PlayerState::allowRightDropTargetProgress() {
  return (qualifiedScoreMultiplier_ != MAX_MODE_MULTIPLIER) && !rightDropTargetsResetQualified_;
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

boolean PlayerState::orbsDropTargetsAllStanding() {
  return orbsDropTargets_[0] == false &&
         orbsDropTargets_[1] == false &&
         orbsDropTargets_[2] == false &&
         orbsDropTargets_[3] == false;
}

boolean PlayerState::orbsDropTargetsCompletedInOrder() {
  return orbsDropTargetsCompletedInOrder_;
}

boolean PlayerState::rightDropTargetsResetQualified() {
  return rightDropTargetsResetQualified_;
}

boolean PlayerState::orbsDropTargetsCompleted() {
  return orbsDropTargets_[0] == true &&
         orbsDropTargets_[1] == true &&
         orbsDropTargets_[2] == true &&
         orbsDropTargets_[3] == true;
}

boolean PlayerState::rightDropTargetsAllStanding() {
  return rightDropTargets_[0] == false &&
         rightDropTargets_[1] == false &&
         rightDropTargets_[2] == false &&
         rightDropTargets_[3] == false;
}

boolean PlayerState::rightDropTargetsCompleted() {
  return rightDropTargets_[0] == true &&
         rightDropTargets_[1] == true &&
         rightDropTargets_[2] == true &&
         rightDropTargets_[3] == true;
}

boolean PlayerState::rightDropTargetsCompletedInOrder() {
  return rightDropTargetsCompletedInOrder_;
}

boolean PlayerState::scoreIncreasing() {
  return score_ < tempScore_;
}

boolean PlayerState::topRolloversCompleted() {
  return topLaneLights_[0] == true &&
         topLaneLights_[1] == true &&
         topLaneLights_[2] == true;
}

byte PlayerState::activeOrbsDropTarget() {
  return activeOrbsDropTarget_;
}

byte PlayerState::activeRightDropTarget() {
  return activeRightDropTarget_;
}

byte PlayerState::bonus() {
  return bonus_;
}

byte PlayerState::bonusMultiplier() {
  return bonusMultiplier_;
}

byte PlayerState::qualifiedScoreMultiplier() {
  return qualifiedScoreMultiplier_;
}

byte PlayerState::queensChamberBonusValue() {
  return queensChamberBonusValue_;
}

byte PlayerState::startQualifiedMode() {
  launchLockedBallsIntoPlay();

  qualifiedScoreMultiplier_ = 1;
  modeStatus_[selectedMode_] = MODE_STATUS_STARTED;

  if (selectedMode_ == 0) return GAME_MODE_ORBS_1;
  if (selectedMode_ == 1) return GAME_MODE_ORBS_2;
  if (selectedMode_ == 2) return GAME_MODE_ORBS_1;
  if (selectedMode_ == 3) return GAME_MODE_ORBS_1;
}

unsigned long PlayerState::dropRightDropTargets(unsigned long activationTime) {
  if (rightDropTargets_[0] == true) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_1, SOL_SINGLE_DROP_STRENGTH, activationTime);
    activationTime += SOLENOID_BUFFER_TIME;
  }
  if (rightDropTargets_[1] == true) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_2, SOL_SINGLE_DROP_STRENGTH, activationTime);
    activationTime += SOLENOID_BUFFER_TIME;
  }
  if (rightDropTargets_[2] == true) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_3, SOL_SINGLE_DROP_STRENGTH, activationTime);
    activationTime += SOLENOID_BUFFER_TIME;
  }
  if (rightDropTargets_[3] == true) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_4, SOL_SINGLE_DROP_STRENGTH, activationTime);
    activationTime += SOLENOID_BUFFER_TIME;
  }

  return activationTime;
}

unsigned long PlayerState::queensChamberHurryUpValue() {
  if (activeInlineDropTarget_ == SW_1ST_INLINE_DROP_TARGET) return 100000;
  if (activeInlineDropTarget_ == SW_2ND_INLINE_DROP_TARGET) return 200000;
  if (activeInlineDropTarget_ == SW_3RD_INLINE_DROP_TARGET) return 300000;
  if (activeInlineDropTarget_ == SW_4TH_INLINE_DROP_TARGET) return 400000;
  if (activeInlineDropTarget_ == SW_INLINE_BACK_TARGET)     return 500000;
}

unsigned long PlayerState::queensChamberScoreValue() {
  return queensChamberScoreValue_;
}

unsigned long PlayerState::rightDropTargetsScoreValue() {
  return rightDropTargetsScoreValue_;
}

unsigned long PlayerState::score() {
  return score_;
}

void PlayerState::completeSelectedMode() {
  modeStatus_[selectedMode_] = MODE_STATUS_COMPLETED;
}

void PlayerState::decreaseBonus() {
  bonus_ -= 1;
}

void PlayerState::flashOrbsDropTargetsLamps() {
  g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW, true);
  g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW, true);
  g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW, true);
  g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW, true);
}

void PlayerState::flashRightDropTargetsLamps() {
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
  if      (value == 2) g_soundHelper.playSoundWithoutInterruptions(SOUND_2X_COMPLETED);
  else if (value == 3) g_soundHelper.playSoundWithoutInterruptions(SOUND_3X_COMPLETED);
  else if (value == 4) g_soundHelper.playSoundWithoutInterruptions(SOUND_4X_COMPLETED);
  else if (value == 5) g_soundHelper.playSoundWithoutInterruptions(SOUND_5X_COMPLETED);

  setBonusMultiplier(value);
}

void PlayerState::increaseQualifiedScoreMultiplier() {
  qualifiedScoreMultiplier_ += 1;
}

void PlayerState::increaseScore(unsigned long amountToAdd) {
  tempScore_ += (amountToAdd * g_machineState.numberOfBallsInPlay());
}

void PlayerState::manageInlineTargetScoring(byte switchHit) {
  if (switchHit == activeInlineDropTarget_) {
    if (switchHit == SW_1ST_INLINE_DROP_TARGET) {
      queensChamberBonusValue_ = 1;
      queensChamberScoreValue_ = 10000;
      activeInlineDropTarget_ = SW_2ND_INLINE_DROP_TARGET;
    }
    if (switchHit == SW_2ND_INLINE_DROP_TARGET) {
      queensChamberBonusValue_ = 2;
      queensChamberScoreValue_ = 20000;
      activeInlineDropTarget_ = SW_3RD_INLINE_DROP_TARGET;
    }
    if (switchHit == SW_3RD_INLINE_DROP_TARGET) {
      queensChamberBonusValue_ = 3;
      queensChamberScoreValue_ = 30000;
      activeInlineDropTarget_ = SW_4TH_INLINE_DROP_TARGET;
    }
    if (switchHit == SW_4TH_INLINE_DROP_TARGET) {
      queensChamberBonusValue_ = 4;
      queensChamberScoreValue_ = 40000;
      activeInlineDropTarget_ = SW_INLINE_BACK_TARGET;
    }
    if (switchHit == SW_INLINE_BACK_TARGET) {
      queensChamberBonusValue_ = 5;
      queensChamberScoreValue_ = 50000;
    }
  }
}

void PlayerState::manageOrbsDropTargetScoring(byte switchHit) {
  if (switchHit == activeOrbsDropTarget_) {
    if (switchHit == SW_O_DROP_TARGET) {
      activeOrbsDropTarget_ = SW_R_DROP_TARGET;
    }
    if (switchHit == SW_R_DROP_TARGET) {
      activeOrbsDropTarget_ = SW_B_DROP_TARGET;
    }
    if (switchHit == SW_B_DROP_TARGET) {
      activeOrbsDropTarget_ = SW_S_DROP_TARGET;
    }
    if (switchHit == SW_S_DROP_TARGET) {
      activeOrbsDropTarget_ = 0xFF;
    }
  } else if (switchHit != activeOrbsDropTarget_) {
    activeOrbsDropTarget_ = 0xFF;
    orbsDropTargetsCompletedInOrder_ = false;
  }
}

void PlayerState::manageRightDropTargetScoring(byte switchHit) {
  if (switchHit == activeRightDropTarget_) {
    if (switchHit == SW_RIGHT_4_DROP_TARGET_1) {
      rightDropTargetsScoreValue_ = 10000;
      activeRightDropTarget_ = SW_RIGHT_4_DROP_TARGET_2;
    }
    if (switchHit == SW_RIGHT_4_DROP_TARGET_2) {
      rightDropTargetsScoreValue_ = 20000;
      activeRightDropTarget_ = SW_RIGHT_4_DROP_TARGET_3;
    }
    if (switchHit == SW_RIGHT_4_DROP_TARGET_3) {
      rightDropTargetsScoreValue_ = 40000;
      activeRightDropTarget_ = SW_RIGHT_4_DROP_TARGET_4;
    }
    if (switchHit == SW_RIGHT_4_DROP_TARGET_4) {
      rightDropTargetsScoreValue_ = 80000;
      activeRightDropTarget_ = 0xFF;
    }
  } else if (switchHit != activeRightDropTarget_) {
    rightDropTargetsScoreValue_ = 500;
    activeRightDropTarget_ = 0xFF;
    rightDropTargetsCompletedInOrder_ = false;
  }
}

void PlayerState::overridePlayerScore(unsigned long value) {
  byte adjacentScoreDisplay = (displayNumber_ + 2) % 4;
  g_displayHelper.overrideScoreDisplay(adjacentScoreDisplay, value);
}

void PlayerState::qualifyMode() {
  byte moddedModeIterator = 0;
  for (byte modeIterator = selectedMode_; modeIterator < (selectedMode_ + 4); modeIterator++) {
    moddedModeIterator = modeIterator % 4;

    if (modeStatus_[moddedModeIterator] == MODE_STATUS_NOT_QUALIFIED) {
      modeStatus_[moddedModeIterator] = MODE_STATUS_QUALIFIED;
      selectedMode_ = moddedModeIterator;

      return;
    }
  }
}

void PlayerState::qualifyRightDropTargetsReset() {
  rightDropTargetsResetQualified_ = true;
}

void PlayerState::registerGuardianRollover(byte switchHit) {
  if (switchHit == SW_LEFT_OUTLANE) {
    if (guardianLights_[0] == true) g_soundHelper.playSoundWithoutInterruptions(SOUND_SLOW_ARENT_YOU);
    guardianLights_[0] = true;
  }
  if (switchHit == SW_LEFT_RETURN_LANE) {
    if (guardianLights_[1] == true) g_soundHelper.playSoundWithoutInterruptions(SOUND_SLOW_ARENT_YOU);
    guardianLights_[1] = true;
  }
  if (switchHit == SW_RIGHT_RETURN_LANE) {
    if (guardianLights_[2] == true) g_soundHelper.playSoundWithoutInterruptions(SOUND_SLOW_ARENT_YOU);
    guardianLights_[2] = true;
  }
  if (switchHit == SW_RIGHT_OUTLANE) {
    if (guardianLights_[3] == true) g_soundHelper.playSoundWithoutInterruptions(SOUND_SLOW_ARENT_YOU);
    guardianLights_[3] = true;
  }
}

void PlayerState::registerInlineDropTarget(byte switchHit) {
  if (switchHit == SW_1ST_INLINE_DROP_TARGET) {
    if (inlineDropTargets_[0] == true) return;
    inlineDropTargets_[0] = true;
  }
  if (switchHit == SW_2ND_INLINE_DROP_TARGET) {
    if (inlineDropTargets_[1] == true) return;
    inlineDropTargets_[1] = true;
  }
  if (switchHit == SW_3RD_INLINE_DROP_TARGET) {
    if (inlineDropTargets_[2] == true) return;
    inlineDropTargets_[2] = true;
  }
  if (switchHit == SW_4TH_INLINE_DROP_TARGET) {
    if (inlineDropTargets_[3] == true) return;
    inlineDropTargets_[3] = true;
  }
}

void PlayerState::registerOrbsDropTarget(byte switchHit) {
  if (switchHit == SW_O_DROP_TARGET) {
    if (orbsDropTargets_[0] == true) return;
    orbsDropTargets_[0] = true;
  }
  if (switchHit == SW_R_DROP_TARGET) {
    if (orbsDropTargets_[1] == true) return;
    orbsDropTargets_[1] = true;
  }
  if (switchHit == SW_B_DROP_TARGET) {
    if (orbsDropTargets_[2] == true) return;
    orbsDropTargets_[2] = true;
  }
  if (switchHit == SW_S_DROP_TARGET) {
    if (orbsDropTargets_[3] == true) return;
    orbsDropTargets_[3] = true;
  }
}

void PlayerState::registerRightDropTarget(byte switchHit) {
  if (switchHit == SW_RIGHT_4_DROP_TARGET_1) {
    if (rightDropTargets_[0] == true) return;
    rightDropTargets_[0] = true;
  }
  if (switchHit == SW_RIGHT_4_DROP_TARGET_2) {
    if (rightDropTargets_[1] == true) return;
    rightDropTargets_[1] = true;
  }
  if (switchHit == SW_RIGHT_4_DROP_TARGET_3) {
    if (rightDropTargets_[2] == true) return;
    rightDropTargets_[2] = true;
  }
  if (switchHit == SW_RIGHT_4_DROP_TARGET_4) {
    if (rightDropTargets_[3] == true) return;
    rightDropTargets_[3] = true;
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

void PlayerState::resetInlineDropTargets() {
  activeInlineDropTarget_ = SW_1ST_INLINE_DROP_TARGET;

  inlineDropTargets_[0] = false;
  inlineDropTargets_[1] = false;
  inlineDropTargets_[2] = false;
  inlineDropTargets_[3] = false;
}

void PlayerState::resetModeStatus() {
  modeStatus_[0] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[1] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[2] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[3] = MODE_STATUS_NOT_QUALIFIED;
}

void PlayerState::resetOrbsDropTargets() {
  activeOrbsDropTarget_            = SW_O_DROP_TARGET;
  orbsDropTargetsCompletedInOrder_ = true;

  orbsDropTargets_[0] = false;
  orbsDropTargets_[1] = false;
  orbsDropTargets_[2] = false;
  orbsDropTargets_[3] = false;
}

void PlayerState::resetPlayerState() {
  activeRightDropTarget_            = SW_RIGHT_4_DROP_TARGET_1;
  rightDropTargetsCompletedInOrder_ = true;

  activeOrbsDropTarget_             = SW_O_DROP_TARGET;
  orbsDropTargetsCompletedInOrder_  = true;

  activeInlineDropTarget_           = SW_1ST_INLINE_DROP_TARGET;

  rightDropTargetsResetQualified_ = false;

  bonusMultiplier_          = 1;
  bonus_                    = 0;
  qualifiedScoreMultiplier_ = 1;
  queensChamberBonusValue_  = 1;
  queensChamberScoreValue_  = 10000;
  score_                    = 0;
  selectedMode_             = 0;
  tempScore_                = 0;

  resetGuardianRollovers();
  resetTopRollovers();

  resetInlineDropTargets();
  resetOrbsDropTargets();
  resetRightDropTargets();

  resetModeStatus();
}

void PlayerState::resetRightDropTargets() {
  activeRightDropTarget_            = SW_RIGHT_4_DROP_TARGET_1;
  rightDropTargetsCompletedInOrder_ = true;

  rightDropTargets_[0] = false;
  rightDropTargets_[1] = false;
  rightDropTargets_[2] = false;
  rightDropTargets_[3] = false;

  rightDropTargetsResetQualified_ = false;
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

  for (byte modeIterator = (selectedMode_ + 1); modeIterator < (selectedMode_ + 4); modeIterator++) {
    moddedModeIterator = modeIterator % 4;

    if (modeStatus_[moddedModeIterator] == MODE_STATUS_NOT_QUALIFIED) {
      modeStatus_[moddedModeIterator] = MODE_STATUS_QUALIFIED;
      modeStatus_[selectedMode_] = MODE_STATUS_NOT_QUALIFIED;
      selectedMode_ = moddedModeIterator;

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

void PlayerState::unqualifyMode() {
  modeStatus_[selectedMode_] = MODE_STATUS_NOT_QUALIFIED;
}

void PlayerState::unqualifyRightDropTargetsReset() {
  rightDropTargetsResetQualified_ = false;
}

void PlayerState::updateBonusLamps() {
  g_lampsHelper.showBonusLamps(bonus_);
  g_lampsHelper.showBonusMultiplierLamps(bonusMultiplier_);
}

void PlayerState::updateCaptiveOrbsLamps() {
  for (byte modeIterator = 0; modeIterator < 4; modeIterator++) {
    char buf[128];
    sprintf(buf, "Mode %d status = %d\n", modeIterator, modeStatus_[modeIterator]);
    Serial.write(buf);

    if (modeStatus_[modeIterator] == MODE_STATUS_NOT_QUALIFIED) g_lampsHelper.hideLamp(captiveOrbsLamps_[modeIterator]);
    if (modeStatus_[modeIterator] == MODE_STATUS_QUALIFIED)     g_lampsHelper.showLamp(captiveOrbsLamps_[modeIterator], true);
    if (modeStatus_[modeIterator] == MODE_STATUS_COMPLETED)     g_lampsHelper.showLamp(captiveOrbsLamps_[modeIterator]);
  }
}

void PlayerState::updateGuardianRolloverLamps() {
  guardianLights_[0] ? g_lampsHelper.showLamp(LAMP_LEFT_OUT_ROLLOVER)     : g_lampsHelper.hideLamp(LAMP_LEFT_OUT_ROLLOVER);
  guardianLights_[1] ? g_lampsHelper.showLamp(LAMP_LEFT_RETURN_ROLLOVER)  : g_lampsHelper.hideLamp(LAMP_LEFT_RETURN_ROLLOVER);
  guardianLights_[2] ? g_lampsHelper.showLamp(LAMP_RIGHT_RETURN_ROLLOVER) : g_lampsHelper.hideLamp(LAMP_RIGHT_RETURN_ROLLOVER);
  guardianLights_[3] ? g_lampsHelper.showLamp(LAMP_RIGHT_OUT_ROLLOVER)    : g_lampsHelper.hideLamp(LAMP_RIGHT_OUT_ROLLOVER);
}

void PlayerState::updateOrbsDropTargetsLamps() {
  boolean shouldFlashLamps = orbsDropTargetsCompletedInOrder() ? true : false;

  orbsDropTargets_[0] ? g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW, shouldFlashLamps) : g_lampsHelper.hideLamp(LAMP_O_DROP_TARGET_ARROW);
  orbsDropTargets_[1] ? g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW, shouldFlashLamps) : g_lampsHelper.hideLamp(LAMP_R_DROP_TARGET_ARROW);
  orbsDropTargets_[2] ? g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW, shouldFlashLamps) : g_lampsHelper.hideLamp(LAMP_B_DROP_TARGET_ARROW);
  orbsDropTargets_[3] ? g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW, shouldFlashLamps) : g_lampsHelper.hideLamp(LAMP_S_DROP_TARGET_ARROW);
}

void PlayerState::updatePlayerScore(boolean flashCurrent, boolean dashCurrent) {
  unsigned long seed = g_machineState.currentTime() / 50; // .05 seconds

  if (seed != lastFlash_) {
    lastFlash_ = seed;

    if (tempScore_ > score_) {
      unsigned long diff = tempScore_ - score_;
      unsigned long valueToIncrease = 1;

      while((valueToIncrease * 10) <= diff) {
        valueToIncrease *= 10;
      }
      score_ += valueToIncrease;
    }
  }

  g_displayHelper.showPlayerScores(displayNumber_, flashCurrent, dashCurrent);
}

void PlayerState::updateQueensChamberLamps() {
  if (activeInlineDropTarget_ == SW_1ST_INLINE_DROP_TARGET) {
    g_lampsHelper.showLamp(LAMP_10_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
  } else if (activeInlineDropTarget_ == SW_2ND_INLINE_DROP_TARGET) {
    g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
    g_lampsHelper.showLamp(LAMP_20_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
  } else if (activeInlineDropTarget_ == SW_3RD_INLINE_DROP_TARGET) {
    g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
    g_lampsHelper.showLamp(LAMP_30_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
  } else if (activeInlineDropTarget_ == SW_4TH_INLINE_DROP_TARGET) {
    g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
    g_lampsHelper.showLamp(LAMP_40_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
  } else if (activeInlineDropTarget_ == SW_INLINE_BACK_TARGET) {
    g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
    g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
    g_lampsHelper.showLamp(LAMP_50_CHAMBER);
  }
}

void PlayerState::updateRightDropTargetsLamps() {
  if (activeRightDropTarget_ == 0xFF) {
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
  }
  if (activeRightDropTarget_ == SW_RIGHT_4_DROP_TARGET_1) {
    g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW, true);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
  }
  if (activeRightDropTarget_ == SW_RIGHT_4_DROP_TARGET_2) {
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
    g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW, true);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
  }
  if (activeRightDropTarget_ == SW_RIGHT_4_DROP_TARGET_3) {
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
    g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW, true);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
  }
  if (activeRightDropTarget_ == SW_RIGHT_4_DROP_TARGET_4) {
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
    g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
    g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW, true);
  }
}

void PlayerState::updateRightDropTargetsResetLamp() {
  if (rightDropTargetsResetQualified_) {
    g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW, true);
  } else {
    g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
  }
}

void PlayerState::updateRightDropTargetsSpotLamp() {
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

void PlayerState::updateScoreMultiplierLamps() {
  if (qualifiedScoreMultiplier_ < 2 && g_machineState.numberOfBallsInPlay() < 2) g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_2X);
  if (qualifiedScoreMultiplier_ < 3 && g_machineState.numberOfBallsInPlay() < 3) g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_3X);
  if (qualifiedScoreMultiplier_ < 4 && g_machineState.numberOfBallsInPlay() < 4) g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_4X);
  if (qualifiedScoreMultiplier_ < 5 && g_machineState.numberOfBallsInPlay() < 5) g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_5X);

  if (g_machineState.numberOfBallsInPlay() >= 2) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X);
  } else if (qualifiedScoreMultiplier_ >= 2) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X, true);
  }
  if (g_machineState.numberOfBallsInPlay() >= 3) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_3X);
  } else if (qualifiedScoreMultiplier_ >= 3) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_3X, true);
  }
  if (g_machineState.numberOfBallsInPlay() >= 4) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X);
  } else if (qualifiedScoreMultiplier_ >= 4) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X, true);
  }
  if (g_machineState.numberOfBallsInPlay() >= 5) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_5X);
  } else if (qualifiedScoreMultiplier_ >= 5) {
    g_lampsHelper.showLamp(LAMP_RIGHT_LANE_5X, true);
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
void PlayerState::launchLockedBallsIntoPlay() {
  int lag = 0;

  for (byte itr = g_machineState.numberOfBallsInPlay(); itr < qualifiedScoreMultiplier_; itr++) {
    g_machineState.launchBallIntoPlay(lag);
    g_machineState.increaseNumberOfBallsInPlay();

    lag += 2000;
  }
}

void PlayerState::spotRightDropTarget() {
  if (rightDropTargets_[0] == false) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_1, SOL_SINGLE_DROP_STRENGTH, g_machineState.currentTime());
    return;
  }

  if (rightDropTargets_[1] == false) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_2, SOL_SINGLE_DROP_STRENGTH, g_machineState.currentTime());
    return;
  }

  if (rightDropTargets_[2] == false) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_3, SOL_SINGLE_DROP_STRENGTH, g_machineState.currentTime());
    return;
  }

  if (rightDropTargets_[3] == false) {
    BSOS_PushToTimedSolenoidStack(SOL_RIGHT_4_DROP_TARGETS_4, SOL_SINGLE_DROP_STRENGTH, g_machineState.currentTime());
    return;
  }
}
