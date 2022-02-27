#include "SharedVariables.h"

PlayerState::PlayerState() {
  bonusMultiplier_ = 1;
  bonus_           = 0;
  selectedMode_    = 0;
  score_           = 0;

  guardianLights_[0] = false;
  guardianLights_[1] = false;
  guardianLights_[2] = false;
  guardianLights_[3] = false;

  topLaneLights_[0] = false;
  topLaneLights_[1] = false;
  topLaneLights_[2] = false;

  orbsDropTargets_[0] = false;
  orbsDropTargets_[1] = false;
  orbsDropTargets_[2] = false;
  orbsDropTargets_[3] = false;

  modeStatus_[0] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[1] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[2] = MODE_STATUS_NOT_QUALIFIED;
  modeStatus_[3] = MODE_STATUS_NOT_QUALIFIED;

}

boolean PlayerState::orbsDropTargetsCompleted() {
  return orbsDropTargets_[0] == true &&
         orbsDropTargets_[1] == true &&
         orbsDropTargets_[2] == true &&
         orbsDropTargets_[3] == true;
}

byte PlayerState::bonus() {
  return bonus_;
}

byte PlayerState::bonusMultiplier() {
  return bonusMultiplier_;
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

void PlayerState::increaseBonus(byte amountToAdd) {
  bonus_ += amountToAdd;

  if (bonus_ >= MAX_DISPLAY_BONUS) bonus_ = MAX_DISPLAY_BONUS;
}

void PlayerState::increaseScore(unsigned long amountToAdd) {
  score_ += amountToAdd;
}

void PlayerState::qualifyMode() {
  if (allModesQualified()) return;

  modeStatus_[selectedMode_] = MODE_STATUS_QUALIFIED;
}

void PlayerState::registerDropTarget(byte switchHit) {
  if (switchHit == SW_O_DROP_TARGET) orbsDropTargets_[0] = true;
  if (switchHit == SW_R_DROP_TARGET) orbsDropTargets_[1] = true;
  if (switchHit == SW_B_DROP_TARGET) orbsDropTargets_[2] = true;
  if (switchHit == SW_S_DROP_TARGET) orbsDropTargets_[3] = true;
}

void PlayerState::registerGuardianRollover(byte switchHit) {
  if (switchHit == SW_LEFT_OUTLANE)      guardianLights_[0] = true;
  if (switchHit == SW_LEFT_RETURN_LANE)  guardianLights_[1] = true;
  if (switchHit == SW_RIGHT_RETURN_LANE) guardianLights_[2] = true;
  if (switchHit == SW_RIGHT_OUTLANE)     guardianLights_[3] = true;

  if (guardianRolloversCompleted()) {
    guardianLights_[0] = false;
    guardianLights_[1] = false;
    guardianLights_[2] = false;
    guardianLights_[3] = false;
  }
}

void PlayerState::registerTopRollover(byte switchHit) {
  if (switchHit == SW_TOP_LEFT_LANE)   topLaneLights_[0]  = true;
  if (switchHit == SW_TOP_MIDDLE_LANE) topLaneLights_[1]  = true;
  if (switchHit == SW_TOP_RIGHT_LANE)  topLaneLights_[2]  = true;

  if (topRolloversCompleted()) {
    topLaneLights_[0] = false;
    topLaneLights_[1] = false;
    topLaneLights_[2] = false;
  }
}

void PlayerState::resetDropTargets() {
  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, g_machineState.currentTime() + 500);

  orbsDropTargets_[0] = false;
  orbsDropTargets_[1] = false;
  orbsDropTargets_[2] = false;
  orbsDropTargets_[3] = false;
}

void PlayerState::resetPlayerState() {
  bonusMultiplier_ = 1;
  bonus_           = 0;
  score_           = 0;

  guardianLights_[0] = false;
  guardianLights_[1] = false;
  guardianLights_[2] = false;
  guardianLights_[3] = false;

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
  if (allModesQualified() || !anyModeQualified()) return;

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
  if (value > 4) value = 4;
  bonusMultiplier_ = value;
}

void PlayerState::setScore(unsigned long value) {
  score_ = value;
}

void PlayerState::startQualifiedMode() {
  if (!anyModeQualified()) return;

  modeStatus_[selectedMode_] = MODE_STATUS_STARTED;
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

void PlayerState::updateOrbsDropTargetLamps() {
  if (orbsDropTargetsCompleted()) {
    g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW, true);
    g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW, true);
    g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW, true);
    g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW, true);
  } else {
    orbsDropTargets_[0] ? g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_O_DROP_TARGET_ARROW);
    orbsDropTargets_[1] ? g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_R_DROP_TARGET_ARROW);
    orbsDropTargets_[2] ? g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_B_DROP_TARGET_ARROW);
    orbsDropTargets_[3] ? g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW) : g_lampsHelper.hideLamp(LAMP_S_DROP_TARGET_ARROW);
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
boolean PlayerState::allModesQualified() {
  return modeStatus_[0] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[1] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[2] != MODE_STATUS_NOT_QUALIFIED &&
         modeStatus_[3] != MODE_STATUS_NOT_QUALIFIED;
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

boolean PlayerState::topRolloversCompleted() {
  return topLaneLights_[0] == true &&
         topLaneLights_[1] == true &&
         topLaneLights_[2] == true;
}
