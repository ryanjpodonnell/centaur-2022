#include "SharedVariables.h"

Base::Base() {
}

int Base::run(byte switchHit) {
  if (switchHit == 0xFF) handleNewMode();
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  switch (switchHit) {
    case SW_TILT:
      g_machineState.registerTiltWarning();
      break;

    case SW_SELF_TEST_SWITCH:
      returnState = MACHINE_STATE_TEST_LIGHTS;
      g_selfTestAndAudit.setLastSelfTestChangedTime(g_machineState.currentTime());
      break;

    case SW_RIGHT_FLIPPER_BUTTON:
      if (g_gameMode.firstSwitchHitTime() != 0) {
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
      handleDefaultScoringLogic();
      break;

    case SW_TOP_LEFT_LANE:
    case SW_TOP_MIDDLE_LANE:
    case SW_TOP_RIGHT_LANE:
      g_machineState.registerTopRollover(switchHit);
      g_machineState.updateTopRolloverLamps();
      handleDefaultScoringLogic();
      break;

    case SW_O_DROP_TARGET:
    case SW_R_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_S_DROP_TARGET:
      g_machineState.registerDropTarget(switchHit);
      g_machineState.updateOrbsDropTargetLamps();

      if (g_machineState.orbsDropTargetsCompleted()) {
        g_machineState.qualifyMode();
        g_machineState.updateCaptiveOrbsLamps();
      }

      handleDefaultScoringLogic();
      break;

    case SW_ORBS_RIGHT_LANE_TARGET:
      if (g_machineState.orbsDropTargetsCompleted()) {
        g_machineState.resetDropTargets();
        g_machineState.updateOrbsDropTargetLamps();
        g_machineState.startQualifiedMode();
        g_machineState.updateSelectedMode();
        g_machineState.updateCaptiveOrbsLamps();
      }
      handleDefaultScoringLogic();
      break;

    case SW_LEFT_THUMPER_BUMPER:
    case SW_RIGHT_THUMPER_BUMPER:
      g_machineState.rotateQualifiedMode();
      g_machineState.updateCaptiveOrbsLamps();
      handleDefaultScoringLogic();
      break;

    case SW_10_POINT_REBOUND:
    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLANE_BACK_TARGET:
    case SW_LEFT_SIDE_RO_BUTTON:
    case SW_LEFT_SLINGSHOT:
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
    case SW_RIGHT_SLINGSHOT:
    case SW_TOP_LEFT_LANE_RO_BUTTON:
    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      handleDefaultScoringLogic();
      break;

    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      g_machineState.writeCoinToAudit(switchHit);
      g_machineState.increaseCredits(true, 1);
      break;

    case SW_CREDIT_BUTTON:
      if (DEBUG_MESSAGES) Serial.write("Start game button pressed\n\r");

      if (g_machineState.currentBallInPlay() == 1) {
        g_machineState.incrementNumberOfPlayers();
      } else if (g_machineState.resetPlayers()) {
        returnState = MACHINE_STATE_INIT_GAMEPLAY;
      }

      break;
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
void Base::handleDefaultScoringLogic() {
      if(!g_gameMode.scoreIncreased()) g_machineState.increaseScore(10);

      if (g_gameMode.firstSwitchHitTime() == 0) {
        g_gameMode.setFirstSwitchHitTime(g_machineState.currentTime());

        g_machineState.updateGuardianRolloverLamps();
        g_machineState.updateTopRolloverLamps();
        g_machineState.updateOrbsDropTargetLamps();
        g_machineState.updateCaptiveOrbsLamps();
      }

      g_machineState.incrementCurrentBallSwitchHitCounter();
      if (g_machineState.currentBallSwitchHitCounter() % 5 == 0) {
        g_machineState.increaseBonus(1);
      }
}

void Base::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Base Mode\n\r");
}
