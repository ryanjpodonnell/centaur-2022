#include "SharedVariables.h"

MachineState::MachineState(byte id) {
  currentPlayer_ = 0;
  machineStateChanged_ = true;
  machineStateId_ = id;
}

boolean MachineState::machineStateChanged() {
  return machineStateChanged_;
}

boolean MachineState::samePlayerShootsAgain() {
  return samePlayerShootsAgain_;
}

boolean MachineState::incrementNumberOfPlayers() {
  if (Credits < 1 && !FreePlayMode) return false;
  if (currentNumPlayers_ >= 4) return false;

  currentNumPlayers_ += 1;

  BSOS_SetDisplay(currentNumPlayers_ - 1, 0);
  BSOS_SetDisplayBlank(currentNumPlayers_ - 1, 0x30);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::resetNumberOfPlayers() {
  if (Credits < 1 && !FreePlayMode) return false;

  currentNumPlayers_ = 1;

  BSOS_SetDisplay(0, 0);
  BSOS_SetDisplayBlank(0, 0x30);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

byte MachineState::bonus() {
  return bonuses_[currentPlayer_];
}

byte MachineState::bonusMultiplier() {
  return bonusMultipliers_[currentPlayer_];
}

byte MachineState::currentBallInPlay() {
  return currentBallInPlay_;
}

byte MachineState::currentPlayer() {
  return currentPlayer_;
}

byte MachineState::GetmachineState() {
  return machineStateId_;
}

byte MachineState::numberOfPlayers() {
  return currentNumPlayers_;
}

byte MachineState::incrementCurrentPlayer() {
  currentPlayer_ += 1;

  if (currentPlayer_ >= currentNumPlayers_) {
    currentPlayer_ = 0;
    currentBallInPlay_ += 1;
  }

  if (currentBallInPlay_ > BALLS_PER_GAME) {
    for (int count = 0; count < currentNumPlayers_; count++) {
      BSOS_SetDisplay(count, scores_[count], true, 2);
    }

    return MACHINE_STATE_INIT_GAMEPLAY;
  } else {
    return MACHINE_STATE_INIT_NEW_BALL;
  }
}

byte MachineState::resetGame() {
  if (FreePlayMode) {
    return MACHINE_STATE_INIT_GAMEPLAY;
  } else if (Credits >= 1 && !FreePlayMode) {
    Credits -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
    BSOS_SetDisplayCredits(Credits);
    return MACHINE_STATE_INIT_GAMEPLAY;
  } else {
    return machineStateId_;
  }
}

int MachineState::initGamePlay() {
  if (DEBUG_MESSAGES) {
    Serial.write("Starting game\n\r");
  }

  BSOS_SetDisableFlippers(false);
  BSOS_EnableSolenoidStack();
  BSOS_TurnOffAllLamps();

  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, currentTime_ + 500);
  BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 10, currentTime_ + 500);
  BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 10, currentTime_ + 500);

  for (int count = 0; count < 4; count++) {
    bonusMultipliers_[count] = 1;
    bonuses_[count] = 0;
  }
  memset(scores_, 0, 4 * sizeof(unsigned long));

  samePlayerShootsAgain_ = false;
  currentBallInPlay_ = 1;
  currentNumPlayers_ = 1;
  currentPlayer_ = 0;
  g_displayHelper.showPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}

int MachineState::initNewBall(bool curStateChanged, byte playerNum, int ballNum) {
  if (curStateChanged) {
    ShowLamps(LAMP_COLLECTION_BONUS_ALL, true);

    BSOS_SetDisplayCredits(Credits, true);
    samePlayerShootsAgain_ = false;

    BSOS_SetDisplayBallInPlay(ballNum);
    BSOS_SetLampState(LAMP_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      BSOS_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    g_gameMode.setGameMode(GAME_MODE_SKILL_SHOT);

    ExtraBallCollected = false;

    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
      BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime_ + 600);
    }

    // Reset progress unless holdover awards
    bonuses_[currentPlayer_] = 0;
    bonusMultipliers_[currentPlayer_] = 1;

    scoreMultiplier_ = 1;
    ScoreAdditionAnimation = 0;
    ScoreAdditionAnimationStartTime = 0;
    BonusXAnimationStart = 0;
  }

  // We should only consider the ball initialized when
  // the ball is no longer triggering the SW_OUTHOLE
  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
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

void MachineState::awardExtraBall() {
  if (ExtraBallCollected) return;

  ExtraBallCollected = true;
  samePlayerShootsAgain_ = true;
  BSOS_SetLampState(LAMP_SHOOT_AGAIN, samePlayerShootsAgain_);
}

void MachineState::decreaseBonus(byte amountToSubtract) {
  bonuses_[currentPlayer_] -= amountToSubtract;
}

void MachineState::increaseBonus(byte amountToAdd) {
  bonuses_[currentPlayer_] += amountToAdd;
  if (bonuses_[currentPlayer_] >= MAX_DISPLAY_BONUS) {
    bonuses_[currentPlayer_] = MAX_DISPLAY_BONUS;
  }
}

void MachineState::increaseBonusMultiplier() {
  boolean soundPlayed = false;
  if (bonusMultipliers_[currentPlayer_] < 5) {
    bonusMultipliers_[currentPlayer_] += 1;
    BonusXAnimationStart = currentTime_;

    if (bonusMultipliers_[currentPlayer_] == 4) {
      bonusMultipliers_[currentPlayer_] += 1;
    }
  }
}

void MachineState::increaseCredits(boolean playSound, byte numToAdd) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
    BSOS_SetDisplayCredits(Credits);
    BSOS_SetCoinLockout(false);
  } else {
    BSOS_SetDisplayCredits(Credits);
    BSOS_SetCoinLockout(true);
  }
}

void MachineState::increaseScore(unsigned long amountToAdd) {
  scores_[currentPlayer_] += amountToAdd;
}


void MachineState::setBonus(byte value) {
  bonuses_[currentPlayer_] = value;
}

void MachineState::setCurrentTime(unsigned long value) {
  currentTime_ = value;
}

void MachineState::setLastTiltWarningTime(unsigned long value) {
  lastTiltWarningTime_ = value;
}

void MachineState::SetmachineState(byte id) {
  if (id != machineStateId_) {
    machineStateChanged_ = true;
  } else {
    machineStateChanged_ = false;
  }

  machineStateId_ = id;
}

void MachineState::setNumberOfPlayers(byte value) {
  currentNumPlayers_ = value;
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
