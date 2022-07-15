#include "SharedVariables.h"

DestroyCentaur::DestroyCentaur() {}

byte DestroyCentaur::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageModeLamps();

  switch(switchHit) {
    case SW_1ST_INLINE_DROP_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);

      g_soundHelper.playSoundWithoutInterruptions(SOUND_HA_HA_HA);
      g_gameMode.setOverrideSound(true);
      break;

    case SW_2ND_INLINE_DROP_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);

      g_soundHelper.playSoundWithoutInterruptions(SOUND_TRY);
      g_gameMode.setOverrideSound(true);
      break;

    case SW_3RD_INLINE_DROP_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);

      g_soundHelper.playSoundWithoutInterruptions(SOUND_BAD_MOVE_HUMAN);
      g_gameMode.setOverrideSound(true);
      break;

    case SW_4TH_INLINE_DROP_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);

      g_soundHelper.playSoundWithoutInterruptions(SOUND_CHALLENGE_ME);
      g_gameMode.setOverrideSound(true);
      break;

    case SW_INLINE_BACK_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);
      g_machineState.increaseScore(jackpotValue_, true);
      g_gameMode.setScoreIncreased(true);

      g_soundHelper.playSoundWithoutInterruptions(SOUND_CRASH);
      g_gameMode.setOverrideSound(true);
      return endMode();
  }

  return GAME_MODE_DESTROY_CENTAUR;
}

unsigned long DestroyCentaur::jackpotValue() {
  return jackpotValue_;
}

