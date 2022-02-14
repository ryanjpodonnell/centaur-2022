#include "SharedVariables.h"

MachineState::MachineState(byte id) {
  player1_               = PlayerState();
  player2_               = PlayerState();
  player3_               = PlayerState();
  player4_               = PlayerState();
  credits_               = 0;
  currentBallInPlay_     = 0;
  currentPlayerNumber_   = 0;
  extraBallCollected_    = false;
  freePlayMode_          = true;
  highScore_             = 0;
  lastTiltWarningTime_   = 0;
  lastScoreChangeTime_   = 0;
  machineStateChanged_   = true;
  machineStateId_        = id;
  numberOfPlayers_       = 0;
  numberOfTiltWarnings_  = 0;
  samePlayerShootsAgain_ = false;
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

  setScore(0, numberOfPlayers_);
  numberOfPlayers_ += 1;
  g_displayHelper.showPlayerScores(0xFF);

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

  player1_.resetPlayerState();
  player2_.resetPlayerState();
  player3_.resetPlayerState();
  player4_.resetPlayerState();
  numberOfPlayers_ = 1;
  g_displayHelper.showPlayerScores(0xFF);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::samePlayerShootsAgain() {
  return samePlayerShootsAgain_;
}

byte MachineState::bonus() {
  return currentPlayer_->bonus();
}

byte MachineState::bonusMultiplier() {
  return currentPlayer_->bonusMultiplier();
}

byte MachineState::credits() {
  return credits_;
}

byte MachineState::currentBallInPlay() {
  return currentBallInPlay_;
}

byte MachineState::currentPlayerNumber() {
  return currentPlayerNumber_;
}

byte MachineState::numberOfPlayers() {
  return numberOfPlayers_;
}

byte MachineState::incrementCurrentPlayer() {
  currentPlayerNumber_ += 1;

  if (currentPlayerNumber_ == numberOfPlayers_) {
    currentPlayerNumber_ = 0;
    currentBallInPlay_ += 1;
  }

  setCurrentPlayer(currentPlayerNumber_);

  if (currentBallInPlay_ > BALLS_PER_GAME) {
    return MACHINE_STATE_ATTRACT;
  } else {
    return MACHINE_STATE_INIT_NEW_BALL;
  }
}

int MachineState::initGamePlay() {
  if (DEBUG_MESSAGES) Serial.write("Initializing gameplay\n\r");

  currentBallInPlay_ = 1;
  setCurrentPlayer(0);

  return MACHINE_STATE_INIT_NEW_BALL;
}

int MachineState::initNewBall(bool curStateChanged) {
  if (curStateChanged) {
    if (DEBUG_MESSAGES) Serial.write("Initializing new ball\n\r");
    ballSaveUsed_          = false;
    extraBallCollected_    = false;
    samePlayerShootsAgain_ = false;
    scoreMultiplier_       = 1;

    setBonus(0);
    setBonusMultiplier(1);

    BSOS_EnableSolenoidStack();
    BSOS_SetDisableFlippers(false);
    BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, currentTime_ + 500);
    BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 10, currentTime_ + 500);
    BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 10, currentTime_ + 500);
    BSOS_SetDisplayBallInPlay(currentBallInPlay_);

    g_lampsHelper.showLamp(LAMP_PLAYFIELD_GI, false, true);
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

unsigned long MachineState::currentTime() {
  return currentTime_;
}

unsigned long MachineState::highScore() {
  return highScore_;
}

unsigned long MachineState::lastTiltWarningTime() {
  return lastTiltWarningTime_;
}

unsigned long MachineState::score(byte player) {
  if (player == 0xFF) player = currentPlayerNumber();
  if (player == 0) return player1_.score();
  if (player == 1) return player2_.score();
  if (player == 2) return player3_.score();
  if (player == 3) return player4_.score();
}

unsigned long MachineState::lastScoreChangeTime() {
  return lastScoreChangeTime_;
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
  currentPlayer_->decreaseBonus(amountToSubtract);
}

void MachineState::increaseBonus(byte amountToAdd) {
  currentPlayer_->increaseBonus(amountToAdd);
}

void MachineState::increaseCredits(boolean playSound, byte numToAdd) {
  credits_ += numToAdd;
  if (credits_ > MAXIMUM_NUMBER_OF_CREDITS) credits_ = MAXIMUM_NUMBER_OF_CREDITS;

  BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
  BSOS_SetDisplayCredits(credits_);
}

void MachineState::increaseScore(unsigned long amountToAdd) {
  lastScoreChangeTime_ = currentTime_;
  currentPlayer_->increaseScore(amountToAdd);
}

void MachineState::readStoredParameters() {
  setHighScore(BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, DEFAULT_HIGH_SCORE));
  setCredits(BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE));
}

void MachineState::registerGuardianRollover(byte switchHit) {
  currentPlayer_->registerGuardianRollover(switchHit);
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

void MachineState::rotatePlayerLamps() {
  currentPlayer_->rotatePlayerLamps();
}

void MachineState::setBallSaveUsed(byte value) {
  ballSaveUsed_ = value;
}

void MachineState::setBonus(byte value) {
  currentPlayer_->setBonus(value);
}

void MachineState::setBonusMultiplier(byte value) {
  currentPlayer_->setBonusMultiplier(value);
}

void MachineState::setCredits(byte value) {
  credits_ = value;
  if (credits_ > MAXIMUM_NUMBER_OF_CREDITS) credits_ = MAXIMUM_NUMBER_OF_CREDITS;
}

void MachineState::setCurrentPlayer(byte value) {
  if (value == 0) currentPlayer_ = &player1_;
  if (value == 1) currentPlayer_ = &player2_;
  if (value == 2) currentPlayer_ = &player3_;
  if (value == 3) currentPlayer_ = &player4_;
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
  if (player == 0xFF) player = currentPlayerNumber();
  if (player == 0) player1_.setScore(value);
  if (player == 1) player2_.setScore(value);
  if (player == 2) player3_.setScore(value);
  if (player == 3) player4_.setScore(value);
}

void MachineState::showPlayerLamps() {
  currentPlayer_->showPlayerLamps();
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
