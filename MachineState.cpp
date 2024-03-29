#include "SharedVariables.h"

MachineState::MachineState() {
  player1_                     = PlayerState(0);
  player2_                     = PlayerState(1);
  player3_                     = PlayerState(2);
  player4_                     = PlayerState(3);

  machineStateId_              = MACHINE_STATE_DEBUG;
  machineStateChanged_         = true;

  coins_                       = 0;
  credits_                     = 0;
  currentBallInPlay_           = 0;
  currentPlayerNumber_         = 0;
  numberOfBallsInPlay_         = 1;
  numberOfPlayers_             = 0;
  highScore_                   = 0;

  activationTime_              = 0;
  currentTime_                 = 0;
  lastTiltWarningTime_         = 0;
  mostRecentRolloverTime_      = 0;
  mostRecentSwitchHitTime_     = 0;
  mostRecentSwitchHit_         = 0xFF;

  resetMachineState();
}

boolean MachineState::allowRightDropTargetProgress() {
  return currentPlayer_->allowRightDropTargetProgress();
}

boolean MachineState::anyModeQualified() {
  return currentPlayer_->anyModeQualified();
}

boolean MachineState::centaurDestroyed() {
  return currentPlayer_->centaurDestroyed();
}

boolean MachineState::currentPlayerTilted() {
  return numberOfTiltWarnings_ > MAXIMUM_NUMBER_OF_TILT_WARNINGS;
}

boolean MachineState::destroyCentaurQualified() {
  return currentPlayer_->destroyCentaurQualified();
}

boolean MachineState::firstBallActive() {
  return currentPlayerNumber() == 0 && currentBallInPlay() == 1;
}

boolean MachineState::guardianRolloversCompleted() {
  return currentPlayer_->guardianRolloversCompleted();
}

