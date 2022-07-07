#include "SharedVariables.h"

GameMode::GameMode() {
  gameModeId_             = GAME_MODE_INITIALIZE;
  gameModeChanged_        = true;

  ballSaveEndTime_        = 0;
  bonusIncreased_         = false;
  indicatorPlayed_        = false;
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
  manageHurryUp();
  managePlayerBonusLamps();
  managePlayerScore();
  manageShotIndicatorShow();

  int returnState = runGameLoop();
  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    returnState = manageBallInTrough();
  } else {
    g_machineState.setTroughSwitchActivated(false);
    pushingBallFromOutlane_ = false;
  }

  return returnState;
}

void GameMode::endHurryUp() {
  if (DEBUG_MESSAGES) Serial.write("Hurry Up Ended\n\r");

  g_displayHelper.showPlayerScores(0xFF);
  g_lampsHelper.showLamps(LAMP_COLLECTION_QUEENS_CHAMBER_GI);
  g_machineState.setHurryUpActivated(false);
  g_machineState.updateQueensChamberLamps();
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

void GameMode::startHurryUp(unsigned long value, int seconds) {
  if (DEBUG_MESSAGES) Serial.write("Hurry Up Started\n\r");

  g_soundHelper.playSoundWithoutInterruptions(SOUND_SIREN_2);
  g_machineState.setHurryUpActivated(true);
  g_machineState.setHurryUpValue(value);
  hurryUpInitialValue_        = value;
  hurryUpStartedTime_         = g_machineState.currentTime();
  hurryUpEndTime_             = g_machineState.currentTime() + (1000 * seconds) + HURRY_UP_GRACE_PERIOD;
  hurryUpValuePerMillisecond_ = value / (1000 * seconds);
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
    if (g_machineState.hurryUpActivated())         endHurryUp();

    if (g_gameMode.gameMode() == GAME_MODE_ORBS_1) g_orbMode1.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_2) g_orbMode2.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_3) g_orbMode1.endModeViaBallEnded();
    if (g_gameMode.gameMode() == GAME_MODE_ORBS_4) g_orbMode1.endModeViaBallEnded();
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

void GameMode::manageHurryUp() {
  if (!g_machineState.hurryUpActivated()) return;

  updateHurryUpValue();
  updateHurryUpLamps();

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

void GameMode::managePlayerBonusLamps() {
  if (gameModeId_ != GAME_MODE_UNSTRUCTURED_PLAY) return;
  if (g_bonusLightShow.running()) return;

  g_machineState.updateBonusLamps();
}

void GameMode::managePlayerScore() {
  boolean shouldFlashScore = g_machineState.playfieldValidated() ? false : true;
  boolean shouldDashScore  =
    g_machineState.playfieldValidated() &&
    ((g_machineState.currentTime() - g_machineState.mostRecentSwitchHitTime()) > 2000) ? true : false;

  g_machineState.updatePlayerScore(shouldFlashScore, shouldDashScore);
}

void GameMode::manageShotIndicatorShow() {
  if (gameModeId_ != GAME_MODE_UNSTRUCTURED_PLAY) return;
  if (g_bonusLightShow.running()) return;

  if (!g_machineState.playfieldValidated()) return;
  if ((g_machineState.currentTime() - g_machineState.mostRecentSwitchHitTime()) < 10000) return;
  if (indicatorPlayed_) return;

  if (g_machineState.orbsDropTargetsAllStanding() && g_machineState.rightDropTargetsAllStanding()) {
    indicatorPlayed_ = true;
    g_lampsHelper.showLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS, true);
    g_lampsHelper.showLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS, true);
    g_bonusLightShow.start(BONUS_LIGHT_SHOW_ORBS_AND_RIGHT_DROPS_ARROW);
    return;
  }

  if (g_machineState.orbsDropTargetsAllStanding()) {
    indicatorPlayed_ = true;
    g_lampsHelper.showLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS, true);
    g_bonusLightShow.start(BONUS_LIGHT_SHOW_ORBS_DROPS_ARROW);
    return;
  }

  if (g_machineState.rightDropTargetsAllStanding()) {
    indicatorPlayed_ = true;
    g_lampsHelper.showLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS, true);
    g_bonusLightShow.start(BONUS_LIGHT_SHOW_RIGHT_DROPS_ARROW);
    return;
  }
}

void GameMode::resetIndicatorPlayed() {
  indicatorPlayed_ = false;
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
    case GAME_MODE_ORBS_3:
    case GAME_MODE_ORBS_4:
      newGameMode = g_orbMode1.run(gameModeChanged_, switchHit);
      break;
    case GAME_MODE_ORBS_2:
      newGameMode = g_orbMode2.run(gameModeChanged_, switchHit);
      break;
  }

  setGameMode(newGameMode);
  if(gameModeChanged_ && rerunSwitch) runGameMode(switchHit);
}

void GameMode::updateHurryUpLamps() {
  unsigned long seed = g_machineState.currentTime() / 50; // .05 seconds
  if (seed != lastFlash_) {
    lastFlash_ = seed;

    byte currentStep = seed % 5;
    if (g_machineState.queensChamberHurryUpValue() == 100000) {
      g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
    } else if (g_machineState.queensChamberHurryUpValue() == 200000) {
      g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
    } else if (g_machineState.queensChamberHurryUpValue() == 300000) {
      g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
    } else if (g_machineState.queensChamberHurryUpValue() == 400000) {
      g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
    } else if (g_machineState.queensChamberHurryUpValue() == 500000) {
      g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
    }
    if (currentStep == 0) g_lampsHelper.showLamp(LAMP_10_CHAMBER);
    if (currentStep == 1) g_lampsHelper.showLamp(LAMP_20_CHAMBER);
    if (currentStep == 2) g_lampsHelper.showLamp(LAMP_30_CHAMBER);
    if (currentStep == 3) g_lampsHelper.showLamp(LAMP_40_CHAMBER);
    if (currentStep == 4) g_lampsHelper.showLamp(LAMP_50_CHAMBER);

    currentStep = seed % 4;
    g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_GI);
    if (currentStep == 0) g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_1);
    if (currentStep == 1) g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_2);
    if (currentStep == 2) g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_3);
    if (currentStep == 3) g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_4);
  }
}

void GameMode::updateHurryUpValue() {
  unsigned long timeSinceHurryUpStarted = g_machineState.currentTime() - hurryUpStartedTime_;
  if (timeSinceHurryUpStarted < HURRY_UP_GRACE_PERIOD) return;

  unsigned long value = hurryUpValuePerMillisecond_ * (timeSinceHurryUpStarted - HURRY_UP_GRACE_PERIOD);
  g_machineState.setHurryUpValue(hurryUpInitialValue_ - value);

  if (g_machineState.currentTime() >= hurryUpEndTime_) {
    endHurryUp();
  }
}
