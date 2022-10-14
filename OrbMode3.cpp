#include "SharedVariables.h"

OrbMode3::OrbMode3() {}

byte OrbMode3::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageTimeRemaining();

  switch(switchHit) {
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      secondsRemaining_ += 5;
      if (secondsRemaining_ >= ORB_MODE_3_MAX_SECONDS) secondsRemaining_ = ORB_MODE_3_MAX_SECONDS;

      BSOS_SetDisplayCredits(secondsRemaining_);
      if (!g_bonusLightShow.running()) g_lampsHelper.showBonusLamps(secondsRemaining_);

      break;

    case SW_TOP_SPOT_1_THROUGH_4_TARGET:
      return endMode();

    case SW_LEFT_THUMPER_BUMPER:
    case SW_RIGHT_THUMPER_BUMPER:
      g_machineState.increaseScore(jackpotValue_, true);
      g_gameMode.setScoreIncreased(true);
      break;

    case SW_TOP_LEFT_LANE:
    case SW_TOP_MIDDLE_LANE:
    case SW_TOP_RIGHT_LANE:
      jackpotValue_ += 5000;
      break;
  }

  if (secondsRemaining_ == 0) return endMode();
  return GAME_MODE_ORBS_3;
}

unsigned long OrbMode3::jackpotValue() {
  return jackpotValue_;
}

void OrbMode3::endModeViaBallEnded() {
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
}

/*********************************************************************
    Private
*********************************************************************/
byte OrbMode3::endMode() {
  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);

  g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
  g_lampsHelper.showLamp(LAMP_LEFT_THUMPER_BUMPER);
  g_lampsHelper.showLamp(LAMP_RIGHT_THUMPER_BUMPER);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  BSOS_SetDisplayCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();
  g_displayHelper.showPlayerScores(0xFF);

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void OrbMode3::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 3\n\r");

  jackpotValue_         = 25000;
  lastFlash_            = 0;
  secondsRemaining_     = ORB_MODE_3_INITIAL_SECONDS;

  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.showLamp(LAMP_3_CAPTIVE_ORBS);
  g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);
  g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW);

  g_lampsHelper.showLamp(LAMP_LEFT_THUMPER_BUMPER,       true);
  g_lampsHelper.showLamp(LAMP_RIGHT_THUMPER_BUMPER,      true);
  g_lampsHelper.showLamps(LAMP_COLLECTION_TOP_ROLLOVERS, false);

  BSOS_SetDisplayCredits(secondsRemaining_);
  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN, SOUND_ALARM);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);
}

void OrbMode3::manageTimeRemaining() {
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
