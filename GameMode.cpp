#include "SharedVariables.h"

GameMode::GameMode() {
  gameModeId_             = GAME_MODE_INITIALIZE;
  gameModeChanged_        = true;

  ballSaveEndTime_        = 0;
  bonusIncreased_         = false;
  pushingBallFromOutlane_ = false;
  scoreIncreased_         = false;
}

boolean GameMode::bonusIncreased() {
  return bonusIncreased_;
}

boolean GameMode::scoreIncreased() {
  return scoreIncreased_;
}

byte GameMode::gameMode() {
  return gameModeId_;
}

int GameMode::run(boolean curStateChanged) {
  if (curStateChanged) manageNewMode();
  if (g_bonusLightShow.running()) g_bonusLightShow.run();

  manageBallSave();
  managePlayerScore();

  int returnState = runGameLoop();
  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    returnState = manageBallInTrough();
  } else {
    g_machineState.setTroughSwitchActivated(false);
    pushingBallFromOutlane_ = false;
  }

  return returnState;
}

void GameMode::setBonusIncreased(boolean value) {
  bonusIncreased_ = value;
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
  return g_machineState.mostRecentSwitchHitTime() < ballSaveEndTime_;
}

boolean GameMode::ballSaveLampActive() {
  return g_machineState.currentTime() < ballSaveEndTime_;
}

int GameMode::manageBallInTrough() {
  if (!g_machineState.troughSwitchActivated()) {
    g_machineState.setTroughSwitchActivated(true);
    g_machineState.setBallEnteredTroughTime();
  }

  if ((g_machineState.currentTime() - g_machineState.ballEnteredTroughTime()) <= 500 ||
      pushingBallFromOutlane_ ||
      g_machineState.scoreIncreasing()
     ) {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  if (!g_machineState.playfieldValidated() && !g_machineState.currentPlayerTilted()) {
    if (DEBUG_MESSAGES) Serial.write("Ball Returned To Shooter Lane\n\r");
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, SOL_OUTHOLE_KICKER_STRENGTH, g_machineState.currentTime() + 100);
    pushingBallFromOutlane_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;

  } else if (ballSaveActive() && !g_machineState.currentPlayerTilted()) {
    if (DEBUG_MESSAGES) Serial.write("Ball Saved\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE,           SOL_BALL_RELEASE_STRENGTH,           g_machineState.currentTime() + 100);
    BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, SOL_BALL_KICK_TO_PLAYFIELD_STRENGTH, g_machineState.currentTime() + 1000);
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER,         SOL_OUTHOLE_KICKER_STRENGTH,         g_machineState.currentTime() + 1100);
    pushingBallFromOutlane_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;

  } else if (g_machineState.numberOfBallsInPlay() > 1) {
    if (DEBUG_MESSAGES) Serial.write("Multiball Drained\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, SOL_OUTHOLE_KICKER_STRENGTH, g_machineState.currentTime() + 1000);
    pushingBallFromOutlane_ = true;

    g_machineState.decreaseNumberOfBallsInPlay();
    g_machineState.updateScoreMultiplierLamps();

    return MACHINE_STATE_NORMAL_GAMEPLAY;

  } else if ((g_machineState.currentTime() - g_machineState.ballEnteredTroughTime()) <= 500 || pushingBallFromOutlane_) {
    return MACHINE_STATE_NORMAL_GAMEPLAY;

  } else {
    if (DEBUG_MESSAGES) Serial.write("Ball Ended\n\r");

    if (!g_machineState.currentPlayerTilted())     g_soundHelper.stopAudio();
    if (g_bonusLightShow.running())                g_bonusLightShow.end();

    if (g_gameMode.gameMode() == GAME_MODE_UNSTRUCTURED_PLAY) g_unstructuredPlay.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_1)            g_orbMode1.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_2)            g_orbMode2.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_3)            g_orbMode3.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_4)            g_orbMode4.endModeViaBallEnded();
    gameModeId_ = GAME_MODE_INITIALIZE;

    g_displayHelper.showPlayerScores(0xFF);

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
        g_selfTestAndAudit.setLastSelfTestChangedTime();
        break;
      case SW_COIN_1:
      case SW_COIN_2:
      case SW_COIN_3:
        g_machineState.manageCoinDrop(switchHit);
        break;
      case SW_CREDIT_BUTTON:
        returnState = g_machineState.manageCreditButton();
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

  return returnState;
}

