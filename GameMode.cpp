#include "SharedVariables.h"

GameMode::GameMode() {
  gameModeId_             = GAME_MODE_INITIALIZE;
  gameModeChanged_        = true;

  pushingBallFromOutlane_ = false;
  scoreIncreased_         = false;
}

boolean GameMode::scoreIncreased() {
  return scoreIncreased_;
}

int GameMode::run(boolean curStateChanged) {
  if (curStateChanged) handleNewMode();

  handlePlayerScore();
  handlePlayerBonusLamps();
  handleShootAgainLamp();

  int returnState = runGameLoop();
  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    returnState = manageBallInTrough();
  } else {
    g_machineState.setTroughSwitchActivated(false);
    pushingBallFromOutlane_ = false;
  }

  return returnState;
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
  if (!g_machineState.playfieldValidated()) return true;

  return (g_machineState.mostRecentSwitchHitTime() - g_machineState.currentBallFirstSwitchHitTime())
    < ((unsigned long)BALL_SAVE_NUMBER_OF_SECONDS * 1000);
}

boolean GameMode::ballSaveLampActive() {
  if (g_machineState.currentPlayerTilted()) return false;
  if (!g_machineState.playfieldValidated()) return true;

  return (g_machineState.currentTime() - g_machineState.currentBallFirstSwitchHitTime())
    < ((unsigned long)BALL_SAVE_NUMBER_OF_SECONDS * 1000);
}

int GameMode::manageBallInTrough() {
  if (!g_machineState.troughSwitchActivated()) {
    g_machineState.setTroughSwitchActivated(true);
    g_machineState.setBallEnteredTroughTime();
  }

  if ((g_machineState.currentTime() - g_machineState.ballEnteredTroughTime()) <= 500 || pushingBallFromOutlane_) {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  if (!g_machineState.playfieldValidated() && !g_machineState.currentPlayerTilted()) {
    if (DEBUG_MESSAGES) Serial.write("Ball Returned To Shooter Lane\n\r");
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, g_machineState.currentTime() + 100);

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else if (!g_machineState.ballSaveUsed() && ballSaveActive()) {
    if (DEBUG_MESSAGES) Serial.write("Ball Saved\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE, 4, g_machineState.currentTime() + 100);
    BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, 6, g_machineState.currentTime() + 1000);
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, g_machineState.currentTime() + 1000);

    g_machineState.setBallSaveUsed(true);
    pushingBallFromOutlane_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else if (g_machineState.numberOfBallsInPlay() > 1) {
    if (DEBUG_MESSAGES) Serial.write("Multiball Drained\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, g_machineState.currentTime() + 1000);
    g_machineState.decreaseNumberOfBallsInPlay();
    g_machineState.decreaseModeMultiplier();
    g_machineState.updateModeMultiplierLamps();
    pushingBallFromOutlane_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else {
    if (DEBUG_MESSAGES) Serial.write("Ball Ended\n\r");
    setGameMode(GAME_MODE_INITIALIZE);

    return MACHINE_STATE_COUNTDOWN_BONUS;
  }
}

int GameMode::manageTilt() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch (switchHit) {
      case SW_SELF_TEST_SWITCH:
        returnState = MACHINE_STATE_TEST_LIGHTS;
        g_selfTestAndAudit.setLastSelfTestChangedTime(g_machineState.currentTime());
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

int GameMode::runGameLoop() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  if (g_machineState.currentPlayerTilted()) {
    returnState = manageTilt();
  } else {
    returnState = runGameModes();
  }
}

int GameMode::runGameModes() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  boolean executedSwitchStack = false;

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    if (DEBUG_MESSAGES) {
      char buf[128];
      sprintf(buf, "Switch Hit = %d\n", switchHit);
      Serial.write(buf);
    }

    runGameMode(switchHit);
    returnState = g_base.run(switchHit);
    scoreIncreased_ = false;
    executedSwitchStack = true;
  }

  if (!executedSwitchStack) runGameMode(0xFF);

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
  boolean shouldFlashScore = g_machineState.playfieldValidated() ? false : true;
  boolean shouldDashScore  =
    g_machineState.playfieldValidated() &&
    ((g_machineState.currentTime() - g_machineState.mostRecentSwitchHitTime()) > 2000) ? true : false;

  g_machineState.updatePlayerScore(shouldFlashScore, shouldDashScore);
}

void GameMode::handleShootAgainLamp() {
  if (!BALL_SAVE_NUMBER_OF_SECONDS) return;

  if (!g_machineState.ballSaveActivated()) {
    g_lampsHelper.showLamp(LAMP_SHOOT_AGAIN, true);
    g_machineState.setBallSaveActivated();
  }

  if (g_machineState.currentPlayerTilted() ||
      g_machineState.ballSaveUsed() ||
      !ballSaveLampActive()) {
    g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);
  }
}

void GameMode::runGameMode(byte switchHit) {
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
  if(gameModeChanged_) runGameMode(switchHit);
}
