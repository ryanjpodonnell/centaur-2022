#include "SharedVariables.h"

UnstructuredPlay::UnstructuredPlay() {
}

byte UnstructuredPlay::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) handleNewMode();
  int returnState = GAME_MODE_UNSTRUCTURED_PLAY;

  switch (switchHit) {
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
      break;

    case SW_TOP_LEFT_LANE:
    case SW_TOP_MIDDLE_LANE:
    case SW_TOP_RIGHT_LANE:
      g_machineState.registerTopRollover(switchHit);
      g_machineState.updateTopRolloverLamps();
      break;

    case SW_O_DROP_TARGET:
    case SW_R_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_S_DROP_TARGET:
      g_machineState.registerDropTarget(switchHit);
      g_machineState.updateOrbsDropTargetLamps();

      if (g_machineState.orbsDropTargetsCompleted()) {
        if (!g_machineState.allModesQualified()) {
          g_machineState.qualifyMode();
        }

        g_machineState.updateCaptiveOrbsLamps();
      }

      break;

    case SW_ORBS_RIGHT_LANE_TARGET:
      if (g_machineState.orbsDropTargetsCompleted()) {
        g_machineState.resetDropTargets();
        g_machineState.updateOrbsDropTargetLamps();

        if (g_machineState.anyModeQualified()) {
          returnState = g_machineState.startQualifiedMode();

          if (!g_machineState.allModesQualified()) {
            g_machineState.updateSelectedMode();
          }

          g_machineState.updateCaptiveOrbsLamps();
        }
      }
      break;

    case SW_LEFT_THUMPER_BUMPER:
    case SW_RIGHT_THUMPER_BUMPER:
      if (g_machineState.anyModeQualified()) {
        g_machineState.rotateQualifiedMode();
        g_machineState.updateCaptiveOrbsLamps();
      }
      break;
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
void UnstructuredPlay::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Unstructured Play Mode\n\r");
  g_machineState.updateGuardianRolloverLamps();
  g_machineState.updateTopRolloverLamps();
  g_machineState.updateOrbsDropTargetLamps();
  g_machineState.updateCaptiveOrbsLamps();
}
