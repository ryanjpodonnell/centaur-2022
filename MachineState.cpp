#include "SharedVariables.h"

MachineState::MachineState(byte id) {
  byte bonusMultipliers_[] = { 1, 1, 1, 1};
  byte bonuses[]           = { 0, 0, 0, 0};
  credits_                 = 0;
  currentBallInPlay_       = 0;
  currentPlayer_           = 0;
  extraBallCollected_      = false;
  freePlayMode_            = true;
  highScore_               = 0;
  machineStateChanged_     = true;
  machineStateId_          = id;
  numberOfPlayers_         = 0;
  numberOfTiltWarnings_    = 0;
  samePlayerShootsAgain_   = false;
  unsigned long scores_[]  = { 0, 0, 0, 0 };
}

boolean MachineState::ballSaveUsed() {
  return ballSaveUsed_;
}

boolean MachineState::currentPlayerTilted() {
  return numberOfTiltWarnings_ > MAXIMUM_NUMBER_OF_TILT_WARNINGS;
}

boolean MachineState::incrementNumberOfPlayers() {
  if (credits_ < 1 && !freePlayMode_) return false;
  if (numberOfPlayers_ >= 4) return false;

  if (!freePlayMode_) {
    credits_ -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
    BSOS_SetDisplayCredits(credits_);
  }

  setScore(numberOfPlayers_, 0);
  g_displayHelper.showPlayerScore(numberOfPlayers_);
  numberOfPlayers_ += 1;

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::machineStateChanged() {
  return machineStateChanged_;
}

boolean MachineState::resetPlayers() {
  if (credits_ < 1 && !freePlayMode_) return false;

  if (!freePlayMode_) {
    credits_ -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
    BSOS_SetDisplayCredits(credits_);
  }

  setScore(0, 0);
  g_displayHelper.showPlayerScore(0);
  numberOfPlayers_ = 1;

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::samePlayerShootsAgain() {
  return samePlayerShootsAgain_;
}

byte MachineState::bonus() {
  return bonuses_[currentPlayer_];
}

byte MachineState::bonusMultiplier() {
  return bonusMultipliers_[currentPlayer_];
}

byte MachineState::credits() {
  return credits_;
}

byte MachineState::currentBallInPlay() {
  return currentBallInPlay_;
}

byte MachineState::currentPlayer() {
  return currentPlayer_;
}

byte MachineState::numberOfPlayers() {
  return numberOfPlayers_;
}

byte MachineState::incrementCurrentPlayer() {
  currentPlayer_ += 1;

  if (currentPlayer_ >= numberOfPlayers_) {
    currentPlayer_ = 0;
    currentBallInPlay_ += 1;
  }

  if (currentBallInPlay_ > BALLS_PER_GAME) {
    for (int count = 0; count < numberOfPlayers_; count++) {
      BSOS_SetDisplay(count, scores_[count], true, 2);
    }

    return MACHINE_STATE_INIT_GAMEPLAY;
  } else {
    return MACHINE_STATE_INIT_NEW_BALL;
  }
}

int MachineState::initGamePlay() {
  if (DEBUG_MESSAGES) Serial.write("Starting game\n\r");

  BSOS_SetDisableFlippers(false);
  BSOS_EnableSolenoidStack();
  BSOS_TurnOffAllLamps();

  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, currentTime_ + 500);
  BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 10, currentTime_ + 500);
  BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 10, currentTime_ + 500);

  g_lampsHelper.showLamp(LAMP_PLAYFIELD_GI, false, true);
  g_displayHelper.showPlayerScores(0xFF);

  return MACHINE_STATE_INIT_NEW_BALL;
}

int MachineState::initNewBall(bool curStateChanged) {
  if (curStateChanged) {
    extraBallCollected_    = false;
    samePlayerShootsAgain_ = false;
    scoreMultiplier_       = 1;

    setBonus(0);
    setBonusMultiplier(1);

    BSOS_SetDisplayBallInPlay(currentBallInPlay_);
    BSOS_SetLampState(LAMP_TILT, 0);
    if (BALL_SAVE_NUMBER_OF_SECONDS) g_lampsHelper.showLamp(LAMP_SHOOT_AGAIN, true);
    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime_ + 600);
  }

  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    g_gameMode.setGameMode(GAME_MODE_SKILL_SHOT);
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }
}

int MachineState::machineState() {
  return machineStateId_;
}

