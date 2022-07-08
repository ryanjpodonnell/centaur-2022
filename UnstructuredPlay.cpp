#include "SharedVariables.h"

UnstructuredPlay::UnstructuredPlay() {}

boolean UnstructuredPlay::hurryUpActivated() {
  return hurryUpActivated_;
}

byte UnstructuredPlay::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageHurryUp();
  managePlayerBonusLamps();
  manageShotIndicatorShow();

  int returnState = GAME_MODE_UNSTRUCTURED_PLAY;
  byte multiplier = 0xFF;

  switch (switchHit) {
    case SW_RIGHT_FLIPPER_BUTTON:
      if (g_machineState.playfieldValidated()) {
        g_machineState.rotatePlayerLamps();
        g_machineState.updateGuardianRolloverLamps();
        g_machineState.updateTopRolloverLamps();
      }
      break;

    case SW_LEFT_OUTLANE:
    case SW_LEFT_RETURN_LANE:
    case SW_RIGHT_OUTLANE:
    case SW_RIGHT_RETURN_LANE:
      g_machineState.registerGuardianRollover(switchHit);
      g_machineState.updateGuardianRolloverLamps();

      if (g_machineState.guardianRolloversCompleted()) {
        g_machineState.resetGuardianRollovers();
        g_machineState.updateGuardianRolloverLamps();
        startHurryUp(g_machineState.queensChamberHurryUpValue());
      }
      break;

    case SW_TOP_LEFT_LANE:
    case SW_TOP_MIDDLE_LANE:
    case SW_TOP_RIGHT_LANE:
      g_machineState.registerTopRollover(switchHit);
      g_machineState.updateTopRolloverLamps();

      if (g_machineState.topRolloversCompleted() &&
          g_machineState.bonusMultiplier() < MAX_BONUS_MULTIPLIER) {
        g_machineState.resetTopRollovers();
        g_machineState.updateTopRolloverLamps();
        g_machineState.increaseBonusMultiplier();
      }
      break;

    case SW_O_DROP_TARGET:
    case SW_R_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_S_DROP_TARGET:
      indicatorPlayed_ = false;
      if (g_bonusLightShow.running()) g_bonusLightShow.end();

      if (g_machineState.activeOrbsDropTarget() != 0xFF &&
          g_machineState.activeOrbsDropTarget() != switchHit &&
          rerunOrbsSwitch_ == false) {
        rerunOrbsSwitchHit_  = switchHit;
        rerunOrbsSwitchTime_ = g_machineState.currentTime() + 110;
        g_gameMode.setScoreIncreased(true);
      } else if (rerunOrbsSwitch_ == true) {
        rerunOrbsSwitchHit_  = 0xFF;
        rerunOrbsSwitchTime_ = 0;
        rerunOrbsSwitch_     = false;
        g_machineState.registerOrbsDropTarget(switchHit);
        g_machineState.manageOrbsDropTargetScoring(switchHit);
        g_machineState.increaseScore(1000);
        g_gameMode.setScoreIncreased(true);
      } else {
        g_machineState.registerOrbsDropTarget(switchHit);
        g_machineState.manageOrbsDropTargetScoring(switchHit);
        g_machineState.increaseScore(1000);
        g_gameMode.setScoreIncreased(true);
      }

      if (g_machineState.orbsDropTargetsCompleted()) {
        if (g_machineState.orbsDropTargetsCompletedInOrder()) {
          if (DEBUG_MESSAGES) Serial.write("Additional Ball Added\n\r");
          g_machineState.launchBallIntoPlay();
          g_machineState.increaseNumberOfBallsInPlay();
          g_machineState.updateScoreMultiplierLamps();
        }

        if (!g_machineState.anyModeQualified()) {
          g_machineState.qualifyMode();
          g_machineState.updateCaptiveOrbsLamps();
          g_machineState.updateRightOrbsReleaseLamp();
        }

        g_machineState.resetOrbsDropTargets(true, true, g_machineState.currentTime() + 250);
      }

      g_machineState.updateOrbsDropTargetsLamps();
      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
      indicatorPlayed_ = false;
      if (g_bonusLightShow.running()) g_bonusLightShow.end();

      if (g_machineState.activeRightDropTarget() != 0xFF &&
          g_machineState.activeRightDropTarget() != switchHit &&
          rerunRightSwitch_ == false) {
        rerunRightSwitchHit_  = switchHit;
        rerunRightSwitchTime_ = g_machineState.currentTime() + 110;
        g_gameMode.setScoreIncreased(true);
      } else if (rerunRightSwitch_ == true) {
        rerunRightSwitchHit_  = 0xFF;
        rerunRightSwitchTime_ = 0;
        rerunRightSwitch_     = false;
        g_machineState.registerRightDropTarget(switchHit);
        g_machineState.manageRightDropTargetScoring(switchHit);
        g_machineState.increaseScore(g_machineState.rightDropTargetsScoreValue());
        g_gameMode.setScoreIncreased(true);
      } else {
        g_machineState.registerRightDropTarget(switchHit);
        g_machineState.manageRightDropTargetScoring(switchHit);
        g_machineState.increaseScore(g_machineState.rightDropTargetsScoreValue());
        g_gameMode.setScoreIncreased(true);
      }

      if (g_machineState.rightDropTargetsCompleted()) {
        if (g_machineState.rightDropTargetsCompletedInOrder()) {
          if (DEBUG_MESSAGES) Serial.write("Additional Ball Qualified\n\r");
          g_machineState.increaseQualifiedScoreMultiplier();
          g_machineState.updateScoreMultiplierLamps();

          if (g_machineState.allowRightDropTargetProgress()) g_machineState.resetRightDropTargets(true, true, g_machineState.currentTime() + 250);
        } else {
          g_machineState.qualifyRightDropTargetsReset();
          g_bonusLightShow.start(BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW);
        }
      }

      g_machineState.updateRightDropTargetsLamps();
      g_machineState.updateRightDropTargetsResetLamp();
      g_machineState.updateRightDropTargetsSpotLamp();
      break;

    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      if (g_machineState.rightDropTargetsResetQualified()) {
        if (g_bonusLightShow.running()) g_bonusLightShow.end();

        g_machineState.unqualifyRightDropTargetsReset();
        g_machineState.increaseQualifiedScoreMultiplier();

        if (g_machineState.allowRightDropTargetProgress()) g_machineState.resetRightDropTargets(true, true);

        g_machineState.updateRightDropTargetsLamps();
        g_machineState.updateRightDropTargetsResetLamp();
        g_machineState.updateRightDropTargetsSpotLamp();
        g_machineState.updateScoreMultiplierLamps();
      }
      break;

    case SW_ORBS_RIGHT_LANE_TARGET:
      if (g_machineState.anyModeQualified()) {
        if (hurryUpActivated_) endHurryUp();

        returnState = g_machineState.startQualifiedMode();
        g_machineState.updateScoreMultiplierLamps();
        g_machineState.updateRightOrbsReleaseLamp();
      }
      break;

    case SW_LEFT_THUMPER_BUMPER:
    case SW_RIGHT_THUMPER_BUMPER:
      if (g_machineState.anyModeQualified()) {
        g_machineState.rotateQualifiedMode();
        g_machineState.updateCaptiveOrbsLamps();
      }

      break;

    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      if (g_machineState.allowRightDropTargetProgress() &&
          !g_machineState.rightDropTargetsCompleted()) {
        g_machineState.spotRightDropTarget();
      }

      break;

    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLINE_BACK_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);
      g_machineState.manageInlineTargetScoring(switchHit);
      g_machineState.updateQueensChamberLamps();

      if (hurryUpActivated_) {
        endHurryUp();
        g_machineState.increaseBonus(g_machineState.queensChamberBonusValue(), true);
        g_machineState.increaseScore(hurryUpValue_,                            true);
      } else {
        g_machineState.increaseBonus(g_machineState.queensChamberBonusValue(), true);
        g_machineState.increaseScore(g_machineState.queensChamberScoreValue(), true);
      }

      g_gameMode.setBonusIncreased(true);
      g_gameMode.setScoreIncreased(true);

      break;
  }

  manageSwitchReruns();
  return returnState;
}

