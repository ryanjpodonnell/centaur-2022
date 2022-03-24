#include "SharedVariables.h"

GameMode::GameMode() {
  ballTimeInTrough_   = 0;
  firstSwitchHitTime_ = 0;
  gameModeChanged_    = true;
  savingBall_         = false;
  scoreIncreased_     = false;
  gameModeId_         = GAME_MODE_INITIALIZE;
}

boolean GameMode::scoreIncreased() {
  return scoreIncreased_;
}

int GameMode::run(boolean curStateChanged) {
  if (curStateChanged) handleNewMode();
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  handlePlayerScore();
  handlePlayerBonusLamps();
  if (!ballSaveLampActive()) g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);

  if (g_machineState.currentPlayerTilted()) {
    returnState = manageTilt();
  } else {
    returnState = manageGameModes();
  }

  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    returnState       = manageBallInTrough();
  } else {
    ballTimeInTrough_ = 0;
    savingBall_       = false;
  }

  return returnState;
}

unsigned long GameMode::firstSwitchHitTime() {
  return firstSwitchHitTime_;
}

unsigned long GameMode::mostRecentSwitchHitTime() {
  return mostRecentSwitchHitTime_;
}

void GameMode::setFirstSwitchHitTime(unsigned long value) {
  firstSwitchHitTime_ = value;
}

void GameMode::setMostRecentSwitchHitTime(unsigned long value) {
  mostRecentSwitchHitTime_ = value;
}

void GameMode::setGameMode(byte id) {
  if (id != gameModeId_) {
    gameModeChanged_ = true;
    gameModeId_ = id;
  } else {
    gameModeChanged_ = false;
  }
}

void GameMode::setScoreIncreased(boolean value) {
  scoreIncreased_ = value;
}


/*********************************************************************
    Private
*********************************************************************/
boolean GameMode::ballSaveActive() {
  if (g_machineState.currentPlayerTilted()) return false;
  if (!firstSwitchHitTime_) return true;

  return (mostRecentSwitchHitTime_ - firstSwitchHitTime_) < ((unsigned long)BALL_SAVE_NUMBER_OF_SECONDS * 1000);
}

boolean GameMode::ballSaveLampActive() {
  if (g_machineState.currentPlayerTilted()) return false;
  if (!firstSwitchHitTime_) return true;

  return (g_machineState.currentTime() - firstSwitchHitTime_) < ((unsigned long)BALL_SAVE_NUMBER_OF_SECONDS * 1000);
}

int GameMode::manageBallInTrough() {
  if (ballTimeInTrough_ == 0) ballTimeInTrough_ = g_machineState.currentTime();

  if ((g_machineState.currentTime() - ballTimeInTrough_) <= 500) return MACHINE_STATE_NORMAL_GAMEPLAY;
  if (savingBall_) return MACHINE_STATE_NORMAL_GAMEPLAY;

  if (!firstSwitchHitTime_ && !g_machineState.currentPlayerTilted()) {
    // no switches hit. return ball to shooter lane
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, g_machineState.currentTime() + 100);

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else if (!g_machineState.ballSaveUsed() && ballSaveActive()) {
    // ball save active. return ball to shooter lane
    if (DEBUG_MESSAGES) Serial.write("Ball Saved\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE, 4, g_machineState.currentTime() + 100);
    BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, 6, g_machineState.currentTime() + 1000);
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, g_machineState.currentTime() + 1000);

    g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);
    g_machineState.setBallSaveUsed(true);
    savingBall_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else if (g_machineState.numberOfBallsInPlay() > 1) {
    if (DEBUG_MESSAGES) Serial.write("Multiball Drained\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, g_machineState.currentTime() + 1000);
    g_machineState.decreaseNumberOfBallsInPlay();
    g_machineState.decreaseModeMultiplier();
    g_machineState.updateModeMultiplierLamps();
    savingBall_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else {
    // end ball
    setGameMode(GAME_MODE_INITIALIZE);
    firstSwitchHitTime_ = 0;

    return MACHINE_STATE_COUNTDOWN_BONUS;
  }
}

int GameMode::manageGameModes() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  boolean executedSwitchStack = false;

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    if (DEBUG_MESSAGES) {
      char buf[128];
      sprintf(buf, "Switch Hit = %d\n", switchHit);
      Serial.write(buf);
    }

    manageGameMode(switchHit);
    returnState = g_base.run(switchHit);
    scoreIncreased_ = false;
    executedSwitchStack = true;
  }

  if (!executedSwitchStack) manageGameMode(0xFF);

  return returnState;
}

int GameMode::manageTilt() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  unsigned long currentTime = g_machineState.currentTime();

  g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch (switchHit) {
      case SW_SELF_TEST_SWITCH:
        returnState = MACHINE_STATE_TEST_LIGHTS;
        g_selfTestAndAudit.setLastSelfTestChangedTime(currentTime);
        break;
      case SW_COIN_1:
      case SW_COIN_2:
      case SW_COIN_3:
        g_machineState.writeCoinToAudit(switchHit);
        g_machineState.increaseCredits(true, 1);
        break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return returnState;
}

void GameMode::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Game Mode Loop\n\r");
  while (BSOS_PullFirstFromSwitchStack() != SWITCH_STACK_EMPTY) {}

  setGameMode(GAME_MODE_SKILL_SHOT);
}

void GameMode::handlePlayerBonusLamps() {
  g_machineState.updateBonusLamps();
}

void GameMode::handlePlayerScore() {
  boolean shouldFlashScore = (firstSwitchHitTime_ == 0) ? true : false;
  boolean shouldDashScore  = (firstSwitchHitTime_ > 0 && ((g_machineState.currentTime() - g_machineState.lastScoreChangeTime()) > 2000)) ? true : false;
  g_machineState.updatePlayerScore(shouldFlashScore, shouldDashScore);
}

void GameMode::manageGameMode(byte switchHit) {
  byte newGameMode = gameModeId_;

  switch (gameModeId_) {
    case GAME_MODE_SKILL_SHOT:
      newGameMode = g_skillShot.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_UNSTRUCTURED_PLAY:
      newGameMode = g_unstructuredPlay.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_ORBS_1:
    case GAME_MODE_ORBS_2:
    case GAME_MODE_ORBS_3:
    case GAME_MODE_ORBS_4:
      newGameMode = g_orbMode1.run(gameModeChanged_, switchHit);
      break;
  }

  setGameMode(newGameMode);
  if(gameModeChanged_) manageGameMode(switchHit);
}
