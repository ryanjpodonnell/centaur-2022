#include "SharedVariables.h"

MachineState::MachineState() {
  player1_                     = PlayerState(0);
  player2_                     = PlayerState(1);
  player3_                     = PlayerState(2);
  player4_                     = PlayerState(3);

  machineStateId_              = MACHINE_STATE_DEBUG;
  machineStateChanged_         = true;

  credits_                     = 0;
  currentBallInPlay_           = 0;
  currentPlayerNumber_         = 0;
  numberOfBallsInPlay_         = 1;
  numberOfPlayers_             = 0;
  numberOfTiltWarnings_        = 0;

  highScore_                   = 0;

  currentTime_                 = 0;
  ballEnteredTroughTime_       = 0;
  lastTiltWarningTime_         = 0;
  mostRecentSwitchHitTime_     = 0;

  resetMachineState();
}

boolean MachineState::allModesQualified() {
  return currentPlayer_->allModesQualified();
}

boolean MachineState::allowRightDropTargetProgress() {
  return currentPlayer_->allowRightDropTargetProgress();
}

boolean MachineState::anyModeQualified() {
  return currentPlayer_->anyModeQualified();
}

boolean MachineState::ballSaveActivated() {
  return ballSaveActivated_;
}

boolean MachineState::ballSaveUsed() {
  return ballSaveUsed_;
}

boolean MachineState::currentPlayerTilted() {
  return numberOfTiltWarnings_ > MAXIMUM_NUMBER_OF_TILT_WARNINGS;
}

boolean MachineState::guardianRolloversCompleted() {
  return currentPlayer_->guardianRolloversCompleted();
}

boolean MachineState::increaseNumberOfPlayers() {
  if (credits_ < 1 && !FREE_PLAY) return false;
  if (numberOfPlayers_ >= 4) return false;

  if (!FREE_PLAY) {
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

boolean MachineState::modeMultiplierQualified() {
  return currentPlayer_->modeMultiplierQualified();
}

boolean MachineState::orbsDropTargetsCompleted() {
  return currentPlayer_->orbsDropTargetsCompleted();
}

boolean MachineState::playfieldValidated() {
  return playfieldValidated_;
}

boolean MachineState::qualifyMode() {
  return currentPlayer_->qualifyMode();
}

boolean MachineState::qualifyModeMultiplier() {
  return currentPlayer_->qualifyModeMultiplier();
}

boolean MachineState::resetPlayers() {
  if (credits_ < 1 && !FREE_PLAY) return false;

  if (!FREE_PLAY) {
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

boolean MachineState::rightDropTargetsCompleted() {
  return currentPlayer_->rightDropTargetsCompleted();
}

boolean MachineState::samePlayerShootsAgain() {
  return samePlayerShootsAgain_;
}

boolean MachineState::topRolloversCompleted() {
  return currentPlayer_->topRolloversCompleted();
}

boolean MachineState::troughSwitchActivated() {
  return troughSwitchActivated_;
}

byte MachineState::bonus(byte player) {
  if (player == 0xFF) player = currentPlayerNumber();
  if (player == 0) return player1_.bonus();
  if (player == 1) return player2_.bonus();
  if (player == 2) return player3_.bonus();
  if (player == 3) return player4_.bonus();
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

byte MachineState::numberOfBallsInPlay() {
  return numberOfBallsInPlay_;
}

byte MachineState::numberOfPlayers() {
  return numberOfPlayers_;
}

byte MachineState::increaseCurrentPlayer() {
  currentPlayerNumber_ += 1;
  if (currentPlayerNumber_ == numberOfPlayers_) {
    currentPlayerNumber_ = 0;
    currentBallInPlay_ += 1;
  }

  if (currentBallInPlay_ > BALLS_PER_GAME) {
    numberOfPlayers_ = 0;
    return MACHINE_STATE_ATTRACT;
  } else {
    return MACHINE_STATE_INIT_NEW_BALL;
  }
}

byte MachineState::startQualifiedMode() {
  return currentPlayer_->startQualifiedMode();
}

int MachineState::initGamePlay() {
  if (DEBUG_MESSAGES) Serial.write("Initializing gameplay\n\r");

  currentBallInPlay_ = 1;
  currentPlayerNumber_ = 0;

  return MACHINE_STATE_INIT_NEW_BALL;
}

int MachineState::initNewBall(bool curStateChanged) {
  if (curStateChanged) {
    if (DEBUG_MESSAGES) Serial.write("Initializing new ball\n\r");

    resetMachineState();
    setCurrentPlayer(currentPlayerNumber_);
    setBonus(0);
    setBonusMultiplier(1);

    BSOS_EnableSolenoidStack();
    BSOS_SetDisableFlippers(false);
    BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 8, currentTime_ + 500);
    BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 8, currentTime_ + 500);
    BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 8, currentTime_ + 500);
    BSOS_SetDisplayBallInPlay(currentBallInPlay_);
    BSOS_SetDisplayCredits(credits_);

    g_lampsHelper.showLamp(LAMP_PLAYFIELD_GI, false, true);
    g_lampsHelper.showLamp(LAMP_RIGHT_THUMPER_BUMPER);
    g_lampsHelper.showLamp(LAMP_LEFT_THUMPER_BUMPER);
    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime_ + 600);
  }

  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }
}