int MachineState::resetGame() {
  if (freePlayMode_) {
    return MACHINE_STATE_INIT_GAMEPLAY;
  } else if (credits_ >= 1 && !freePlayMode_) {
    credits_ -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
    return MACHINE_STATE_INIT_GAMEPLAY;
  } else {
    return machineStateId_;
  }
}

unsigned long MachineState::currentTime() {
  return currentTime_;
}

unsigned long MachineState::lastTiltWarningTime() {
  return lastTiltWarningTime_;
}

unsigned long MachineState::score(byte player) {
  if (player == 0xFF) player = currentPlayer_;
  return scores_[player];
}

void MachineState::setMachineState(int id) {
  if (id != machineStateId_) {
    machineStateChanged_ = true;
    machineStateId_ = id;
  } else {
    machineStateChanged_ = false;
  }
}

void MachineState::awardExtraBall() {
  if (extraBallCollected_) return;

  extraBallCollected_ = true;
  samePlayerShootsAgain_ = true;
  BSOS_SetLampState(LAMP_SHOOT_AGAIN, samePlayerShootsAgain_);
}

void MachineState::decreaseBonus(byte amountToSubtract) {
  int currentBonus = bonus();

  if ((currentBonus - amountToSubtract) < 0) {
    setBonus(0);
  } else {
    setBonus(currentBonus - amountToSubtract);
  }
}

void MachineState::increaseBonus(byte amountToAdd) {
  bonuses_[currentPlayer_] += amountToAdd;
  if (bonuses_[currentPlayer_] >= MAX_DISPLAY_BONUS) {
    bonuses_[currentPlayer_] = MAX_DISPLAY_BONUS;
  }
}

void MachineState::increaseCredits(boolean playSound, byte numToAdd) {
  credits_ += numToAdd;
  if (credits_ > MAXIMUM_NUMBER_OF_CREDITS) credits_ = MAXIMUM_NUMBER_OF_CREDITS;

  BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
  BSOS_SetDisplayCredits(credits_);
}

void MachineState::increaseScore(unsigned long amountToAdd) {
  scores_[currentPlayer_] += amountToAdd;
}

void MachineState::readStoredParameters() {
  setHighScore(BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, DEFAULT_HIGH_SCORE));
  setCredits(BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE));
}

void MachineState::registerTiltWarning() {
  if ((currentTime_ - lastTiltWarningTime_) > TILT_WARNING_DEBOUNCE_TIME) {
    lastTiltWarningTime_ = currentTime_;
    numberOfTiltWarnings_ += 1;

    if (currentPlayerTilted()) {
      BSOS_DisableSolenoidStack();
      BSOS_SetDisableFlippers(true);
      BSOS_TurnOffAllLamps();
      BSOS_SetLampState(LAMP_TILT, 1);
    }
  }
}

void MachineState::setBallSaveUsed(byte value) {
  ballSaveUsed_ = value;
}

void MachineState::setBonus(byte value) {
  bonuses_[currentPlayer_] = value;
}

void MachineState::setBonusMultiplier(byte value) {
  if (value > 4) value = 4;
  bonusMultipliers_[currentPlayer_] = value;
}

void MachineState::setCredits(byte value) {
  credits_ = value;
  if (credits_ > MAXIMUM_NUMBER_OF_CREDITS) credits_ = MAXIMUM_NUMBER_OF_CREDITS;
}

void MachineState::setCurrentTime(unsigned long value) {
  currentTime_ = value;
}

void MachineState::setHighScore(unsigned long value) {
  highScore_ = value;
}

void MachineState::setNumberOfPlayers(byte value) {
  numberOfPlayers_ = value;
}

void MachineState::setScore(unsigned long value, byte player) {
  if (player == 0xFF) player = currentPlayer_;
  scores_[player] = value;
}

void MachineState::writeCoinToAudit(byte switchHit) {
  unsigned short coinAuditStartByte = 0;

  switch (switchHit) {
    case SW_COIN_3: coinAuditStartByte = BSOS_CHUTE_3_COINS_START_BYTE; break;
    case SW_COIN_2: coinAuditStartByte = BSOS_CHUTE_2_COINS_START_BYTE; break;
    case SW_COIN_1: coinAuditStartByte = BSOS_CHUTE_1_COINS_START_BYTE; break;
  }

  if (coinAuditStartByte) {
    BSOS_WriteULToEEProm(coinAuditStartByte, BSOS_ReadULFromEEProm(coinAuditStartByte) + 1);
  }
}
