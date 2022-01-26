#include "SharedVariables.h"

GameMode::GameMode(byte id) {
  SetGameMode(id);
}

void GameMode::SetGameMode(byte id) {
  gameModeId = id;

  gameModeStartTime = 0;
  gameModeEndTime = 0;

  ballFirstSwitchHitTime = 0;
  ballTimeInTrough = 0;

  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Game mode set to %d\n", gameModeId);
    Serial.write(buf);
  }
}

int GameMode::ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  unsigned long currentTime = GlobalMachineState.GetCurrentTime();

  switch (gameModeId) {
    case GAME_MODE_SKILL_SHOT:
      if (gameModeStartTime == 0) {
        gameModeStartTime = currentTime;
        gameModeEndTime = 0;
      }

      if (ballFirstSwitchHitTime != 0) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }

      if (gameModeEndTime != 0 && currentTime > gameModeEndTime) {
        ShowPlayerScores(0xFF, false, false);
      }
      break;
  }

  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    if (ballTimeInTrough == 0) {
      ballTimeInTrough = currentTime;
    } else {
      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing
      if ((currentTime - ballTimeInTrough) > 500) {
        if (ballFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings) {
          // Nothing hit yet, so return the ball to the player
          BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime);
          ballTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          GlobalMachineState.IncreaseScore(ScoreAdditionAnimation);
          ScoreAdditionAnimationStartTime = 0;
          ScoreAdditionAnimation = 0;
          ShowPlayerScores(0xFF, false, false);
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (!BallSaveUsed && ((currentTime - ballFirstSwitchHitTime)) < ((unsigned long)BallSaveNumSeconds * 1000)) {
            BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime + 100);
            BallSaveUsed = true;
            BSOS_SetLampState(LAMP_SHOOT_AGAIN, 0);
            ballTimeInTrough = currentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
          } else {
            ShowPlayerScores(0xFF, false, false);

            returnState = MACHINE_STATE_COUNTDOWN_BONUS;
          }
        }
      }
    }
  } else {
    ballTimeInTrough = 0;
  }

  return returnState;
}

int GameMode::RunGamePlayState(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long currentTime = GlobalMachineState.GetCurrentTime();
  byte currentPlayer = GlobalMachineState.GetCurrentPlayer();
  byte currentBallInPlay = GlobalMachineState.GetCurrentBallInPlay();

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = GlobalMachineState.InitGamePlay();
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = GlobalMachineState.InitNewBall(curStateChanged, currentPlayer, currentBallInPlay);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    returnState = CountdownBonus(curStateChanged);
    ShowPlayerScores(0xFF, false, false);
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    BSOS_SetDisplayCredits(Credits);

    if (GlobalMachineState.GetSamePlayerShootsAgain()) {
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {
      returnState = GlobalMachineState.IncrementCurrentPlayer();
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

      unsigned long lastTiltWarningTime = GlobalMachineState.GetLastTiltWarningTime();

      switch (switchHit) {
        case SW_TILT:
          if ((currentTime - lastTiltWarningTime) > TILT_WARNING_DEBOUNCE_TIME) {
            GlobalMachineState.SetLastTiltWarningTime(currentTime);
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
          SetLastSelfTestChangedTime(currentTime);
          break;
        case SW_LEFT_SLINGSHOT:
        case SW_RIGHT_SLINGSHOT:
          GlobalMachineState.IncreaseScore(10);
          if (ballFirstSwitchHitTime == 0) ballFirstSwitchHitTime = currentTime;
          break;
        case SW_COIN_1:
        case SW_COIN_2:
        case SW_COIN_3:
          GlobalMachineState.WriteCoinToAudit(switchHit);
          GlobalMachineState.IncreaseCredits(true, 1);
          break;
        case SW_CREDIT_BUTTON:
          if (DEBUG_MESSAGES) {
            Serial.write("Start game button pressed\n\r");
          }

          if (GlobalMachineState.GetCurrentBallInPlay() == 1) {
            GlobalMachineState.IncrementNumberOfPlayers();
          } else {
            returnState = GlobalMachineState.ResetGame();
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
          SetLastSelfTestChangedTime(currentTime);
          break;
        case SW_COIN_1:
        case SW_COIN_2:
        case SW_COIN_3:
          GlobalMachineState.WriteCoinToAudit(switchHit);
          GlobalMachineState.IncreaseCredits(true, 1);
          break;
      }

      switchHit = BSOS_PullFirstFromSwitchStack();
    }
  }

  return returnState;
}
