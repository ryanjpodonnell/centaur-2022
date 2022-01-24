#include <Arduino.h>
#include "AttractMode.h"
#include "BallySternOS.h"
#include "Lights.h"
#include "MachineState.h"
#include "PinballMachineBase.h"
#include "SelfTestAndAudit.h"
#include "SharedVariables.h"

unsigned long LastFlash = 0;

int RunAttractMode(int curState, boolean curStateChanged) {
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

  unsigned long seed = CurrentTime / 250;
  if (seed != LastFlash) {
    LastFlash = seed;
    if (((CurrentTime / 250) % 3) == 0) ShowLamps(LAMP_COLLECTION_BONUS_CENTER);
    if (((CurrentTime / 250) % 3) == 1) ShowLamps(LAMP_COLLECTION_BONUS_MIDDLE_RING);
    if (((CurrentTime / 250) % 3) == 2) ShowLamps(LAMP_COLLECTION_BONUS_OUTER_RING);
  }

  switchHit = BSOS_PullFirstFromSwitchStack();
  while (switchHit != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
    case(SW_CREDIT_BUTTON):
      if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      AddCoinToAudit(switchHit);
      AddCredit(true, 1);
      break;
    case SW_SELF_TEST_SWITCH:
      if (CurrentTime - GetLastSelfTestChangedTime() > 250) {
        returnState = MACHINE_STATE_TEST_LIGHTS;
        SetLastSelfTestChangedTime(CurrentTime);
      }
      break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return returnState;
}
