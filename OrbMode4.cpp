#include "SharedVariables.h"

OrbMode4::OrbMode4() {}

byte OrbMode4::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageDropsReset();
  manageModeLamps();
  manageTimeRemaining();

  switch(switchHit) {
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      secondsRemaining_ += 5;
      if (secondsRemaining_ >= ORB_MODE_4_MAX_SECONDS) secondsRemaining_ = ORB_MODE_4_MAX_SECONDS;

      BSOS_SetDisplayCredits(secondsRemaining_);
      if (!g_bonusLightShow.running()) g_lampsHelper.showBonusLamps(secondsRemaining_);

      break;

    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      return endMode();

    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLINE_BACK_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);
      if (currentJackpotTarget_ == switchHit) {
        g_machineState.increaseScore(jackpotValue_, true);
        g_gameMode.setScoreIncreased(true);
        jackpotValue_ += 25000;
      }
      resetInlineDropsTime_ = g_machineState.currentTime() + 1000;
      break;

    case SW_O_DROP_TARGET:
    case SW_R_DROP_TARGET:
    case SW_B_DROP_TARGET:
    case SW_S_DROP_TARGET:
      g_machineState.registerOrbsDropTarget(switchHit);
      if (currentJackpotTarget_ == switchHit) {
        g_machineState.increaseScore(jackpotValue_, true);
        g_gameMode.setScoreIncreased(true);
        jackpotValue_ += 25000;
      }
      resetOrbsDropsTime_ = g_machineState.currentTime() + 1000;
      break;

    case SW_RIGHT_4_DROP_TARGET_1:
    case SW_RIGHT_4_DROP_TARGET_2:
    case SW_RIGHT_4_DROP_TARGET_3:
    case SW_RIGHT_4_DROP_TARGET_4:
      g_machineState.registerRightDropTarget(switchHit);
      if (currentJackpotTarget_ == switchHit) {
        g_machineState.increaseScore(jackpotValue_, true);
        g_gameMode.setScoreIncreased(true);
        jackpotValue_ += 25000;
      }
      resetRightDropsTime_ = g_machineState.currentTime() + 1000;
      break;
  }

  if (secondsRemaining_ == 0) return endMode();
  return GAME_MODE_ORBS_4;
}

unsigned long OrbMode4::jackpotValue() {
  return jackpotValue_;
}

void OrbMode4::endModeViaBallEnded() {
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
}

/*********************************************************************
    Private
*********************************************************************/
byte OrbMode4::endMode() {
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

void OrbMode4::manageModeLamps() {
  unsigned long seed = g_machineState.currentTime() / 3000;
  if (seed != lastFlashRovingTarget_) {
    g_lampsHelper.hideLamps(LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS);
    g_lampsHelper.hideLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP);
    g_lampsHelper.hideLamps(LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS);

    lastFlashRovingTarget_ = seed;
    byte currentStep = seed % 9;
    if (currentStep == 0) {
      currentJackpotTarget_ = SW_1ST_INLINE_DROP_TARGET;
      g_lampsHelper.showLamps(LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP, true);
    } else if (currentStep == 1) {
      currentJackpotTarget_ = SW_O_DROP_TARGET;
      g_lampsHelper.showLamp(LAMP_O_DROP_TARGET_ARROW, true);
    } else if (currentStep == 2) {
      currentJackpotTarget_ = SW_R_DROP_TARGET;
      g_lampsHelper.showLamp(LAMP_R_DROP_TARGET_ARROW, true);
    } else if (currentStep == 3) {
      currentJackpotTarget_ = SW_B_DROP_TARGET;
      g_lampsHelper.showLamp(LAMP_B_DROP_TARGET_ARROW, true);
    } else if (currentStep == 4) {
      currentJackpotTarget_ = SW_S_DROP_TARGET;
      g_lampsHelper.showLamp(LAMP_S_DROP_TARGET_ARROW, true);
    } else if (currentStep == 5) {
      currentJackpotTarget_ = SW_RIGHT_4_DROP_TARGET_1;
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_10_ARROW, true);
    } else if (currentStep == 6) {
      currentJackpotTarget_ = SW_RIGHT_4_DROP_TARGET_2;
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_20_ARROW, true);
    } else if (currentStep == 7) {
      currentJackpotTarget_ = SW_RIGHT_4_DROP_TARGET_3;
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_40_ARROW, true);
    } else {
      currentJackpotTarget_ = SW_RIGHT_4_DROP_TARGET_4;
      g_lampsHelper.showLamp(LAMP_RIGHT_DROP_TARGET_80_ARROW, true);
    }
  }
}

void OrbMode4::manageDropsReset() {
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

void OrbMode4::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 4\n\r");

  currentJackpotTarget_      = 0xFF;
  jackpotValue_              = 25000;
  lastFlashRovingTarget_     = 0;
  lastFlashSecondsRemaining_ = 0;
  resetInlineDropsTime_      = 0;
  resetOrbsDropsTime_        = 0;
  resetRightDropsTime_       = 0;
  secondsRemaining_          = ORB_MODE_4_INITIAL_SECONDS;

  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.showLamp(LAMP_4_CAPTIVE_ORBS);
  g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);
  g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW);

  BSOS_SetDisplayCredits(secondsRemaining_);
  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN, SOUND_ALARM);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);
}

void OrbMode4::manageTimeRemaining() {
  unsigned long seed = g_machineState.currentTime() / 1000; // 1 second

  if (seed != lastFlashSecondsRemaining_) {
    lastFlashSecondsRemaining_ = seed;

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
