#include "SharedVariables.h"

Attract::Attract() {
  currentFlashCycle_ = 0;
  lastFlash_         = 0;
  stateStatedTime_   = 0;
}

byte Attract::run(byte curState, boolean curStateChanged) {
  byte returnState = curState;

  if (curStateChanged) {
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract State\n\r");
    }
    BSOS_DisableSolenoidStack();
    BSOS_TurnOffAllLamps();
    BSOS_SetDisableFlippers(true);
    BSOS_SetDisplayCredits(Credits, true);

    g_machineState.setScore(0, 0);
    g_machineState.setScore(0, 1);
    g_machineState.setScore(0, 2);
    g_machineState.setScore(0, 3);
    g_displayHelper.showPlayerScores(0xFF, false, false);
  }

  byte switchHit;
  unsigned long currentTime = g_machineState.currentTime();
  unsigned long seed = currentTime / 250;        // .25 seconds
  unsigned long cycleSeed = currentTime / 10000; // 10 seconds

  if (cycleSeed != currentFlashCycle_) {
    currentFlashCycle_ = cycleSeed;
  }

  if (seed != lastFlash_) {
    lastFlash_ = seed;

    if (currentFlashCycle_ % 2 == 0) {
      if ((seed % 3) == 0) ShowLamp(LAMP_40K_BONUS, true);
      if ((seed % 3) == 1) ShowLamps(LAMP_COLLECTION_BONUS_MIDDLE_RING, true);
      if ((seed % 3) == 2) ShowLamps(LAMP_COLLECTION_BONUS_OUTER_RING, true);
    } else {
      if ((seed % 3) == 0) ShowLamps(LAMP_COLLECTION_BONUS_OUTER_RING, true);
      if ((seed % 3) == 1) ShowLamps(LAMP_COLLECTION_BONUS_MIDDLE_RING, true);
      if ((seed % 3) == 2) ShowLamp(LAMP_40K_BONUS, true);
    }
  }

  switchHit = BSOS_PullFirstFromSwitchStack();
  while (switchHit != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
    case(SW_CREDIT_BUTTON):
      if (g_machineState.resetNumberOfPlayers()) returnState = MACHINE_STATE_INIT_GAMEPLAY;
      break;
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
      g_machineState.writeCoinToAudit(switchHit);
      g_machineState.increaseCredits(true, 1);
      break;
    case SW_SELF_TEST_SWITCH:
      if (currentTime - g_selfTestAndAudit.lastSelfTestChangedTime() > 250) {
        returnState = MACHINE_STATE_TEST_LIGHTS;
        g_selfTestAndAudit.setLastSelfTestChangedTime(currentTime);
      }
      break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return returnState;
}
