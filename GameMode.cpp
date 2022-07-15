#include "SharedVariables.h"

GameMode::GameMode() {
  gameModeId_             = GAME_MODE_INITIALIZE;
  gameModeChanged_        = true;

  ballEnteredTroughTime_  = 0;
  ballSaveEndTime_        = 0;
  ballSaveSolEndTime_     = 0;
  bonusIncreased_         = false;
  overrideSound_          = false;
  pushingBallFromOutlane_ = false;
  scoreIncreased_         = false;
}

boolean GameMode::bonusIncreased() {
  return bonusIncreased_;
}

boolean GameMode::overrideSound() {
  return overrideSound_;
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
    ballEnteredTroughTime_  = 0;
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

void GameMode::setOverrideSound(boolean value) {
  overrideSound_ = value;
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
  if (!ballEnteredTroughTime_) {
    if (DEBUG_MESSAGES) Serial.write("Trough Activated\n\r");
    ballEnteredTroughTime_ = g_machineState.currentTime();
  }

  if ((g_machineState.currentTime() - ballEnteredTroughTime_) <= 500 || pushingBallFromOutlane_ || g_machineState.scoreIncreasing()) {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  // ball has settled in SW_OUTHOLE for 1/2 second

  if (!g_machineState.playfieldValidated() && !g_machineState.currentPlayerTilted()) {
    if (DEBUG_MESSAGES) Serial.write("Ball Returned To Shooter Lane\n\r");
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, SOL_OUTHOLE_KICKER_STRENGTH, g_machineState.currentTime());
    pushingBallFromOutlane_ = true;

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  if (ballSaveActive() && !g_machineState.currentPlayerTilted()) {
    if (g_machineState.currentTime() < ballSaveSolEndTime_) return MACHINE_STATE_NORMAL_GAMEPLAY;

    if (DEBUG_MESSAGES) Serial.write("Ball Saved\n\r");

    // push ball from outhole to trough (in case the trough is empty)
    if (g_machineState.numberOfBallsInTrough() == 0) {
      BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER,       SOL_OUTHOLE_KICKER_STRENGTH,         g_machineState.currentTime());
    }

    // launch new ball from trough to playfield
    BSOS_PushToTimedSolenoidStack(SOL_BALL_RELEASE,           SOL_BALL_RELEASE_STRENGTH,           g_machineState.currentTime() + 1500);
    BSOS_PushToTimedSolenoidStack(SOL_BALL_KICK_TO_PLAYFIELD, SOL_BALL_KICK_TO_PLAYFIELD_STRENGTH, g_machineState.currentTime() + 2500);

    // push ball from outhole to trough
    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER,         SOL_OUTHOLE_KICKER_STRENGTH,         g_machineState.currentTime() + 2750);

    pushingBallFromOutlane_ = true;
    ballSaveSolEndTime_     = g_machineState.currentTime() + 3000;

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  if (g_machineState.numberOfBallsInTrough() != 2) {
    if (DEBUG_MESSAGES) Serial.write("Multiball Drained\n\r");

    BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, SOL_OUTHOLE_KICKER_STRENGTH, g_machineState.currentTime());
    pushingBallFromOutlane_ = true;

    g_machineState.decreaseNumberOfBallsInPlay();
    if (!g_machineState.currentPlayerTilted()) g_machineState.updateScoreMultiplierLamps();

    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  if (g_machineState.numberOfBallsInTrough() == 2) {
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
  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch (switchHit) {
      case SW_CREDIT_BUTTON:
        return g_machineState.manageCreditButton(MACHINE_STATE_NORMAL_GAMEPLAY);
        break;
      case SW_COIN_1:
      case SW_COIN_2:
      case SW_COIN_3:
        g_machineState.manageCoinDrop(switchHit);
        break;
      case SW_SELF_TEST_SWITCH:
        return MACHINE_STATE_TEST_LIGHTS;
        break;
    }
  }

  return MACHINE_STATE_NORMAL_GAMEPLAY;
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
      returnState         = g_base.run(switchHit);
      executedSwitchStack = true;
      bonusIncreased_     = false;
      overrideSound_      = false;
      scoreIncreased_     = false;
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

  ballEnteredTroughTime_ = 0;
  ballSaveEndTime_       = 0;
  ballSaveSolEndTime_    = 0;

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
    case GAME_MODE_DESTROY_CENTAUR:
      newGameMode = g_destroyCentaur.run(gameModeChanged_, switchHit);
      break;
  }

  setGameMode(newGameMode);
  if(gameModeChanged_ && rerunSwitch) runGameMode(switchHit);
}
