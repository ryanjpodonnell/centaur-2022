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
        g_machineState.showPlayerLamps();
      }
      break;
    case SW_LEFT_OUTLANE:
    case SW_LEFT_RETURN_LANE:
    case SW_RIGHT_OUTLANE:
    case SW_RIGHT_RETURN_LANE:
    case SW_TOP_LEFT_LANE:
    case SW_TOP_MIDDLE_LANE:
    case SW_TOP_RIGHT_LANE:
      g_machineState.registerRollover(switchHit);
      g_machineState.showPlayerLamps();
    case SW_10_POINT_REBOUND:
    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_INLANE_BACK_TARGET:
    case SW_LEFT_SIDE_RO_BUTTON:
    case SW_LEFT_SLINGSHOT:
    case SW_LEFT_THUMPER_BUMPER:
    case SW_ORBS_RIGHT_LANE_TARGET:
    case SW_O_DROP_TARGET:
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
    case SW_RIGHT_SLINGSHOT:
    case SW_RIGHT_THUMPER_BUMPER:
    case SW_R_DROP_TARGET:
    case SW_S_DROP_TARGET:
    case SW_TOP_LEFT_LANE_RO_BUTTON:
    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      if(!g_gameMode.scoreIncreased()) g_machineState.increaseScore(10);
      if (g_gameMode.firstSwitchHitTime() == 0) g_gameMode.setFirstSwitchHitTime(g_machineState.currentTime());
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      g_machineState.writeCoinToAudit(switchHit);
      g_machineState.increaseCredits(true, 1);
      break;
    case SW_CREDIT_BUTTON:
      if (DEBUG_MESSAGES) {
        Serial.write("Start game button pressed\n\r");
      }

      if (g_machineState.currentBallInPlay() == 1) {
        g_machineState.incrementNumberOfPlayers();
      } else if (g_machineState.resetPlayers()) {
        returnState = MACHINE_STATE_INIT_GAMEPLAY;
      }
      break;
  }

  return returnState;
}

void Base::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Base Mode\n\r");
}
