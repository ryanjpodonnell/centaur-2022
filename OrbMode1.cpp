#include "SharedVariables.h"

OrbMode1::OrbMode1() {}

byte OrbMode1::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageModeLamps();
  manageTimeRemaining();

  switch(switchHit) {
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      secondsRemaining_ += 5;
      if (secondsRemaining_ >= ORB_MODE_1_MAX_SECONDS) secondsRemaining_ = ORB_MODE_1_MAX_SECONDS;

      BSOS_SetDisplayCredits(secondsRemaining_);
      if (!g_bonusLightShow.running()) g_lampsHelper.showBonusLamps(secondsRemaining_);

      break;

    case SW_ORBS_RIGHT_LANE_TARGET:
      if (currentJackpotTarget_ == SW_ORBS_RIGHT_LANE_TARGET) {
        g_machineState.increaseScore(jackpotValue_);
        g_gameMode.setScoreIncreased(true);

        jackpotValue_         += 100000;
        currentJackpotTarget_  = SW_1ST_INLINE_DROP_TARGET;
      }
      break;

    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLINE_BACK_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);

      if (currentJackpotTarget_ == SW_1ST_INLINE_DROP_TARGET) {
        g_machineState.increaseScore(jackpotValue_, true);
        g_gameMode.setScoreIncreased(true);

        jackpotValue_         += 100000;
        currentJackpotTarget_  = SW_ORBS_RIGHT_LANE_TARGET;
      }

      g_machineState.resetInlineDropTargets(true, true, g_machineState.currentTime() + 250);
      break;

    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      return endMode();
  }

  if (secondsRemaining_ == 0) return endMode();
  return GAME_MODE_ORBS_1;
}

unsigned long OrbMode1::jackpotValue() {
  return jackpotValue_;
}

void OrbMode1::endModeViaBallEnded() {
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
}

/*********************************************************************
    Private
*********************************************************************/
byte OrbMode1::endMode() {
  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);

  g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);
  g_lampsHelper.hideLamp(LAMP_COLLECT_BONUS_ARROW);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
  g_displayHelper.showPlayerScores(0xFF);

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void OrbMode1::manageModeLamps() {
  currentJackpotTarget_ == SW_ORBS_RIGHT_LANE_TARGET ? g_lampsHelper.showLamp(LAMP_COLLECT_BONUS_ARROW, true)                 : g_lampsHelper.hideLamp(LAMP_COLLECT_BONUS_ARROW);
  currentJackpotTarget_ == SW_1ST_INLINE_DROP_TARGET ? g_lampsHelper.showLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP, true) : g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);
}

void OrbMode1::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 1\n\r");

  currentJackpotTarget_ = SW_1ST_INLINE_DROP_TARGET;
  jackpotValue_         = 100000;
  lastFlash_            = 0;
  secondsRemaining_     = ORB_MODE_1_INITIAL_SECONDS;

  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.showLamp(LAMP_1_CAPTIVE_ORBS);
  g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);
  g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW);

  BSOS_SetDisplayCredits(secondsRemaining_);
  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN, SOUND_ALARM);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);
}

void OrbMode1::manageTimeRemaining() {
  unsigned long seed = g_machineState.currentTime() / 1000; // 1 second

  if (seed != lastFlash_) {
    lastFlash_ = seed;

    secondsRemaining_--;
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
