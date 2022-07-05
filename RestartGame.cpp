#include "SharedVariables.h"

RestartGame::RestartGame() {
}

int RestartGame::run(boolean curStateChanged) {
  if (curStateChanged) manageNewState();

  if (g_bonusLightShow.running()) {
    g_bonusLightShow.run();
  } else {
    g_lampsHelper.hideAllLamps();
    g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  }

  if (g_bonusLightShow.ended()) return MACHINE_STATE_INIT_GAMEPLAY;
  return MACHINE_STATE_RESTART_GAME;
}

/*********************************************************************
    Private
*********************************************************************/
void RestartGame::manageNewState() {
  if (DEBUG_MESSAGES) Serial.write("Entering Restart Game State\n\r");

  g_bonusLightShow.reset();
  g_lampsHelper.hideAllLamps();
  g_soundHelper.stopAudio();
  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);
}