int GameMode::runGameModes() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
  boolean executedSwitchStack = false;

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    char buf[128];

    if (switchHit == g_machineState.mostRecentSwitchHit() && (g_machineState.currentTime() - g_machineState.mostRecentSwitchHitTime() < 100)) {
      if (DEBUG_MESSAGES && switchHit != SW_LEFT_FLIPPER_BUTTON) {
        sprintf(buf, "Skipped Switch Hit = %d\n", switchHit);
        Serial.write(buf);
      }
    } else {
      if (DEBUG_MESSAGES && switchHit != SW_LEFT_FLIPPER_BUTTON) {
        sprintf(buf, "Switch Hit = %d\n", switchHit);
        Serial.write(buf);
      }

      runGameMode(switchHit);
      returnState = g_base.run(switchHit);
      bonusIncreased_ = false;
      scoreIncreased_ = false;
      executedSwitchStack = true;
    }
  }

  if (!executedSwitchStack) runGameMode(0xFF);

  return returnState;
}

void GameMode::manageBallSave() {
  if (ballSaveActive()) {
    g_lampsHelper.showLamp(LAMP_SHOOT_AGAIN, true);
    g_lampsHelper.showLamp(LAMP_RELEASE_ORB, true);
  }

  if (g_machineState.currentPlayerTilted() || !ballSaveLampActive()) {
    g_lampsHelper.hideLamp(LAMP_SHOOT_AGAIN);
    g_lampsHelper.hideLamp(LAMP_RELEASE_ORB);
  }
}

void GameMode::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Game Mode Loop\n\r");
  while (BSOS_PullFirstFromSwitchStack() != SWITCH_STACK_EMPTY) {}

  g_bonusLightShow.reset();
  g_soundHelper.stopAudio();
  g_soundHelper.playSound(SOUND_CONTINIOUS_DRONE);
  g_machineState.increaseBonus(1);
  if (g_machineState.firstBallActive()) g_soundHelper.playSound(SOUND_DESTROY_CENTAUR);

  ballSaveEndTime_ = 0;

  setGameMode(GAME_MODE_SKILL_SHOT);
}

void GameMode::managePlayerScore() {
  boolean shouldFlashScore = g_machineState.playfieldValidated() ? false : true;
  boolean shouldDashScore  =
    g_machineState.playfieldValidated() &&
    ((g_machineState.currentTime() - g_machineState.mostRecentSwitchHitTime()) > 2000) ? true : false;

  g_machineState.updatePlayerScore(shouldFlashScore, shouldDashScore);
}

void GameMode::setBallSaveEndTime(unsigned long value) {
  ballSaveEndTime_ = value;
}

void GameMode::runGameMode(byte switchHit) {
  byte newGameMode    = gameModeId_;
  boolean rerunSwitch = false;

  switch (gameModeId_) {
    case GAME_MODE_SKILL_SHOT:
      newGameMode = g_skillShot.run(gameModeChanged_, switchHit);
      rerunSwitch = true;
      break;
    case GAME_MODE_UNSTRUCTURED_PLAY:
      newGameMode = g_unstructuredPlay.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_ORBS_1:
      newGameMode = g_orbMode1.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_ORBS_2:
      newGameMode = g_orbMode2.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_ORBS_3:
      newGameMode = g_orbMode3.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_ORBS_4:
      newGameMode = g_orbMode4.run(gameModeChanged_, switchHit);
      break;
  }

  setGameMode(newGameMode);
  if(gameModeChanged_ && rerunSwitch) runGameMode(switchHit);
}