unsigned long UnstructuredPlay::hurryUpValue() {
  return hurryUpValue_;
}

void UnstructuredPlay::endModeViaBallEnded() {
  if (hurryUpActivated_) endHurryUp();
}

/*********************************************************************
    Private
*********************************************************************/
void UnstructuredPlay::endHurryUp() {
  if (DEBUG_MESSAGES) Serial.write("Hurry Up Ended\n\r");

  hurryUpActivated_ = false;
  g_displayHelper.showPlayerScores(0xFF);

  g_lampsHelper.showLamps(LAMP_COLLECTION_QUEENS_CHAMBER_GI);
  g_machineState.updateQueensChamberLamps();
}

void UnstructuredPlay::manageHurryUp() {
  if (!hurryUpActivated_) return;

  updateHurryUpValue();
  updateHurryUpLamps();
}

void UnstructuredPlay::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Unstructured Play Mode\n\r");
  hurryUpActivated_     = false;
  indicatorPlayed_      = false;
  rerunOrbsSwitchHit_   = 0xFF;
  rerunOrbsSwitchTime_  = 0;
  rerunOrbsSwitch_      = false;
  rerunRightSwitchHit_  = 0xFF;
  rerunRightSwitchTime_ = 0;
  rerunRightSwitch_     = false;

  g_machineState.showAllPlayerLamps();
}

