#include "SharedVariables.h"

OrbMode1::OrbMode1() {}

byte OrbMode1::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageBonusLightShow();
  manageModeLamps();
  manageTimeRemaining();

  switch(switchHit) {
    case SW_RESET_1_THROUGH_4_TARGETS_TARGET:
      if (allowAddTime_) {
        totalTime_ += 5;

        if (totalTime_ >= ORB_MODE_1_MAX_SECONDS) {
          totalTime_ = ORB_MODE_1_MAX_SECONDS;
          allowAddTime_ = false;
        }
      }
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
  g_machineState.resetInlineDropTargets(false, true, 0);
  g_machineState.resetOrbsDropTargets  (false, true, 0);
  g_machineState.resetRightDropTargets (false, true, 0);

  g_displayHelper.overrideCredits(g_machineState.credits());
  g_displayHelper.showPlayerScores(0xFF);
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

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  g_displayHelper.overrideCredits(g_machineState.credits());
  g_displayHelper.showPlayerScores(0xFF);
  g_machineState.completeSelectedMode();

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void OrbMode1::manageBonusLightShow() {
  if (g_bonusLightShow.running()) g_bonusLightShow.run();
}

void OrbMode1::manageModeLamps() {
  allowAddTime_ ? g_lampsHelper.showLamp(LAMP_RESET_1_THROUGH_4_ARROW) : g_lampsHelper.hideLamp(LAMP_RESET_1_THROUGH_4_ARROW);
}

void OrbMode1::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 1\n\r");
  if (g_machineState.hurryUpActivated()) g_gameMode.endHurryUp();

  allowAddTime_     = true;
  jackpotValue_     = 100000;
  secondsRemaining_ = ORB_MODE_1_TOTAL_SECONDS;
  startedTime_      = g_machineState.currentTime();
  totalTime_        = ORB_MODE_1_TOTAL_SECONDS;

  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.showLamp(LAMP_1_CAPTIVE_ORBS);
  g_lampsHelper.showLamp(LAMP_SPOT_1_THROUGH_4);

  g_displayHelper.overrideCredits(secondsRemaining_);
  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, false, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, false, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, false, activationTime);
}

void OrbMode1::manageTimeRemaining() {
  byte secondsSinceModeStarted = (g_machineState.currentTime() - startedTime_) / 1000;
  byte secondsRemaining = (totalTime_ - secondsSinceModeStarted);

  if (secondsRemaining != secondsRemaining_) {
    secondsRemaining_ = secondsRemaining;
    g_displayHelper.overrideCredits(secondsRemaining_);
    if (!g_bonusLightShow.running()) g_lampsHelper.showBonusLamps(secondsRemaining_);

    if (secondsRemaining_ == 10) g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_6);
    if (secondsRemaining_ == 5)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_5);
    if (secondsRemaining_ == 4)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_4);
    if (secondsRemaining_ == 3)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_3);
    if (secondsRemaining_ == 2)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_2);
    if (secondsRemaining_ == 1)  g_soundHelper.playSoundWithoutInterruptions(SOUND_CHIRP_1);
  }
}
