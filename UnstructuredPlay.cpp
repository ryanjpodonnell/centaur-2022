#include "SharedVariables.h"

UnstructuredPlay::UnstructuredPlay() {
}

byte UnstructuredPlay::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
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
        g_gameMode.startHurryUp(g_machineState.queensChamberHurryUpValue(), 10);
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

        g_machineState.resetOrbsDropTargets(true, true);
      }

      g_machineState.updateOrbsDropTargetsLamps();
      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
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

          if (g_machineState.allowRightDropTargetProgress()) g_machineState.resetRightDropTargets(true, true);
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
        if (g_machineState.hurryUpActivated()) g_gameMode.endHurryUp();

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

      if (g_machineState.hurryUpActivated()) {
        g_gameMode.endHurryUp();
        g_machineState.increaseBonus(g_machineState.queensChamberBonusValue(), true);
        g_machineState.increaseScore(g_machineState.hurryUpValue(),            true);
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

/*********************************************************************
    Private
*********************************************************************/
void UnstructuredPlay::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Unstructured Play Mode\n\r");
  rerunOrbsSwitchHit_   = 0xFF;
  rerunOrbsSwitchTime_  = 0;
  rerunOrbsSwitch_      = false;
  rerunRightSwitchHit_  = 0xFF;
  rerunRightSwitchTime_ = 0;
  rerunRightSwitch_     = false;

  g_machineState.showAllPlayerLamps();
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