int MachineState::machineState() {
  return machineStateId_;
}

unsigned long MachineState::currentBallSwitchHitCounter() {
  return currentBallSwitchHitCounter_;
}

unsigned long MachineState::ballEnteredTroughTime() {
  return ballEnteredTroughTime_;
}

unsigned long MachineState::currentBallFirstSwitchHitTime() {
  return currentBallFirstSwitchHitTime_;
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

unsigned long MachineState::mostRecentSwitchHitTime() {
  return mostRecentSwitchHitTime_;
}

unsigned long MachineState::score(byte player) {
  if (player == 0xFF) player = currentPlayerNumber();
  if (player == 0) return player1_.score();
  if (player == 1) return player2_.score();
  if (player == 2) return player3_.score();
  if (player == 3) return player4_.score();
}

void MachineState::setMachineState(int id) {
  if (id != machineStateId_) {
    machineStateChanged_ = true;
    machineStateId_ = id;
  } else {
    machineStateChanged_ = false;
  }
}

void MachineState::setMostRecentSwitchHitTime() {
  mostRecentSwitchHitTime_ = currentTime_;
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

void MachineState::decreaseModeMultiplier() {
  currentPlayer_->decreaseModeMultiplier();
}

void MachineState::decreaseNumberOfBallsInPlay() {
  if (numberOfBallsInPlay_ == 1) return;

  numberOfBallsInPlay_ -= 1;
}

void MachineState::flashOrbsDropTargetLamps() {
  currentPlayer_->flashOrbsDropTargetLamps();
}

void MachineState::flashRightDropTargetLamps() {
  currentPlayer_->flashRightDropTargetLamps();
}

void MachineState::hideAllPlayerLamps() {
  g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
  g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_RELEASE_ORBS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_ALL_ROLLOVERS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_CAPTIVE_ORBS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS);
}

void MachineState::increaseBonus(byte amountToAdd) {
  currentPlayer_->increaseBonus(amountToAdd);
}

void MachineState::increaseBonusMultiplier() {
  currentPlayer_->increaseBonusMultiplier();
}

void MachineState::increaseCredits(boolean playSound, byte numToAdd) {
  credits_ += numToAdd;
  if (credits_ > MAXIMUM_NUMBER_OF_CREDITS) credits_ = MAXIMUM_NUMBER_OF_CREDITS;

  BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
  BSOS_SetDisplayCredits(credits_);
}

void MachineState::increaseCurrentBallSwitchHitCounter() {
  currentBallSwitchHitCounter_ += 1;
}

void MachineState::increaseModeMultiplier() {
  currentPlayer_->increaseModeMultiplier();
}

void MachineState::increaseNumberOfBallsInPlay() {
  if (numberOfBallsInPlay_ == MAXIMUM_NUMBER_OF_BALLS_IN_PLAY) return;

  numberOfBallsInPlay_ += 1;
}

void MachineState::increaseScore(unsigned long amountToAdd) {
  currentPlayer_->increaseScore(amountToAdd);
}

void MachineState::readStoredParameters() {
  setHighScore(BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, DEFAULT_HIGH_SCORE));
  setCredits(BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE));
}

void MachineState::registerGuardianRollover(byte switchHit) {
  currentPlayer_->registerGuardianRollover(switchHit);
}

void MachineState::registerOrbsDropTarget(byte switchHit) {
  currentPlayer_->registerOrbsDropTarget(switchHit);
}

