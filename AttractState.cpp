#include "SharedVariables.h"

Attract::Attract() {
  bonusLightShowEndTime_  = 0;
  currentFeatureCycle_    = 0;
  currentFlashCycle_      = 0;
  featureShowStartedTime_ = 0;
  lastFlash_              = 0;
  lastTaunt_              = 0;

  updateScores_       = true;
  bonusShowRunning_   = false;
  featureShowRunning_ = false;
}

int Attract::run(int curState, boolean curStateChanged) {
  if (curStateChanged) handleNewState();

  if (bonusShowRunning_) {
    handleBonusLightShow();
  } else if (featureShowRunning_) {
    handleFeatureShow();
  } else {
    handleLightShow();
  }

  int returnState = curState;
  byte switchHit  = BSOS_PullFirstFromSwitchStack();

  while (switchHit != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
    case(SW_CREDIT_BUTTON):
      if (g_machineState.resetPlayers()) startBonusLightsShow();
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      g_soundHelper.playSound(SOUND_ENERGIZE_ME);
      g_machineState.writeCoinToAudit(switchHit);
      g_machineState.increaseCredits(true, 1);
      break;
    case SW_SELF_TEST_SWITCH:
      if (g_machineState.currentTime() - g_selfTestAndAudit.lastSelfTestChangedTime() > 250) {
        returnState = MACHINE_STATE_TEST_LIGHTS;
        g_selfTestAndAudit.setLastSelfTestChangedTime(g_machineState.currentTime());
      }
      break;
    case SW_RIGHT_FLIPPER_BUTTON:
      if (featureShowEligible()) startFeatureShow();
      else if (tauntEligible()) {
        g_soundHelper.playSound(SOUND_HA_HA_HA);
        lastTaunt_ = g_machineState.currentTime();
      }
      break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  if (bonusLightShowEndTime_ && g_machineState.currentTime() > bonusLightShowEndTime_) {
    bonusLightShowEndTime_ = 0;
    bonusShowRunning_      = false;
    featureShowRunning_    = false;
    returnState            = MACHINE_STATE_INIT_GAMEPLAY;
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
boolean Attract::featureShowEligible() {
  if (featureShowRunning_)          return false;
  if (featureShowStartedTime_ == 0) return true;

  return g_machineState.currentTime() - featureShowStartedTime_ > FEATURE_SHOW_TIMEOUT;
}

boolean Attract::tauntEligible() {
  if (featureShowRunning_) return false;

  return g_machineState.currentTime() - lastTaunt_ > TAUNT_TIMEOUT;
}

void Attract::handleBonusLightShow() {
  unsigned long seed = g_machineState.currentTime() / 100;   // .10 seconds

  if (seed != lastFlash_) {
    lastFlash_ = seed;
    byte numberOfSteps = 5;
    byte currentStep = seed % numberOfSteps;

    g_lampsHelper.hideAllLamps();
    if (currentStep == 0) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_1);
    if (currentStep == 1) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_2);
    if (currentStep == 2) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_3);
    if (currentStep == 3) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_4);
    if (currentStep == 4) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_5);
  }
}

