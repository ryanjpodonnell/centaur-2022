#include "SharedVariables.h"

OrbMode1::OrbMode1() {}

byte OrbMode1::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) handleNewMode();
  byte newGameMode = GAME_MODE_ORBS_1;

  switch(switchHit) {
    case 0xFF:
      break;
  }

  byte secondsSinceModeStarted = (g_machineState.currentTime() - modeStartedTime_) / 1000;
  byte secondsRemaining = (totalSeconds_ - secondsSinceModeStarted);
  if (secondsRemaining != secondsRemaining_) {
    secondsRemaining_ = secondsRemaining;
    g_displayHelper.overrideCredits(secondsRemaining_);
  }

  if (secondsRemaining_ == 0) {
    newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
    g_displayHelper.overrideCredits(g_machineState.credits());
  }

  return newGameMode;
}

/*********************************************************************
    Private
*********************************************************************/
void OrbMode1::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Orb Mode 1\n\r");
  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.showLamp(LAMP_1_CAPTIVE_ORBS);

  modeStartedTime_ = g_machineState.currentTime();
  secondsRemaining_ = 30;
  totalSeconds_ = 30;
  g_displayHelper.overrideCredits(secondsRemaining_);
}
