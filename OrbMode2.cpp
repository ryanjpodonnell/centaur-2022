#include "SharedVariables.h"

OrbMode2::OrbMode2() {}

byte OrbMode2::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageDropsReset();
  manageModeLamps();
  manageTimeRemaining();

  switch(switchHit) {
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      if (allowAddTime_) {
        totalTime_ += 5;

        if (totalTime_ >= ORB_MODE_2_MAX_SECONDS) {
          totalTime_ = ORB_MODE_2_MAX_SECONDS;
          allowAddTime_ = false;
        }
      }
      break;

    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      return endMode();

    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLINE_BACK_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);
      g_machineState.increaseScore(jackpotValue_, true);
      g_gameMode.setScoreIncreased(true);
      resetInlineDropsTime_ = g_machineState.currentTime() + 1000;
      break;

    case SW_O_DROP_TARGET:
    case SW_R_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_S_DROP_TARGET:
      g_machineState.registerOrbsDropTarget(switchHit);
      g_machineState.increaseScore(jackpotValue_);
      g_gameMode.setScoreIncreased(true);
      resetOrbsDropsTime_ = g_machineState.currentTime() + 1000;
      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
      g_machineState.registerRightDropTarget(switchHit);
      g_machineState.increaseScore(jackpotValue_);
      g_gameMode.setScoreIncreased(true);
      resetRightDropsTime_ = g_machineState.currentTime() + 1000;
      break;

    case SW_LEFT_OUTLANE:
    case SW_LEFT_RETURN_LANE:
    case SW_RIGHT_OUTLANE:
    case SW_RIGHT_RETURN_LANE:
      jackpotValue_ += 25000;
      break;
  }

  if (secondsRemaining_ == 0) return endMode();
  return GAME_MODE_ORBS_2;
}

unsigned long OrbMode2::jackpotValue() {
  return jackpotValue_;
}

void OrbMode2::endModeViaBallEnded() {
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
}

/*********************************************************************
    Private
*********************************************************************/
byte OrbMode2::endMode() {
  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);

  g_lampsHelper.hideLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS);
  g_lampsHelper.hideLamps(LAMP_COLLECTION_GUARDIAN_ROLLOVERS);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
  g_displayHelper.showPlayerScores(0xFF);

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void OrbMode2::manageModeLamps() {
  allowAddTime_ ? g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW) : g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
}

void OrbMode2::manageDropsReset() {
  if (resetInlineDropsTime_ && resetInlineDropsTime_ < g_machineState.currentTime()) {
    g_machineState.resetInlineDropTargets(true, true, g_machineState.currentTime());
    resetInlineDropsTime_ = 0;
  }

  if (resetOrbsDropsTime_ && resetOrbsDropsTime_ < g_machineState.currentTime()) {
    g_machineState.resetOrbsDropTargets(true, true, g_machineState.currentTime());
    resetOrbsDropsTime_ = 0;
  }

  if (resetRightDropsTime_ && resetRightDropsTime_ < g_machineState.currentTime()) {
    g_machineState.resetRightDropTargets(true, true, g_machineState.currentTime());
    resetRightDropsTime_ = 0;
  }
}

void OrbMode2::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 2\n\r");

  allowAddTime_         = true;
  jackpotValue_         = 25000;
  resetInlineDropsTime_ = 0;
  resetOrbsDropsTime_   = 0;
  resetRightDropsTime_  = 0;
  secondsRemaining_     = ORB_MODE_2_INITIAL_SECONDS;
  startedTime_          = g_machineState.currentTime();
  totalTime_            = ORB_MODE_2_INITIAL_SECONDS;

  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.showLamp(LAMP_2_CAPTIVE_ORBS);
  g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);

  g_lampsHelper.showLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS,  true);
  g_lampsHelper.showLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP,  true);
  g_lampsHelper.showLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS, true);
  g_lampsHelper.showLamps(LAMP_COLLECTION_GUARDIAN_ROLLOVERS, false);

  BSOS_SetDisplayCredits(secondsRemaining_);
  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);
}

void OrbMode2::manageTimeRemaining() {
  byte secondsSinceModeStarted = (g_machineState.currentTime() - startedTime_) / 1000;
  byte secondsRemaining = (totalTime_ - secondsSinceModeStarted);

  if (secondsRemaining != secondsRemaining_) {
    secondsRemaining_ = secondsRemaining;
    BSOS_SetDisplayCredits(secondsRemaining_);
    if (!g_bonusLightShow.running()) g_lampsHelper.showBonusLamps(secondsRemaining_);

    if (secondsRemaining_ == 10) g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_6);
    if (secondsRemaining_ == 5)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_5);
    if (secondsRemaining_ == 4)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_4);
    if (secondsRemaining_ == 3)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_3);
    if (secondsRemaining_ == 2)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_2);
    if (secondsRemaining_ == 1)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_1);
  }
}