boolean MachineState::increaseNumberOfPlayers() {
  if (credits_ < 1 && !freePlay_) return false;
  if (numberOfPlayers_ >= 4) return false;

  if (!freePlay_) {
    credits_ -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
    BSOS_SetDisplayCredits(credits_);
  }

  setScore(0, numberOfPlayers_);
  numberOfPlayers_ += 1;

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  g_displayHelper.showPlayerScores(0xFF);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::machineStateChanged() {
  return machineStateChanged_;
}

boolean MachineState::orbsDropTargetsAllStanding() {
  return currentPlayer_->orbsDropTargetsAllStanding();
}

boolean MachineState::orbsDropTargetsCompleted() {
  return currentPlayer_->orbsDropTargetsCompleted();
}

boolean MachineState::orbsDropTargetsCompletedInOrder() {
  return currentPlayer_->orbsDropTargetsCompletedInOrder();
}

boolean MachineState::playfieldValidated() {
  return playfieldValidated_;
}

boolean MachineState::resetPlayers() {
  if (credits_ < 1 && !freePlay_) return false;
  if (numberOfPlayers_ >= 4) return false;

  if (!freePlay_) {
    credits_ -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
  }

  player1_.resetPlayerState();
  player2_.resetPlayerState();
  player3_.resetPlayerState();
  player4_.resetPlayerState();
  numberOfPlayers_ = 1;
  g_displayHelper.showPlayerScores(0xFF);
  BSOS_SetDisplayCredits(credits_);

  BSOS_WriteULToEEProm(BSOS_TOTAL_REPLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_REPLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::rightDropTargetsAllStanding() {
  return currentPlayer_->rightDropTargetsAllStanding();
}

boolean MachineState::rightDropTargetsCompleted() {
  return currentPlayer_->rightDropTargetsCompleted();
}

boolean MachineState::rightDropTargetsCompletedInOrder() {
  return currentPlayer_->rightDropTargetsCompletedInOrder();
}

boolean MachineState::rightDropTargetsResetQualified() {
  return currentPlayer_->rightDropTargetsResetQualified();
}

boolean MachineState::samePlayerShootsAgain() {
  return samePlayerShootsAgain_;
}

boolean MachineState::scoreIncreasing() {
  return currentPlayer_->scoreIncreasing();
}

boolean MachineState::topRolloversCompleted() {
  return currentPlayer_->topRolloversCompleted();
}

byte MachineState::activeOrbsDropTarget() {
  return currentPlayer_->activeOrbsDropTarget();
}

byte MachineState::activeRightDropTarget() {
  return currentPlayer_->activeRightDropTarget();
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

byte MachineState::increaseCurrentPlayer() {
  currentPlayerNumber_ += 1;
  if (currentPlayerNumber_ == numberOfPlayers_) {
    currentPlayerNumber_ = 0;
    currentBallInPlay_ += 1;
  }

  if (currentBallInPlay_ > BALLS_PER_GAME) {
    return MACHINE_STATE_MATCH_MODE;
  } else {
    return MACHINE_STATE_INIT_NEW_BALL;
  }
}

byte MachineState::manageCreditButton(byte state) {
  if (DEBUG_MESSAGES) Serial.write("Start game button pressed\n\r");

  if (currentBallInPlay() == 0 && resettingPlayers_) {
    increaseNumberOfPlayers();
    return state;
  } else if (currentBallInPlay() == 1 && increaseNumberOfPlayers()) {
    return state;
  } else if (resetPlayers()) {
    currentBallInPlay_ = 0;
    resettingPlayers_ = true;
    return MACHINE_STATE_RESTART_GAME;
  }

  return state;
}

byte MachineState::mostRecentSwitchHit() {
  return mostRecentSwitchHit_;
}

byte MachineState::numberOfBallsInPlay() {
  return numberOfBallsInPlay_;
}

byte MachineState::numberOfBallsInTrough() {
  if (!BSOS_ReadSingleSwitchState(SW_1ST_BALL_TROUGH) && !BSOS_ReadSingleSwitchState(SW_4TH_BALL_TROUGH))      return 0;
  else if (BSOS_ReadSingleSwitchState(SW_1ST_BALL_TROUGH)  && !BSOS_ReadSingleSwitchState(SW_4TH_BALL_TROUGH)) return 1;
  else if (BSOS_ReadSingleSwitchState(SW_1ST_BALL_TROUGH)  && BSOS_ReadSingleSwitchState(SW_4TH_BALL_TROUGH))  return 2;
}

byte MachineState::numberOfPlayers() {
  return numberOfPlayers_;
}

byte MachineState::qualifiedScoreMultiplier() {
  return currentPlayer_->qualifiedScoreMultiplier();
}

byte MachineState::queensChamberBonusValue() {
  return currentPlayer_->queensChamberBonusValue();
}

byte MachineState::startQualifiedMode() {
  return currentPlayer_->startQualifiedMode();
}

int MachineState::initGamePlay() {
  if (DEBUG_MESSAGES) Serial.write("Initializing gameplay\n\r");

  currentBallInPlay_ = 1;
  currentPlayerNumber_ = 0;
  resettingPlayers_ = false;

  return MACHINE_STATE_INIT_NEW_BALL;
}

int MachineState::initNewBall(bool curStateChanged) {

  if (curStateChanged) {
    if (DEBUG_MESSAGES) Serial.write("Initializing new ball\n\r");
    activationTime_ = 0;

    resetMachineState();
    setCurrentPlayer(currentPlayerNumber_);
    setBonus(0);
    setBonusMultiplier(1);
    resetTopRollovers();
    unqualifyMode();

    BSOS_EnableSolenoidStack();
    BSOS_SetDisableFlippers(false);

    activationTime_ = currentTime_ + 500;
    activationTime_ = resetInlineDropTargets(true, true,  activationTime_);
    activationTime_ = resetOrbsDropTargets  (true, true,  activationTime_);
    activationTime_ = resetRightDropTargets (true, false, activationTime_);
    activationTime_ = dropRightDropTargets(activationTime_);

    BSOS_SetDisplayBallInPlay(currentBallInPlay_);
    BSOS_SetDisplayCredits(credits_);

    g_lampsHelper.hideAllLamps();
    g_lampsHelper.showLamp(LAMP_BALL_IN_PLAY);
    g_lampsHelper.showLamps(LAMP_COLLECTION_GENERAL_ILLUMINATION);
    g_gameMode.setGameMode(GAME_MODE_INITIALIZE);

    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
      BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, SOL_OUTHOLE_KICKER_STRENGTH, activationTime_);
      activationTime_ += 1500;
    }
  }

  if (currentTime_ < activationTime_) {
    return MACHINE_STATE_INIT_NEW_BALL;
  }

  if (currentTime_ >= activationTime_ && BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, SOL_OUTHOLE_KICKER_STRENGTH, currentTime_);
    activationTime_ += 1500;

    return MACHINE_STATE_INIT_NEW_BALL;
  }

  numberOfBallsInPlay_ = 1;
  return MACHINE_STATE_NORMAL_GAMEPLAY;
}

int MachineState::machineState() {
  return machineStateId_;
}

unsigned long MachineState::currentTime() {
  return currentTime_;
}

unsigned long MachineState::dropRightDropTargets(unsigned long activationTime) {
  return currentPlayer_->dropRightDropTargets(activationTime);
}

unsigned long MachineState::highScore() {
  return highScore_;
}

unsigned long MachineState::lastTiltWarningTime() {
  return lastTiltWarningTime_;
}

unsigned long MachineState::mostRecentRolloverTime() {
  return mostRecentRolloverTime_;
}

unsigned long MachineState::mostRecentSwitchHitTime() {
  return mostRecentSwitchHitTime_;
}

unsigned long MachineState::queensChamberHurryUpValue() {
  return currentPlayer_->queensChamberHurryUpValue();
}

unsigned long MachineState::queensChamberScoreValue() {
  return currentPlayer_->queensChamberScoreValue();
}

unsigned long MachineState::resetInlineDropTargets(boolean activateSolenoid, boolean resetProgress, unsigned long activationTime) {
  if (activateSolenoid) BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, SOL_INLINE_DROPS_RESET_STRENGTH, activationTime);
  if (resetProgress)    currentPlayer_->resetInlineDropTargets();

  return activationTime + SOLENOID_BUFFER_TIME;
}

