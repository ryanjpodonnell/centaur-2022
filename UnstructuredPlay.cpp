#include "SharedVariables.h"

UnstructuredPlay::UnstructuredPlay() {
  currentFlashCycle_ = 0;
  lastFlash_         = 0;
}

byte UnstructuredPlay::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  int returnState = GAME_MODE_UNSTRUCTURED_PLAY;

  if (g_machineState.hurryUpActivated()) manageHurryUp();
  if (g_bonusLightShow.running())        g_bonusLightShow.run();

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
        startHurryUp(100000, 10);
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
      g_machineState.registerOrbsDropTarget(switchHit);
      g_machineState.updateOrbsDropTargetLamps();

      if (g_machineState.orbsDropTargetsCompleted() && g_machineState.qualifyMode()) {
        g_machineState.updateCaptiveOrbsLamps();
        g_machineState.updateRightOrbsReleaseLamp();
      }

      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
      if (g_bonusLightShow.running()) g_bonusLightShow.end();

      g_machineState.registerRightDropTarget(switchHit);
      g_machineState.updateRightDropTargetLamps();
      g_machineState.updateRightDropTargetSpotLamp();

      if (g_machineState.rightDropTargetsCompleted() && g_machineState.qualifyModeMultiplier()) {
        g_machineState.updateRightDropTargetResetLamp();
        g_bonusLightShow.start(BONUS_LIGHT_SHOW_RESET_1_THROUGH_4_ARROW);
      }

      break;

    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      if (g_machineState.modeMultiplierQualified()) {
        if (g_bonusLightShow.running()) g_bonusLightShow.end();

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
        if (g_machineState.hurryUpActivated()) endHurryUp();

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

    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLINE_BACK_TARGET:
      if (g_machineState.hurryUpActivated()) {
        endHurryUp();
        g_machineState.increaseScore(g_machineState.hurryUpValue());
      } else {
        if ((g_machineState.queensChamberBonusValue() < MAX_QUEENS_CHAMBER_BONUS_VALUE) &&
            (g_machineState.queensChamberScoreValue() < MAX_QUEENS_CHAMBER_SCORE_VALUE)
           ) {
          g_machineState.increaseQueensChamberBonusValue();
          g_machineState.increaseQueensChamberScoreValue();
          g_machineState.updateQueensChamberLamps();
        }
      }

      break;

    case SW_OUTHOLE:
      if (g_machineState.hurryUpActivated()) endHurryUp();
      break;

    case 0xFF:
      break;
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
void UnstructuredPlay::endHurryUp() {
  if (DEBUG_MESSAGES) Serial.write("Hurry Up Ended\n\r");

  g_machineState.setHurryUpActivated(false);
  g_displayHelper.showPlayerScores(0xFF);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);
  g_machineState.resetGuardianRollovers();
  g_machineState.updateGuardianRolloverLamps();
}

void UnstructuredPlay::manageHurryUp() {
  if (!g_machineState.hurryUpActivated()) return;
  updateHurryUpValue();

  unsigned long seed = g_machineState.currentTime() / 50; // .05 seconds
  if (seed != lastFlash_) {
    lastFlash_ = seed;
    g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);

    byte currentStep = seed % 5;
    if (currentStep == 0) g_lampsHelper.showLamp(LAMP_10_CHAMBER);
    if (currentStep == 1) g_lampsHelper.showLamp(LAMP_20_CHAMBER);
    if (currentStep == 2) g_lampsHelper.showLamp(LAMP_30_CHAMBER);
    if (currentStep == 3) g_lampsHelper.showLamp(LAMP_40_CHAMBER);
    if (currentStep == 4) g_lampsHelper.showLamp(LAMP_50_CHAMBER);
  }
}

void UnstructuredPlay::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Unstructured Play Mode\n\r");
  g_machineState.showAllPlayerLamps();
}

void UnstructuredPlay::startHurryUp(unsigned long value, int seconds) {
  if (DEBUG_MESSAGES) Serial.write("Hurry Up Started\n\r");
  g_soundHelper.playSoundWithoutInterruptions(SOUND_SIREN_2);
  g_machineState.setHurryUpActivated(true);
  g_machineState.setHurryUpValue(value);
  hurryUpInitialValue_        = value;
  hurryUpStartedTime_         = g_machineState.currentTime();
  hurryUpEndTime_             = g_machineState.currentTime() + (1000 * seconds) + HURRY_UP_GRACE_PERIOD;
  hurryUpValuePerMillisecond_ = value / (1000 * seconds);
}

void UnstructuredPlay::updateHurryUpValue() {
  unsigned long timeSinceHurryUpStarted = g_machineState.currentTime() - hurryUpStartedTime_;
  if (timeSinceHurryUpStarted < HURRY_UP_GRACE_PERIOD) return;

  unsigned long value = hurryUpValuePerMillisecond_ * (timeSinceHurryUpStarted - HURRY_UP_GRACE_PERIOD);
  g_machineState.setHurryUpValue(hurryUpInitialValue_ - value);

  if (g_machineState.currentTime() >= hurryUpEndTime_) {
    endHurryUp();
  }
}
