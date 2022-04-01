#include "SharedVariables.h"

UnstructuredPlay::UnstructuredPlay() {
}

byte UnstructuredPlay::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) handleNewMode();
  int returnState = GAME_MODE_UNSTRUCTURED_PLAY;

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
      g_machineState.registerOrbsDropTarget(switchHit);
      g_machineState.updateOrbsDropTargetLamps();

      if (g_machineState.orbsDropTargetsCompleted() && g_machineState.qualifyMode()) {
        g_machineState.flashOrbsDropTargetLamps();
        g_machineState.updateCaptiveOrbsLamps();
        g_machineState.updateRightOrbsReleaseLamp();
      }

      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
      if (g_machineState.allowRightDropTargetProgress()) {
        g_machineState.registerRightDropTarget(switchHit);
        g_machineState.updateRightDropTargetLamps();
        g_machineState.updateRightDropTargetSpotLamp();

        if (g_machineState.rightDropTargetsCompleted() && g_machineState.qualifyModeMultiplier()) {
          g_machineState.flashRightDropTargetLamps();
          g_machineState.updateRightDropTargetResetLamp();
        }
      } else {
        g_machineState.resetRightDropTargets(true);
      }

      break;

    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      if (g_machineState.modeMultiplierQualified()) {
        g_machineState.increaseModeMultiplier();
        g_machineState.updateModeMultiplierLamps();

        g_machineState.resetRightDropTargets(true);
        g_machineState.updateRightDropTargetLamps();
        g_machineState.updateRightDropTargetResetLamp();
        g_machineState.updateRightDropTargetSpotLamp();
      }

      break;

    case SW_ORBS_RIGHT_LANE_TARGET:
      if (g_machineState.anyModeQualified()) {
        g_machineState.resetOrbsDropTargets(false);
        returnState = g_machineState.startQualifiedMode();

        g_machineState.updateModeMultiplierLamps();
        g_machineState.updateRightOrbsReleaseLamp();
        g_machineState.updateSelectedMode();
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
      if (g_machineState.allowRightDropTargetProgress() && !g_machineState.rightDropTargetsCompleted()) {
        g_machineState.registerRightDropTarget(0xFF);
      }
      break;

    case 0xFF:
      break;
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
void UnstructuredPlay::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Unstructured Play Mode\n\r");
  g_machineState.showAllPlayerLamps();
}
