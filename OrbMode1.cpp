#include "SharedVariables.h"

OrbMode1::OrbMode1() {}

byte OrbMode1::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) handleNewMode();
  byte newGameMode = GAME_MODE_ORBS_1;

  switch(switchHit) {
    case 0xFF:
      break;
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
}