/*********************************************************************
    Private
*********************************************************************/
byte DestroyCentaur::endMode() {
  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN, SOUND_ALARM);
  g_displayHelper.showPlayerScores(0xFF);

  g_machineState.unqualifyAllModes();
  g_lampsHelper.hideAllPlayfieldLamps();
  g_lampsHelper.showLamps(LAMP_COLLECTION_GENERAL_ILLUMINATION);

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void DestroyCentaur::manageModeLamps() {
  unsigned long seed = g_machineState.currentTime() / 100;
  if (seed != lastFlash_) {
    lastFlash_ = seed;
    byte currentStep = seed % 24;

    if (currentStep == 0) {
      g_lampsHelper.hideAllPlayfieldLamps();
    } else if (currentStep == 1) {
      g_lampsHelper.showLamp(LAMP_RELEASE_ORB);
    } else if (currentStep == 2) {
      g_lampsHelper.showLamp(LAMP_1_CAPTIVE_ORBS);
      g_lampsHelper.showLamp(LAMP_2_CAPTIVE_ORBS);
      g_lampsHelper.showLamp(LAMP_3_CAPTIVE_ORBS);
      g_lampsHelper.showLamp(LAMP_4_CAPTIVE_ORBS);
      g_lampsHelper.showLamp(LAMP_LEFT_SLINGSHOT_GI);
      g_lampsHelper.showLamp(LAMP_RIGHT_OUT_ROLLOVER);
      g_lampsHelper.showLamp(LAMP_RIGHT_RETURN_ROLLOVER);
      g_lampsHelper.showLamp(LAMP_RIGHT_SLINGSHOT_GI);
    } else if (currentStep == 3) {
      g_lampsHelper.showLamp(LAMP_4K_BONUS);
      g_lampsHelper.showLamp(LAMP_5K_BONUS);
      g_lampsHelper.showLamp(LAMP_6K_BONUS);
    } else if (currentStep == 4) {
      g_lampsHelper.hideLamp(LAMP_RELEASE_ORB);
      g_lampsHelper.showLamp(LAMP_3K_BONUS);
      g_lampsHelper.showLamp(LAMP_5X_BONUS);
      g_lampsHelper.showLamp(LAMP_60K_BONUS);
      g_lampsHelper.showLamp(LAMP_7K_BONUS);
      g_lampsHelper.showLamp(LAMP_LEFT_OUT_ROLLOVER);
      g_lampsHelper.showLamp(LAMP_LEFT_RETURN_ROLLOVER);
    } else if (currentStep == 5) {
      g_lampsHelper.hideLamp(LAMP_1_CAPTIVE_ORBS);
      g_lampsHelper.hideLamp(LAMP_2_CAPTIVE_ORBS);
      g_lampsHelper.hideLamp(LAMP_3_CAPTIVE_ORBS);
      g_lampsHelper.hideLamp(LAMP_4_CAPTIVE_ORBS);
      g_lampsHelper.hideLamp(LAMP_LEFT_SLINGSHOT_GI);
      g_lampsHelper.hideLamp(LAMP_RIGHT_OUT_ROLLOVER);
      g_lampsHelper.hideLamp(LAMP_RIGHT_RETURN_ROLLOVER);
      g_lampsHelper.hideLamp(LAMP_RIGHT_SLINGSHOT_GI);
      g_lampsHelper.showLamp(LAMP_3X_BONUS);
      g_lampsHelper.showLamp(LAMP_40K_BONUS);
      g_lampsHelper.showLamp(LAMP_4X_BONUS);
      g_lampsHelper.showLamp(LAMP_8K_BONUS);
    } else if (currentStep == 6) {
      g_lampsHelper.hideLamp(LAMP_4K_BONUS);
      g_lampsHelper.hideLamp(LAMP_5K_BONUS);
      g_lampsHelper.hideLamp(LAMP_6K_BONUS);
      g_lampsHelper.showLamp(LAMP_2K_BONUS);
      g_lampsHelper.showLamp(LAMP_2X_BONUS);
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
    } else if (currentStep == 7) {
      g_lampsHelper.hideLamp(LAMP_3K_BONUS);
      g_lampsHelper.hideLamp(LAMP_5X_BONUS);
      g_lampsHelper.hideLamp(LAMP_60K_BONUS);
      g_lampsHelper.hideLamp(LAMP_7K_BONUS);
      g_lampsHelper.hideLamp(LAMP_LEFT_OUT_ROLLOVER);
      g_lampsHelper.hideLamp(LAMP_LEFT_RETURN_ROLLOVER);
      g_lampsHelper.showLamp(LAMP_1K_BONUS);
      g_lampsHelper.showLamp(LAMP_9K_BONUS);
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
    } else if (currentStep == 8) {
      g_lampsHelper.hideLamp(LAMP_3X_BONUS);
      g_lampsHelper.hideLamp(LAMP_40K_BONUS);
      g_lampsHelper.hideLamp(LAMP_4X_BONUS);
      g_lampsHelper.hideLamp(LAMP_8K_BONUS);
      g_lampsHelper.showLamp(LAMP_10K_BONUS);
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
    } else if (currentStep == 9) {
      g_lampsHelper.hideLamp(LAMP_2K_BONUS);
      g_lampsHelper.hideLamp(LAMP_2X_BONUS);
      g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW);
      g_lampsHelper.showLamp(LAMP_10_CHAMBER);
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
      g_lampsHelper.showLamp(LAMP_RIGHT_LANE_RELEASE_ORBS);
    } else if (currentStep == 10) {
      g_lampsHelper.hideLamp(LAMP_1K_BONUS);
      g_lampsHelper.hideLamp(LAMP_9K_BONUS);
      g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW);
      g_lampsHelper.showLamp(LAMP_20_CHAMBER);
      g_lampsHelper.showLamp(LAMP_COLLECT_BONUS_ARROW);
      g_lampsHelper.showLamp(LAMP_ORBS_50K);
      g_lampsHelper.showLamp(LAMP_ORBS_SPECIAL);
      g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW);
    } else if (currentStep == 11) {
      g_lampsHelper.hideLamp(LAMP_10K_BONUS);
      g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW);
      g_lampsHelper.showLamp(LAMP_30_CHAMBER);
      g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW);
      g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW);
      g_lampsHelper.showLamp(LAMP_RIGHT_LANE_2X);
      g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW);
      g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW);
    } else if (currentStep == 12) {
      g_lampsHelper.hideLamp(LAMP_10_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW);
      g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_RELEASE_ORBS);
      g_lampsHelper.showLamp(LAMP_40_CHAMBER);
      g_lampsHelper.showLamp(LAMP_RIGHT_LANE_3X);
    } else if (currentStep == 13) {
      g_lampsHelper.hideLamp(LAMP_20_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_COLLECT_BONUS_ARROW);
      g_lampsHelper.hideLamp(LAMP_ORBS_50K);
      g_lampsHelper.hideLamp(LAMP_ORBS_SPECIAL);
      g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
      g_lampsHelper.showLamp(LAMP_50_CHAMBER);
      g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X);
    } else if (currentStep == 14) {
      g_lampsHelper.hideLamp(LAMP_30_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_B_DROP_TARGET_ARROW);
      g_lampsHelper.hideLamp(LAMP_O_DROP_TARGET_ARROW);
      g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_2X);
      g_lampsHelper.hideLamp(LAMP_R_DROP_TARGET_ARROW);
      g_lampsHelper.hideLamp(LAMP_S_DROP_TARGET_ARROW);
      g_lampsHelper.showLamp(LAMP_RIGHT_LANE_5X);
    } else if (currentStep == 15) {
      g_lampsHelper.hideLamp(LAMP_40_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_3X);
      g_lampsHelper.showLamp(LAMP_RIGHT_THUMPER_BUMPER);
      g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);
    } else if (currentStep == 16) {
      g_lampsHelper.hideLamp(LAMP_50_CHAMBER);
      g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_4X);
      g_lampsHelper.showLamp(LAMP_LEFT_THUMPER_BUMPER);
      g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_1);
      g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER);
    } else if (currentStep == 17) {
      g_lampsHelper.hideLamp(LAMP_RIGHT_LANE_5X);
      g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_2);
      g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER);
    } else if (currentStep == 18) {
      g_lampsHelper.hideLamp(LAMP_RIGHT_THUMPER_BUMPER);
      g_lampsHelper.hideLamp(LAMP_SPOT_1_THROUGH_4);
      g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_3);
      g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER);
    } else if (currentStep == 19) {
      g_lampsHelper.hideLamp(LAMP_LEFT_THUMPER_BUMPER);
      g_lampsHelper.hideLamp(LAMP_QUEENS_CHAMBER_GI_1);
      g_lampsHelper.hideLamp(LAMP_TOP_RIGHT_ROLLOVER);
      g_lampsHelper.showLamp(LAMP_QUEENS_CHAMBER_GI_4);
      g_lampsHelper.showLamp(LAMP_CHAMBERS_SPECIAL);
    } else if (currentStep == 20) {
      g_lampsHelper.hideLamp(LAMP_QUEENS_CHAMBER_GI_2);
      g_lampsHelper.hideLamp(LAMP_TOP_MIDDLE_ROLLOVER);
      g_lampsHelper.showLamp(LAMP_CHAMBERS_RELEASE);
    } else if (currentStep == 21) {
      g_lampsHelper.hideLamp(LAMP_QUEENS_CHAMBER_GI_3);
      g_lampsHelper.hideLamp(LAMP_TOP_LEFT_ROLLOVER);
    } else if (currentStep == 22) {
      g_lampsHelper.hideLamp(LAMP_QUEENS_CHAMBER_GI_4);
      g_lampsHelper.hideLamp(LAMP_CHAMBERS_SPECIAL);
    } else if (currentStep == 23) {
      g_lampsHelper.hideLamp(LAMP_CHAMBERS_RELEASE);
    }
  }
}

void DestroyCentaur::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Destroy Centaur Mode\n\r");

  jackpotValue_ = 1;
  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.hideLamps(LAMP_COLLECTION_GENERAL_ILLUMINATION);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);
}