unsigned long MachineState::resetOrbsDropTargets(boolean activateSolenoid, boolean resetProgress, unsigned long activationTime) {
  if (activateSolenoid) BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, SOL_ORBS_DROPS_RESET_STRENGTH, activationTime);
  if (resetProgress)    currentPlayer_->resetOrbsDropTargets();

  return activationTime + SOLENOID_BUFFER_TIME;
}

unsigned long MachineState::resetRightDropTargets(boolean activateSolenoid, boolean resetProgress, unsigned long activationTime) {
  if (activateSolenoid) BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, SOL_RIGHT_DROPS_RESET_STRENGTH, activationTime);
  if (resetProgress)    currentPlayer_->resetRightDropTargets();

  return activationTime + SOLENOID_BUFFER_TIME;
}

unsigned long MachineState::rightDropTargetsScoreValue() {
  return currentPlayer_->rightDropTargetsScoreValue();
}

unsigned long MachineState::score(byte player) {
  if (player == 0xFF) player = currentPlayerNumber();
  if (player == 0) return player1_.score();
  if (player == 1) return player2_.score();
  if (player == 2) return player3_.score();
  if (player == 3) return player4_.score();
}

void MachineState::awardExtraBall() {
  if (extraBallCollected_) return;

  extraBallCollected_ = true;
  samePlayerShootsAgain_ = true;
  g_lampsHelper.showLamp(LAMP_SHOOT_AGAIN, false);
}

void MachineState::completeSelectedMode() {
  currentPlayer_->completeSelectedMode();
}

void MachineState::decreaseBonus() {
  currentPlayer_->decreaseBonus();
}

void MachineState::decreaseNumberOfBallsInPlay() {
  if (numberOfBallsInPlay_ == 1) return;

  numberOfBallsInPlay_ -= 1;
}

void MachineState::flashOrbsDropTargetsLamps() {
  currentPlayer_->flashOrbsDropTargetsLamps();
}

void MachineState::flashRightDropTargetsLamps() {
  currentPlayer_->flashRightDropTargetsLamps();
}

void MachineState::hideAllPlayerLamps() {
  g_lampsHelper.hideLamp(LAMP_ORBS_50K);
  g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
  g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_RELEASE_ORBS);
  g_lampsHelper.hideLamp(LAMP_SPOT_1_THROUGH_4);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_ALL_ROLLOVERS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_CAPTIVE_ORBS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS);
}

void MachineState::increaseBonus(byte amountToAdd, boolean checkQueensChamberMultiplier) {
  byte multiplier = 1;
  if (checkQueensChamberMultiplier &&
      g_machineState.currentTime() - g_machineState.mostRecentRolloverTime() < 1500) {
    multiplier = 2;
  }

  currentPlayer_->increaseBonus(amountToAdd * multiplier);
}

void MachineState::increaseBonusMultiplier() {
  currentPlayer_->increaseBonusMultiplier();
}

void MachineState::increaseCredits() {
  credits_ += 1;
  if (credits_ > MAXIMUM_NUMBER_OF_CREDITS) credits_ = MAXIMUM_NUMBER_OF_CREDITS;

  BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, credits_);
  BSOS_SetDisplayCredits(credits_);
}

void MachineState::increaseNumberOfBallsInPlay() {
  if (numberOfBallsInPlay_ == MAXIMUM_NUMBER_OF_BALLS_IN_PLAY) return;

  numberOfBallsInPlay_ += 1;
}

