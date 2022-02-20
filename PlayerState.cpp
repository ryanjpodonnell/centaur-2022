#include "SharedVariables.h"

PlayerState::PlayerState() {
  bonusMultiplier_ = 1;
  bonus_           = 0;
  score_           = 0;

  guardianLights_[0] = false;
  guardianLights_[1] = false;
  guardianLights_[2] = false;
  guardianLights_[3] = false;
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

void PlayerState::registerGuardianRollover(byte switchHit) {
  if (switchHit == SW_LEFT_OUTLANE)      guardianLights_[0] = true;
  if (switchHit == SW_LEFT_RETURN_LANE)  guardianLights_[1] = true;
  if (switchHit == SW_RIGHT_RETURN_LANE) guardianLights_[2] = true;
  if (switchHit == SW_RIGHT_OUTLANE)     guardianLights_[3] = true;

  if (guardianLights_[0] == true && guardianLights_[1] == true && guardianLights_[2] == true && guardianLights_[3] == true) {
    guardianLights_[0] = false;
    guardianLights_[1] = false;
    guardianLights_[2] = false;
    guardianLights_[3] = false;
  }
}

void PlayerState::resetPlayerState() {
  bonusMultiplier_ = 1;
  bonus_           = 0;
  score_           = 0;

  guardianLights_[0] = false;
  guardianLights_[1] = false;
  guardianLights_[2] = false;
  guardianLights_[3] = false;
}

void PlayerState::rotatePlayerLamps() {
  bool tempGuardianLight = guardianLights_[0];
  guardianLights_[0]     = guardianLights_[1];
  guardianLights_[1]     = guardianLights_[2];
  guardianLights_[2]     = guardianLights_[3];
  guardianLights_[3]     = tempGuardianLight;
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

void PlayerState::showPlayerLamps() {
  guardianLights_[0] ? g_lampsHelper.showLamp(LAMP_LEFT_OUT_ROLLOVER)     : g_lampsHelper.hideLamp(LAMP_LEFT_OUT_ROLLOVER);
  guardianLights_[1] ? g_lampsHelper.showLamp(LAMP_LEFT_RETURN_ROLLOVER)  : g_lampsHelper.hideLamp(LAMP_LEFT_RETURN_ROLLOVER);
  guardianLights_[2] ? g_lampsHelper.showLamp(LAMP_RIGHT_RETURN_ROLLOVER) : g_lampsHelper.hideLamp(LAMP_RIGHT_RETURN_ROLLOVER);
  guardianLights_[3] ? g_lampsHelper.showLamp(LAMP_RIGHT_OUT_ROLLOVER)    : g_lampsHelper.hideLamp(LAMP_RIGHT_OUT_ROLLOVER);
}
