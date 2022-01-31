#include "SharedVariables.h"

GameMode::GameMode(byte id) {
  setGameMode(id);
}

void GameMode::setGameMode(byte id) {
  ballFirstSwitchHitTime_ = 0;
  ballTimeInTrough_       = 0;
  gameModeEndTime_        = 0;
  gameModeId_             = id;
  gameModeStartTime_      = 0;

  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Game mode set to %d\n", gameModeId_);
    Serial.write(buf);
  }
}

int GameMode::run(int curState, boolean curStateChanged) {
  int returnState           = curState;
  byte currentBallInPlay    = g_machineState.currentBallInPlay();
  byte currentPlayer        = g_machineState.currentPlayer();

  g_displayHelper.showPlayerScores(currentPlayer);

  if (g_machineState.currentPlayerTilted()) {
    returnState = manageTilt(returnState);
  } else {
    returnState = manageGameBase(returnState);
    returnState = manageGameMode(returnState);

    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
      returnState = manageBallInTrough(returnState);
    } else {
      ballTimeInTrough_ = 0;
    }
  }

  return returnState;
}

boolean GameMode::ballSaveActive() {
  if (!ballFirstSwitchHitTime_) return true;

  return (g_machineState.currentTime() - ballFirstSwitchHitTime_) < ((unsigned long)BALL_SAVE_NUMBER_OF_SECONDS * 1000);
}

int GameMode::manageBallInTrough(int returnState) {
  unsigned long currentTime = g_machineState.currentTime();

  if (ballTimeInTrough_ == 0) ballTimeInTrough_ = currentTime;
  if ((currentTime - ballTimeInTrough_) <= 500) return returnState;

  if (ballFirstSwitchHitTime_ == 0) {
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime);
  } else if (!g_machineState.ballSaveUsed() && ballSaveActive()) {
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime + 100);
    g_machineState.setBallSaveUsed(true);
    BSOS_SetLampState(LAMP_SHOOT_AGAIN, 0);
    ballTimeInTrough_ = currentTime;
    returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  } else {
    returnState = MACHINE_STATE_COUNTDOWN_BONUS;
  }

  return returnState;
}

int GameMode::manageGameBase(int returnState) {
  unsigned long currentTime = g_machineState.currentTime();

  if (!ballSaveActive()) BSOS_SetLampState(LAMP_SHOOT_AGAIN, 0);

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    if (DEBUG_MESSAGES) {
      char buf[128];
      sprintf(buf, "Switch Hit = %d\n", switchHit);
      Serial.write(buf);
    }

    switch (switchHit) {
      case SW_TILT:
        g_machineState.registerTiltWarning();
        break;
      case SW_SELF_TEST_SWITCH:
        returnState = MACHINE_STATE_TEST_LIGHTS;
        g_selfTestAndAudit.setLastSelfTestChangedTime(currentTime);
        break;
      case SW_10_POINT_REBOUND:
      case SW_1ST_INLINE_DROP_TARGET:
      case SW_2ND_INLINE_DROP_TARGET:
      case SW_3RD_INLINE_DROP_TARGET:
      case SW_4TH_INLINE_DROP_TARGET:
      case SW_B_DROP_TARGET:
      case SW_INLANE_BACK_TARGET:
      case SW_LEFT_OUTLANE:
      case SW_LEFT_RETURN_LANE:
      case SW_LEFT_SIDE_RO_BUTTON:
      case SW_LEFT_SLINGSHOT:
      case SW_LEFT_THUMPER_BUMPER:
      case SW_ORBS_RIGHT_LANE_TARGET:
      case SW_O_DROP_TARGET:
      case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      case SW_RIGHT_4_DROP_TARGET_1:
      case SW_RIGHT_4_DROP_TARGET_2:
      case SW_RIGHT_4_DROP_TARGET_3:
      case SW_RIGHT_4_DROP_TARGET_4:
      case SW_RIGHT_OUTLANE:
      case SW_RIGHT_RETURN_LANE:
      case SW_RIGHT_SLINGSHOT:
      case SW_RIGHT_THUMPER_BUMPER:
      case SW_R_DROP_TARGET:
      case SW_S_DROP_TARGET:
      case SW_TOP_LEFT_LANE:
      case SW_TOP_LEFT_LANE_RO_BUTTON:
      case SW_TOP_MIDDLE_LANE:
      case SW_TOP_RIGHT_LANE:
      case SW_TOP_SPOT_1_THROUGH_4_TARGET:
        g_machineState.increaseScore(10);
        if (ballFirstSwitchHitTime_ == 0) ballFirstSwitchHitTime_ = currentTime;
        break;
      case SW_COIN_1:
      case SW_COIN_2:
      case SW_COIN_3:
        g_machineState.writeCoinToAudit(switchHit);
        g_machineState.increaseCredits(true, 1);
        break;
      case SW_CREDIT_BUTTON:
        if (DEBUG_MESSAGES) {
          Serial.write("Start game button pressed\n\r");
        }

        if (g_machineState.currentBallInPlay() == 1) {
          g_machineState.incrementNumberOfPlayers();
        } else {
          returnState = g_machineState.resetGame();
        }
        break;
    }
  }
}

int GameMode::manageGameMode(int returnState) {
  switch (gameModeId_) {
    case GAME_MODE_SKILL_SHOT:
      break;
  }

  return returnState;
}

int GameMode::manageTilt(int returnState) {
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