void MachineState::increaseQualifiedScoreMultiplier() {
  currentPlayer_->increaseQualifiedScoreMultiplier();
}

void MachineState::increaseScore(unsigned long amountToAdd, boolean checkQueensChamberMultiplier) {
  byte multiplier = 1;
  if (checkQueensChamberMultiplier &&
      g_machineState.currentTime() - g_machineState.mostRecentRolloverTime() < 1500) {
    multiplier = 2;
  }

  currentPlayer_->increaseScore(amountToAdd * multiplier);
}

void MachineState::launchBallIntoPlay(int lag) {
  BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE,           SOL_BALL_RELEASE_STRENGTH,           currentTime_ + lag);
  BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, SOL_BALL_KICK_TO_PLAYFIELD_STRENGTH, currentTime_ + 1000 + lag);

  g_gameMode.setBallSaveEndTime(g_machineState.currentTime() + ((unsigned long)MULTIBALL_SAVE_NUMBER_OF_SECONDS * 1000));
}

void MachineState::manageCoinDrop(byte switchHit) {
  g_soundHelper.playSound(SOUND_ENERGIZE_ME);
  writeCoinToAudit(switchHit);

  coins_ += 1;
  if (coins_ == numberOfCoinsPerCredit_) {
    coins_ = 0;
    increaseCredits();
  }
}

void MachineState::manageInlineTargetScoring(byte switchHit) {
  currentPlayer_->manageInlineTargetScoring(switchHit);
}

void MachineState::manageOrbsDropTargetScoring(byte switchHit) {
  currentPlayer_->manageOrbsDropTargetScoring(switchHit);
}

void MachineState::manageRightDropTargetScoring(byte switchHit) {
  currentPlayer_->manageRightDropTargetScoring(switchHit);
}

void MachineState::overridePlayerScore(unsigned long value) {
  currentPlayer_->overridePlayerScore(value);
}

void MachineState::qualifyMode() {
  return currentPlayer_->qualifyMode();
}

void MachineState::qualifyRightDropTargetsReset() {
  return currentPlayer_->qualifyRightDropTargetsReset();
}

void MachineState::readStoredParameters() {
  setCredits(BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE));
  setHighScore(BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, DEFAULT_HIGH_SCORE));
  setScore(BSOS_ReadULFromEEProm(BSOS_PLAYER_1_SCORE_START_BYTE, 0), 0);
  setScore(BSOS_ReadULFromEEProm(BSOS_PLAYER_2_SCORE_START_BYTE, 0), 1);
  setScore(BSOS_ReadULFromEEProm(BSOS_PLAYER_3_SCORE_START_BYTE, 0), 2);
  setScore(BSOS_ReadULFromEEProm(BSOS_PLAYER_4_SCORE_START_BYTE, 0), 3);

  numberOfCoinsPerCredit_ = BSOS_ReadByteFromEEProm(BSOS_FREE_PLAY_EEPROM_START_BYTE);
  freePlay_ = numberOfCoinsPerCredit_ == 0 ? true : false;
}

void MachineState::registerGuardianRollover(byte switchHit) {
  currentPlayer_->registerGuardianRollover(switchHit);
}

void MachineState::registerInlineDropTarget(byte switchHit) {
  currentPlayer_->registerInlineDropTarget(switchHit);
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

    if (numberOfTiltWarnings_ == 1) {
      g_lampsHelper.showLamp(LAMP_WARNING);
      g_lampsHelper.showLamp(LAMP_CREDIT_INDICATOR);
    } else if (numberOfTiltWarnings_ == 2) {
      g_lampsHelper.showLamp(LAMP_WARNING, true);
      g_lampsHelper.showLamp(LAMP_CREDIT_INDICATOR, true);
    }

    if (!currentPlayerTilted()) {
      g_soundHelper.playSoundWithoutInterruptions(SOUND_NO_CLASS_HUMAN);
    } else {
      if (DEBUG_MESSAGES) Serial.write("Ball Tilted\n\r");

      g_lampsHelper.hideAllLamps();
      g_lampsHelper.showLamp(LAMP_BALL_IN_PLAY);
      g_lampsHelper.showLamp(LAMP_TILT);

      BSOS_DisableSolenoidStack();
      BSOS_SetDisableFlippers(true);

      g_soundHelper.stopAudio();
      g_soundHelper.playSoundWithoutInterruptions(SOUND_POWERING_DOWN);
      setBonus(0);
    }
  }
}

void MachineState::registerTopRollover(byte switchHit) {
  currentPlayer_->registerTopRollover(switchHit);
}