void MachineState::registerRightDropTarget(byte switchHit) {
  currentPlayer_->registerRightDropTarget(switchHit);
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

void MachineState::registerTopRollover(byte switchHit) {
  currentPlayer_->registerTopRollover(switchHit);
}

void MachineState::resetGuardianRollovers() {
  currentPlayer_->resetGuardianRollovers();
}

void MachineState::resetOrbsDropTargets(boolean activateSolenoid) {
  currentPlayer_->resetOrbsDropTargets(activateSolenoid);
}

void MachineState::resetRightDropTargets(boolean activateSolenoid) {
  currentPlayer_->resetRightDropTargets(activateSolenoid);
}

void MachineState::resetTopRollovers() {
  currentPlayer_->resetTopRollovers();
}

void MachineState::rotatePlayerLamps() {
  currentPlayer_->rotatePlayerLamps();
}

void MachineState::rotateQualifiedMode() {
  currentPlayer_->rotateQualifiedMode();
}

void MachineState::setBallEnteredTroughTime() {
  ballEnteredTroughTime_ = currentTime_;
}

void MachineState::setBallSaveActivated() {
  ballSaveActivated_ = true;
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

void MachineState::setCurrentBallFirstSwitchHitTime() {
  currentBallFirstSwitchHitTime_ = currentTime_;
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

void MachineState::setPlayfieldValidated() {
  playfieldValidated_ = true;
}

void MachineState::setScore(unsigned long value, byte player) {
  if (player == 0xFF) player = currentPlayerNumber();
  if (player == 0) player1_.setScore(value);
  if (player == 1) player2_.setScore(value);
  if (player == 2) player3_.setScore(value);
  if (player == 3) player4_.setScore(value);
}

void MachineState::setTroughSwitchActivated(boolean value) {
  troughSwitchActivated_ = value;
}

void MachineState::showAllPlayerLamps() {
  updateCaptiveOrbsLamps();
  updateGuardianRolloverLamps();
  updateModeMultiplierLamps();
  updateTopRolloverLamps();

  updateRightDropTargetResetLamp();
  updateRightDropTargetSpotLamp();
  if (modeMultiplierQualified()) {
    flashRightDropTargetLamps();
  } else {
    updateRightDropTargetLamps();
  }

  updateRightOrbsReleaseLamp();
  if (anyModeQualified()) {
    flashOrbsDropTargetLamps();
  } else {
    updateOrbsDropTargetLamps();
  }
}

void MachineState::updateBonusLamps() {
  currentPlayer_->updateBonusLamps();
}

void MachineState::updateCaptiveOrbsLamps() {
  currentPlayer_->updateCaptiveOrbsLamps();
}

void MachineState::updateGuardianRolloverLamps() {
  currentPlayer_->updateGuardianRolloverLamps();
}

void MachineState::updateModeMultiplierLamps() {
  currentPlayer_->updateModeMultiplierLamps();
}

void MachineState::updateOrbsDropTargetLamps() {
  currentPlayer_->updateOrbsDropTargetLamps();
}

void MachineState::updatePlayerScore(boolean flashCurrent, boolean dashCurrent) {
  currentPlayer_->updatePlayerScore(flashCurrent, dashCurrent);
}

void MachineState::updateRightDropTargetLamps() {
  currentPlayer_->updateRightDropTargetLamps();
}

void MachineState::updateRightDropTargetResetLamp() {
  currentPlayer_->updateRightDropTargetResetLamp();
}

void MachineState::updateRightDropTargetSpotLamp() {
  currentPlayer_->updateRightDropTargetSpotLamp();
}

void MachineState::updateRightOrbsReleaseLamp() {
  currentPlayer_->updateRightOrbsReleaseLamp();
}

void MachineState::updateSelectedMode() {
  currentPlayer_->updateSelectedMode();
}

void MachineState::updateTopRolloverLamps() {
  currentPlayer_->updateTopRolloverLamps();
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


/*********************************************************************
    Private
*********************************************************************/
void MachineState::resetMachineState() {
  ballSaveActivated_           = false;
  ballSaveUsed_                = false;
  extraBallCollected_          = false;
  playfieldValidated_          = false;
  samePlayerShootsAgain_       = false;
  troughSwitchActivated_       = false;

  currentBallFirstSwitchHitTime_ = 0;
  currentBallSwitchHitCounter_   = 0;

  scoreMultiplier_               = 1;
}
