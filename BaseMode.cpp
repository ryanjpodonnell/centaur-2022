#include "SharedVariables.h"

Base::Base() {
}

int Base::run(byte switchHit) {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  switch (switchHit) {
    case SW_TILT:
      g_machineState.registerTiltWarning();
      break;

    case SW_SELF_TEST_SWITCH:
      returnState = MACHINE_STATE_TEST_LIGHTS;
      g_selfTestAndAudit.setLastSelfTestChangedTime(g_machineState.currentTime());
      break;

    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      g_soundHelper.playSound(SOUND_ENERGIZE_ME);
      g_machineState.writeCoinToAudit(switchHit);
      g_machineState.increaseCredits(true, 1);
      break;

    case SW_CREDIT_BUTTON:
      if (DEBUG_MESSAGES) Serial.write("Start game button pressed\n\r");

      if (g_machineState.currentBallInPlay() == 1) {
        g_machineState.increaseNumberOfPlayers();
      } else if (g_machineState.resetPlayers()) {
        BSOS_DisableSolenoidStack();
        BSOS_SetDisableFlippers(true);
        BSOS_SetDisplayCredits(g_machineState.credits());
        g_gameMode.setGameMode(GAME_MODE_RESTART_GAME);
      }

      break;

    case SW_LEFT_THUMPER_BUMPER:
    case SW_RIGHT_THUMPER_BUMPER:
      g_soundHelper.playSound(SOUND_1K_SWITCH);
      manageDefaultScoringLogic(1000);
      break;

    case SW_LEFT_SLINGSHOT:
    case SW_RIGHT_SLINGSHOT:
      g_soundHelper.playSound(SOUND_SLINGSHOT);
      manageDefaultScoringLogic(10);
      break;

    case SW_TOP_LEFT_LANE:
    case SW_TOP_MIDDLE_LANE:
    case SW_TOP_RIGHT_LANE:
      g_soundHelper.playSound(SOUND_TOP_ROLLOVER);
      manageDefaultScoringLogic(500);
      break;

    case SW_LEFT_OUTLANE:
    case SW_LEFT_RETURN_LANE:
    case SW_RIGHT_OUTLANE:
    case SW_RIGHT_RETURN_LANE:
      g_soundHelper.playSound(SOUND_BOTTOM_ROLLOVER);
      manageDefaultScoringLogic(5000);
      break;

    case SW_O_DROP_TARGET:
    case SW_R_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_S_DROP_TARGET:
      g_soundHelper.playSound(SOUND_1K_SWITCH);
      g_machineState.increaseBonus(1);
      manageDefaultScoringLogic(1000);
      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
      g_soundHelper.playSound(SOUND_SPOT_1_THROUGH_4);
      g_machineState.increaseBonus(1);
      manageDefaultScoringLogic(1000);
      break;

    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
      g_machineState.increaseBonus(g_machineState.queensChamberBonusValue());
      manageDefaultScoringLogic(g_machineState.queensChamberScoreValue());
      break;

    case SW_TOP_LEFT_LANE_RO_BUTTON:
      g_soundHelper.playSound(SOUND_5K_SWITCH);
      g_machineState.increaseBonus(1);
      manageDefaultScoringLogic(5000);
      break;

    case SW_LEFT_SIDE_RO_BUTTON:
      g_soundHelper.playSound(SOUND_QUEENS_CHAMBER_ROLLOVER);
      g_machineState.increaseBonus(1);
      manageDefaultScoringLogic(5000);
      break;

    case SW_ORBS_RIGHT_LANE_TARGET:
      g_soundHelper.playSound(SOUND_5K_SWITCH);
      manageDefaultScoringLogic(5000);
      break;

    case SW_INLINE_BACK_TARGET:
      g_soundHelper.playSound(SOUND_INLINE_BACK_TARGET);
      g_machineState.increaseBonus(g_machineState.queensChamberBonusValue());
      manageDefaultScoringLogic(g_machineState.queensChamberScoreValue());
      break;

    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      g_soundHelper.playSound(SOUND_RESET_1_THROUGH_4);
      g_machineState.increaseBonus(1);
      manageDefaultScoringLogic(5000);
      break;

    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      g_soundHelper.playSound(SOUND_SPOT_1_THROUGH_4);
      manageDefaultScoringLogic(10000);
      break;

    case SW_10_POINT_REBOUND:
      g_soundHelper.playSound(SOUND_10_POINT_REBOUND);
      manageDefaultScoringLogic(10);
      break;
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
void Base::manageDefaultScoringLogic(unsigned long value) {
  if(!g_gameMode.scoreIncreased()) g_machineState.increaseScore(value);

  g_gameMode.resetIndicatorPlayed();
  g_machineState.setMostRecentSwitchHitTime();
  if (!g_machineState.playfieldValidated()) {
    g_machineState.setPlayfieldValidated();
    g_machineState.setCurrentBallFirstSwitchHitTime();
  }
}
