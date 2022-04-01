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
  byte secondsRemaining = (ORB_MODE_1_TOTAL_SECONDS - secondsSinceModeStarted);
  if (secondsRemaining != secondsRemaining_) {
    secondsRemaining_ = secondsRemaining;
    g_displayHelper.overrideCredits(secondsRemaining_);
  }

  if (secondsRemaining_ == 0) {
    BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 8, g_machineState.currentTime() + 500);
    BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 8, g_machineState.currentTime() + 500);
    BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 8, g_machineState.currentTime() + 500);

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

  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 8, g_machineState.currentTime() + 500);
  BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 8, g_machineState.currentTime() + 500);
  BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 8, g_machineState.currentTime() + 500);

  modeStartedTime_ = g_machineState.currentTime();
  secondsRemaining_ = ORB_MODE_1_TOTAL_SECONDS;

  g_displayHelper.overrideCredits(secondsRemaining_);
}