void MachineState::resetCurrentBallInPlay() {
  currentBallInPlay_ = 0;
}

void MachineState::resetGuardianRollovers() {
  currentPlayer_->resetGuardianRollovers();
}

void MachineState::resetTiltWarnings() {
  numberOfTiltWarnings_ = 0;
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

void MachineState::setBonus(byte value) {
  currentPlayer_->setBonus(value);
}

void MachineState::setBonusMultiplier(byte value) {
  currentPlayer_->setBonusMultiplier(value);
}

void MachineState::setCentaurDestoyed(boolean value) {
  currentPlayer_->setCentaurDestoyed(value);
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

void MachineState::setMachineState(int id) {
  if (id != machineStateId_) {
    machineStateChanged_ = true;
    machineStateId_ = id;
  } else {
    machineStateChanged_ = false;
  }
}

void MachineState::setMostRecentSwitchHit(byte value) {
  mostRecentSwitchHit_ = value;
}

void MachineState::setMostRecentRolloverTime() {
  mostRecentRolloverTime_ = currentTime_;
}

void MachineState::setMostRecentSwitchHitTime() {
  mostRecentSwitchHitTime_ = currentTime_;
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

void MachineState::showAllPlayerLamps() {
  updateBonusLamps();
  updateCaptiveOrbsLamps();
  updateGuardianRolloverLamps();
  updateOrbs50KLamp();
  updateOrbsDropTargetsLamps();
  updateQueensChamberLamps();
  updateRightDropTargetsLamps();
  updateRightDropTargetsResetLamp();
  updateRightDropTargetsSpotLamp();
  updateRightOrbsReleaseLamp();
  updateScoreMultiplierLamps();
  updateTopRolloverLamps();
}

void MachineState::spotRightDropTarget() {
  return currentPlayer_->spotRightDropTarget();
}

void MachineState::unqualifyAllModes() {
  return currentPlayer_->unqualifyAllModes();
}

void MachineState::unqualifyMode() {
  return currentPlayer_->unqualifyMode();
}

void MachineState::unqualifyRightDropTargetsReset() {
  return currentPlayer_->unqualifyRightDropTargetsReset();
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

void MachineState::updateOrbs50KLamp() {
  currentPlayer_->updateOrbs50KLamp();
}

void MachineState::updateOrbsDropTargetsLamps() {
  currentPlayer_->updateOrbsDropTargetsLamps();
}

void MachineState::updatePlayerScore(boolean flashCurrent, boolean dashCurrent) {
  if (g_gameMode.gameMode() == GAME_MODE_UNSTRUCTURED_PLAY && g_unstructuredPlay.hurryUpActivated()) currentPlayer_->overridePlayerScore(g_unstructuredPlay.hurryUpValue());
  if (g_gameMode.gameMode() == GAME_MODE_ORBS_1)                                                     currentPlayer_->overridePlayerScore(g_orbMode1.jackpotValue());
  if (g_gameMode.gameMode() == GAME_MODE_ORBS_2)                                                     currentPlayer_->overridePlayerScore(g_orbMode2.jackpotValue());
  if (g_gameMode.gameMode() == GAME_MODE_ORBS_3)                                                     currentPlayer_->overridePlayerScore(g_orbMode3.jackpotValue());
  if (g_gameMode.gameMode() == GAME_MODE_ORBS_4)                                                     currentPlayer_->overridePlayerScore(g_orbMode4.jackpotValue());
  if (g_gameMode.gameMode() == GAME_MODE_DESTROY_CENTAUR)                                            currentPlayer_->overridePlayerScore(g_destroyCentaur.jackpotValue());

  currentPlayer_->updatePlayerScore(flashCurrent, dashCurrent);
}

void MachineState::updateQueensChamberLamps() {
  currentPlayer_->updateQueensChamberLamps();
}

void MachineState::updateRightDropTargetsLamps() {
  currentPlayer_->updateRightDropTargetsLamps();
}

void MachineState::updateRightDropTargetsResetLamp() {
  currentPlayer_->updateRightDropTargetsResetLamp();
}

void MachineState::updateRightDropTargetsSpotLamp() {
  currentPlayer_->updateRightDropTargetsSpotLamp();
}

void MachineState::updateRightOrbsReleaseLamp() {
  currentPlayer_->updateRightOrbsReleaseLamp();
}

void MachineState::updateScoreMultiplierLamps() {
  currentPlayer_->updateScoreMultiplierLamps();
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
  extraBallCollected_    = false;
  playfieldValidated_    = false;
  samePlayerShootsAgain_ = false;
  numberOfTiltWarnings_  = 0;
}
