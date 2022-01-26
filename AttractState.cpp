#include "SharedVariables.h"

int RunAttractState(int curState, boolean curStateChanged) {
  int returnState = curState;

  if (curStateChanged) {
    BSOS_DisableSolenoidStack();
    BSOS_TurnOffAllLamps();
    BSOS_SetDisableFlippers(true);
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract Mode\n\r");
    }
    BSOS_SetDisplayCredits(Credits, true);
  }

  byte switchHit;

  unsigned long currentTime = GlobalMachineState.GetCurrentTime();
  unsigned long seed = currentTime / 250;
  if (seed != LastFlash) {
    LastFlash = seed;
    if (((currentTime / 250) % 3) == 0) ShowLamp(LAMP_40K_BONUS, true);
    if (((currentTime / 250) % 3) == 1) ShowLamps(LAMP_COLLECTION_BONUS_MIDDLE_RING, true);
    if (((currentTime / 250) % 3) == 2) ShowLamps(LAMP_COLLECTION_BONUS_OUTER_RING, true);
  }

  switchHit = BSOS_PullFirstFromSwitchStack();
  while (switchHit != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
    case(SW_CREDIT_BUTTON):
      if (GlobalMachineState.ResetNumberOfPlayers()) returnState = MACHINE_STATE_INIT_GAMEPLAY;
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      GlobalMachineState.WriteCoinToAudit(switchHit);
      GlobalMachineState.IncreaseCredits(true, 1);
      break;
    case SW_SELF_TEST_SWITCH:
      if (currentTime - GetLastSelfTestChangedTime() > 250) {
        returnState = MACHINE_STATE_TEST_LIGHTS;
        SetLastSelfTestChangedTime(currentTime);
      }
      break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return returnState;
}
