#include "SharedVariables.h"

OrbMode1::OrbMode1() {}

byte OrbMode1::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  if (g_bonusLightShow.running()) g_bonusLightShow.run();

  switch(switchHit) {
    case 0xFF:
      break;
  }

  byte secondsSinceModeStarted = (g_machineState.currentTime() - modeStartedTime_) / 1000;
  byte secondsRemaining = (ORB_MODE_1_TOTAL_SECONDS - secondsSinceModeStarted);
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
    if (secondsRemaining_ == 0)  return endMode();
  }

  return GAME_MODE_ORBS_1;
}

/*********************************************************************
    Private
*********************************************************************/
byte OrbMode1::endMode() {
  unsigned long activationTime_ = g_machineState.currentTime() + 500;
  activationTime_ = g_machineState.resetInlineDropTargets(true, true, activationTime_);
  activationTime_ = g_machineState.resetOrbsDropTargets  (true, true, activationTime_);
  activationTime_ = g_machineState.resetRightDropTargets (true, true, activationTime_);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  g_displayHelper.overrideCredits(g_machineState.credits());
  g_machineState.completeSelectedMode();

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void OrbMode1::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 1\n\r");
  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.hideLamps(LAMP_COLLECTION_BONUS_ALL);
  g_lampsHelper.showLamp(LAMP_1_CAPTIVE_ORBS);

  unsigned long activationTime_ = g_machineState.currentTime() + 500;
  activationTime_ = g_machineState.resetInlineDropTargets(true, false, activationTime_);
  activationTime_ = g_machineState.resetOrbsDropTargets  (true, false, activationTime_);
  activationTime_ = g_machineState.resetRightDropTargets (true, false, activationTime_);

  modeStartedTime_ = g_machineState.currentTime();
  secondsRemaining_ = ORB_MODE_1_TOTAL_SECONDS;

  g_displayHelper.overrideCredits(secondsRemaining_);
  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
}