void Attract::handleNewState() {
  if (DEBUG_MESSAGES) Serial.write("Entering Attract State\n\r");

  BSOS_DisableSolenoidStack();
  BSOS_TurnOffAllLamps();
  BSOS_SetDisableFlippers(true);

  score1_ = g_machineState.score(0);
  score2_ = g_machineState.score(1);
  score3_ = g_machineState.score(2);
  score4_ = g_machineState.score(3);

  BSOS_SetDisplayCredits(g_machineState.credits());

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch (switchHit) {
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

void Attract::handleFeatureShow() {
  unsigned long timeSinceStateStarted = g_machineState.currentTime() - featureShowStartedTime_;

  byte          featureCycle = 0;
  if (timeSinceStateStarted >= 0)     featureCycle = 1;
  if (timeSinceStateStarted >= 1350)  featureCycle = 2;
  if (timeSinceStateStarted >= 2950)  featureCycle = 3;
  if (timeSinceStateStarted >= 4875)  featureCycle = 4;
  if (timeSinceStateStarted >= 6925)  featureCycle = 5;
  if (timeSinceStateStarted >= 8875)  featureCycle = 6;
  if (timeSinceStateStarted >= 10750) featureCycle = 7;

  if (featureCycle != currentFeatureCycle_) {
    currentFeatureCycle_ = featureCycle;
    g_lampsHelper.hideAllLamps();

    switch(featureCycle) {
      case(1):
        g_soundHelper.playSound(SOUND_CHALLENGE_ME);
        break;
      case(2):
        g_soundHelper.playSound(SOUND_ORB_FEATURE);
        g_lampsHelper.showLamps(LAMP_COLLECTION_ORB_FEATURE, true);
        break;
      case(3):
        g_soundHelper.playSound(SOUND_SEQUENCE_FEATURE);
        g_lampsHelper.showLamps(LAMP_COLLECTION_SEQUENCE_FEATURE, true);
        break;
      case(4):
        g_soundHelper.playSound(SOUND_CHAMBER_FEATURE);
        g_lampsHelper.showLamps(LAMP_COLLECTION_CHAMBER_FEATURE, true);
        break;
      case(5):
        g_soundHelper.playSound(SOUND_GUARDIAN_FEATURE);
        g_lampsHelper.showLamps(LAMP_COLLECTION_GUARDIAN_FEATURE, true);
        break;
      case(6):
        g_soundHelper.playSound(SOUND_BONUS_FEATURE);
        g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_FEATURE, true);
        break;
      case(7):
        featureShowRunning_  = false;
        break;
    }
  }
}

void Attract::handleLightShow() {
  unsigned long seed      = g_machineState.currentTime() / 100;   // .10 seconds
  unsigned long cycleSeed = g_machineState.currentTime() / 10000; // 10 seconds

  if (cycleSeed != currentFlashCycle_) {
    currentFlashCycle_ = cycleSeed;
    updateScores_ = true;
  }

  if (seed != lastFlash_) {
    lastFlash_ = seed;
    byte numberOfSteps = 16;
    byte currentStep = 0;

    if (currentFlashCycle_ % 2 == 0) {
      currentStep = seed % numberOfSteps;

      if (updateScores_ == true) {
        unsigned long highScore = g_machineState.highScore();
        g_machineState.setScore(highScore, 0);
        g_machineState.setScore(highScore, 1);
        g_machineState.setScore(highScore, 2);
        g_machineState.setScore(highScore, 3);
        g_displayHelper.showPlayerScores(0xFF);
        updateScores_ = false;
      }
    } else {
      currentStep = numberOfSteps - ((seed % numberOfSteps) + 1);

      if (updateScores_ == true) {
        g_machineState.setScore(score1_, 0);
        g_machineState.setScore(score2_, 1);
        g_machineState.setScore(score3_, 2);
        g_machineState.setScore(score4_, 3);
        g_displayHelper.showPlayerScores(0xFF);
        updateScores_ = false;
      }
    }

    g_lampsHelper.hideAllLamps();
    if (currentStep == 0) g_lampsHelper.showLamp(LAMP_40K_BONUS);
    if (currentStep == 1) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_MIDDLE_RING);
    if (currentStep == 2) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_OUTER_RING);
    if (currentStep == 3) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_4);
    if (currentStep == 4) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_5);
    if (currentStep == 5) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_6);
    if (currentStep == 6) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_7);
    if (currentStep == 7) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_8);
    if (currentStep == 8) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_9);
    if (currentStep == 9) g_lampsHelper.showLamp(LAMP_COLLECT_BONUS_ARROW);
    if (currentStep == 10) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_10);
    if (currentStep == 11) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_11);
    if (currentStep == 12) g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X);
    if (currentStep == 13) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_12);
    if (currentStep == 14) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_13);
    if (currentStep == 15) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_14);
  }
}

void Attract::startBonusLightsShow() {
  bonusLightShowEndTime_ = g_machineState.currentTime() + 1800;
  g_soundHelper.playSound(SOUND_BONUS);
  bonusShowRunning_ = true;
}

void Attract::startFeatureShow() {
  currentFeatureCycle_    = 0;
  featureShowRunning_     = true;
  featureShowStartedTime_ = g_machineState.currentTime();
}