void UnstructuredPlay::managePlayerBonusLamps() {
  if (g_bonusLightShow.running()) return;

  g_machineState.updateBonusLamps();
}

void UnstructuredPlay::manageSwitchReruns() {
  if (rerunOrbsSwitchHit_ != 0xFF && rerunOrbsSwitchTime_ < g_machineState.currentTime()) {
    if (DEBUG_MESSAGES) Serial.write("Rerunning Orbs Switch Hit\n\r");
    BSOS_PushToSwitchStack(rerunOrbsSwitchHit_);
    rerunOrbsSwitch_ = true;
  }

  if (rerunRightSwitchHit_ != 0xFF && rerunRightSwitchTime_ < g_machineState.currentTime()) {
    if (DEBUG_MESSAGES) Serial.write("Rerunning Right Switch Hit\n\r");
    BSOS_PushToSwitchStack(rerunRightSwitchHit_);
    rerunRightSwitch_ = true;
  }
}

void UnstructuredPlay::manageShotIndicatorShow() {
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

void UnstructuredPlay::startHurryUp(unsigned long value) {
  if (DEBUG_MESSAGES) Serial.write("Hurry Up Started\n\r");

  g_soundHelper.playSoundWithoutInterruptions(SOUND_SIREN_2);
  hurryUpActivated_           = true;
  hurryUpValue_               = value;
  hurryUpInitialValue_        = value;
  hurryUpStartedTime_         = g_machineState.currentTime();
  hurryUpEndTime_             = g_machineState.currentTime() + HURRY_UP_LENGTH + HURRY_UP_GRACE_PERIOD;
  hurryUpValuePerMillisecond_ = value / HURRY_UP_LENGTH;
}

void UnstructuredPlay::updateHurryUpLamps() {
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

void UnstructuredPlay::updateHurryUpValue() {
  unsigned long timeSinceHurryUpStarted = g_machineState.currentTime() - hurryUpStartedTime_;
  if (timeSinceHurryUpStarted < HURRY_UP_GRACE_PERIOD) return;

  unsigned long value = hurryUpValuePerMillisecond_ * (timeSinceHurryUpStarted - HURRY_UP_GRACE_PERIOD);
  hurryUpValue_ = hurryUpInitialValue_ - value;

  if (g_machineState.currentTime() >= hurryUpEndTime_) {
    endHurryUp();
  }
}
