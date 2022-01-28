#include "SharedVariables.h"

GameMode::GameMode(byte id) {
  setGameMode(id);
}

void GameMode::setGameMode(byte id) {
  gameModeId_ = id;

  gameModeStartTime_ = 0;
  gameModeEndTime_ = 0;

  ballFirstSwitchHitTime_ = 0;
  ballTimeInTrough_ = 0;

  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Game mode set to %d\n", gameModeId_);
    Serial.write(buf);
  }
}

int GameMode::manageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  unsigned long currentTime = g_machineState.currentTime();

  switch (gameModeId_) {
    case GAME_MODE_SKILL_SHOT:
      if (gameModeStartTime_ == 0) {
        gameModeStartTime_ = currentTime;
        gameModeEndTime_ = 0;
      }

      if (ballFirstSwitchHitTime_ != 0) {
        setGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }

      if (gameModeEndTime_ != 0 && currentTime > gameModeEndTime_) {
        g_displayHelper.showPlayerScores(0xFF, false, false);
      }
      break;
  }

  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    if (ballTimeInTrough_ == 0) {
      ballTimeInTrough_ = currentTime;
    } else {
      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing
      if ((currentTime - ballTimeInTrough_) > 500) {
        if (ballFirstSwitchHitTime_ == 0 && NumTiltWarnings <= MaxTiltWarnings) {
          // Nothing hit yet, so return the ball to the player
          BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime);
          ballTimeInTrough_ = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          g_machineState.increaseScore(ScoreAdditionAnimation);
          ScoreAdditionAnimationStartTime = 0;
          ScoreAdditionAnimation = 0;
          g_displayHelper.showPlayerScores(0xFF, false, false);
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          byte ballSaveNumSeconds = g_machineState.ballSaveNumSeconds();
          if (!BallSaveUsed && ((currentTime - ballFirstSwitchHitTime_)) < ((unsigned long)ballSaveNumSeconds * 1000)) {
            BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime + 100);
            BallSaveUsed = true;
            BSOS_SetLampState(LAMP_SHOOT_AGAIN, 0);
            ballTimeInTrough_ = currentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
          } else {
            g_displayHelper.showPlayerScores(0xFF, false, false);

            returnState = MACHINE_STATE_COUNTDOWN_BONUS;
          }
        }
      }
    }
  } else {
    ballTimeInTrough_ = 0;
  }

  return returnState;
}

int GameMode::runGamePlayState(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long currentTime = g_machineState.currentTime();
  byte currentPlayer = g_machineState.currentPlayer();
  byte currentBallInPlay = g_machineState.currentBallInPlay();

  g_displayHelper.showPlayerScores(currentPlayer, false, false);

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = g_machineState.initGamePlay();
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = g_machineState.initNewBall(curStateChanged, currentPlayer, currentBallInPlay);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = manageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    returnState = CountdownBonus(curStateChanged);
    g_displayHelper.showPlayerScores(0xFF, false, false);
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    BSOS_SetDisplayCredits(Credits);

    if (g_machineState.samePlayerShootsAgain()) {
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {
      returnState = g_machineState.incrementCurrentPlayer();
    }
  } else if (curState == MACHINE_STATE_MATCH_MODE) {
  }

  byte switchHit;
  if (NumTiltWarnings <= MaxTiltWarnings) {
    while ( (switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {

      if (DEBUG_MESSAGES) {
        char buf[128];
        sprintf(buf, "Switch Hit = %d\n", switchHit);
        Serial.write(buf);
      }

      unsigned long lastTiltWarningTime = g_machineState.lastTiltWarningTime();

      switch (switchHit) {
        case SW_TILT:
          if ((currentTime - lastTiltWarningTime) > TILT_WARNING_DEBOUNCE_TIME) {
            g_machineState.setLastTiltWarningTime(currentTime);
            NumTiltWarnings += 1;
            if (NumTiltWarnings > MaxTiltWarnings) {
              BSOS_DisableSolenoidStack();
              BSOS_SetDisableFlippers(true);
              BSOS_TurnOffAllLamps();
              BSOS_SetLampState(LAMP_TILT, 1);
            }
          }
          break;
        case SW_SELF_TEST_SWITCH:
          returnState = MACHINE_STATE_TEST_LIGHTS;
          g_selfTestAndAudit.setLastSelfTestChangedTime(currentTime);
          break;
        case SW_LEFT_SLINGSHOT:
        case SW_RIGHT_SLINGSHOT:
          g_machineState.increaseScore(1);
          if (ballFirstSwitchHitTime_ == 0) ballFirstSwitchHitTime_ = currentTime;
          break;
        case SW_TOP_RIGHT_LANE:
        case SW_TOP_MIDDLE_LANE:
        case SW_TOP_LEFT_LANE:
          g_machineState.increaseScore(12345678);
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
  } else {
    // We're tilted, so just wait for outhole
    switchHit = BSOS_PullFirstFromSwitchStack();

    while (switchHit != SWITCH_STACK_EMPTY) {
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
  }

  return returnState;
}
