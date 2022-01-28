#include "SharedVariables.h"

Attract::Attract() {
  currentFlashCycle_ = 0;
  lastFlash_         = 0;
  stateStatedTime_   = 0;
}

int Attract::run(int curState, boolean curStateChanged) {
  if (curStateChanged) handleNewState();
  handleLightShow();

  int returnState = curState;
  byte switchHit  = BSOS_PullFirstFromSwitchStack();

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
      if (g_machineState.currentTime() - g_selfTestAndAudit.lastSelfTestChangedTime() > 250) {
        returnState = MACHINE_STATE_TEST_LIGHTS;
        g_selfTestAndAudit.setLastSelfTestChangedTime(g_machineState.currentTime());
      }
      break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return returnState;
}

void Attract::handleNewState() {
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

void Attract::handleLightShow() {
  unsigned long currentTime = g_machineState.currentTime();
  unsigned long seed        = currentTime / 100;   // .10 seconds
  unsigned long cycleSeed   = currentTime / 10000; // 10 seconds

  if (cycleSeed != currentFlashCycle_) {
    currentFlashCycle_ = cycleSeed;
  }

  if (seed != lastFlash_) {
    lastFlash_ = seed;

    byte numberOfSteps = 16;
    byte currentStep = 0;
    if (currentFlashCycle_ % 2 == 0) {
      currentStep = seed % numberOfSteps;
    } else {
      currentStep = numberOfSteps - ((seed % numberOfSteps) + 1);
    }

    if (currentStep == 0) g_lampsHelper.showLamp(LAMP_40K_BONUS, true);
    if (currentStep == 1) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_MIDDLE_RING, true);
    if (currentStep == 2) g_lampsHelper.showLamps(LAMP_COLLECTION_BONUS_OUTER_RING, true);
    if (currentStep == 3) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_4, true);
    if (currentStep == 4) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_5, true);
    if (currentStep == 5) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_6, true);
    if (currentStep == 6) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_7, true);
    if (currentStep == 7) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_8, true);
    if (currentStep == 8) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_9, true);
    if (currentStep == 9) g_lampsHelper.showLamp(LAMP_COLLECT_BONUS_ARROW, true);
    if (currentStep == 10) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_10, true);
    if (currentStep == 11) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_11, true);
    if (currentStep == 12) g_lampsHelper.showLamp(LAMP_RIGHT_LANE_4X, true);
    if (currentStep == 13) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_12, true);
    if (currentStep == 14) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_13, true);
    if (currentStep == 15) g_lampsHelper.showLamps(LAMP_COLLECTION_RING_14, true);
  }
}
