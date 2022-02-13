#include "SharedVariables.h"

PlayerState::PlayerState() {
  byte bonusMultiplier_ = 1;
  byte bonus_           = 0;
  unsigned long score_  = 0;
}

byte PlayerState::bonus() {
  return bonus_;
}

byte PlayerState::bonusMultiplier() {
  return bonusMultiplier_;
}

byte PlayerState::test() {
  return 0;
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

void PlayerState::setBonus(byte value) {
  bonus_ = value;
}

void PlayerState::setBonusMultiplier(byte value) {
  if (value > 4) value = 4;
  bonusMultiplier_ = value;
}

void PlayerState::setScore(unsigned long value, byte player) {
  score_ = value;
}
