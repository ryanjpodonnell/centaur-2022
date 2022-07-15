#include "SharedVariables.h"

RestartGame::RestartGame() {
}

int RestartGame::run(boolean curStateChanged) {
  if (curStateChanged) manageNewState();

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
      case(SW_CREDIT_BUTTON):
        return g_machineState.manageCreditButton(MACHINE_STATE_RESTART_GAME);
        break;
      case SW_COIN_1:
      case SW_COIN_2:
      case SW_COIN_3:
        g_machineState.manageCoinDrop(switchHit);
        break;
      case SW_SELF_TEST_SWITCH:
        return MACHINE_STATE_TEST_LIGHTS;
        break;
    }
  }

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
