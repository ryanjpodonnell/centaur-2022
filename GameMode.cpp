#include "SharedVariables.h"

GameMode::GameMode(byte id) {
  ballTimeInTrough_   = 0;
  firstSwitchHitTime_ = 0;
  gameModeChanged_    = true;
  savingBall_         = false;
  scoreIncreased_     = false;

  setGameMode(id);
}

boolean GameMode::scoreIncreased() {
  return scoreIncreased_;
}

int GameMode::run(boolean curStateChanged) {
  if (curStateChanged) handleNewMode();
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  // flashes or dashes the current score
  boolean shouldFlashScore = (firstSwitchHitTime_ == 0) ? true : false;
  boolean shouldDashScore  = (firstSwitchHitTime_ > 0 && ((g_machineState.currentTime() - g_machineState.lastScoreChangeTime()) > 2000)) ? true : false;
  g_displayHelper.showPlayerScores(g_machineState.currentPlayer(), shouldFlashScore, shouldDashScore);

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

void GameMode::setFirstSwitchHitTime(unsigned long value) {
  firstSwitchHitTime_ = value;
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
  if (!firstSwitchHitTime_) return true;

  return (g_machineState.currentTime() - firstSwitchHitTime_) < ((unsigned long)BALL_SAVE_NUMBER_OF_SECONDS * 1000);
}


int GameMode::manageBallInTrough() {
  unsigned long currentTime = g_machineState.currentTime();

  if (ballTimeInTrough_ == 0) ballTimeInTrough_ = currentTime;
  if ((currentTime - ballTimeInTrough_) <= 500) return MACHINE_STATE_NORMAL_GAMEPLAY;
  if (savingBall_) return MACHINE_STATE_NORMAL_GAMEPLAY;

  if (firstSwitchHitTime_ == 0) {
    // no switches hit. return ball to shooter lane
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime + 100);

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else if (!g_machineState.ballSaveUsed() && ballSaveActive()) {
    // ball save active. return ball to shooter lane
    if (DEBUG_MESSAGES) Serial.write("Ball Saved\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE, 4, currentTime + 100);
    BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, 4, currentTime + 1000);
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime + 1000);

    g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);
    g_machineState.setBallSaveUsed(true);
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

  if (!ballSaveActive()) g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);

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
  }

  return returnState;
}

int GameMode::manageTilt() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  unsigned long currentTime = g_machineState.currentTime();

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
  if (DEBUG_MESSAGES) Serial.write("Entering Skill Shot Game Mode\n\r");

  g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
  g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER, true);
}

void GameMode::manageGameMode(byte switchHit) {
  byte newGameMode = gameModeId_;

  switch (gameModeId_) {
    case GAME_MODE_SKILL_SHOT:
      newGameMode = g_skillShot.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_INITIALIZE:
      newGameMode = GAME_MODE_INITIALIZE;
    case GAME_MODE_UNSTRUCTURED_PLAY:
      newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
      break;
  }

  setGameMode(newGameMode);
}
