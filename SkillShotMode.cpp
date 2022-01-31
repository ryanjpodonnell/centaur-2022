#include "SharedVariables.h"

SkillShot::SkillShot() {
  byte litRollover_ = 0;
}

byte SkillShot::run(byte gameModeId, boolean gameModeChanged) {
  if (gameModeChanged) handleNewMode();
  byte newGameMode = gameModeId;

  byte switchHit  = BSOS_PullFirstFromSwitchStack();
  while (switchHit != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
      case SW_RIGHT_FLIPPER_BUTTON:
        if(litRollover_ == 0) {
          g_lampsHelper.hideLamp(LAMP_TOP_LEFT_ROLLOVER);
          g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER, true);
          litRollover_ = 1;
        } else if(litRollover_ == 1) {
          g_lampsHelper.hideLamp(LAMP_TOP_MIDDLE_ROLLOVER);
          g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER, true);
          litRollover_ = 2;
        } else {
          g_lampsHelper.hideLamp(LAMP_TOP_RIGHT_ROLLOVER);
          g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER, true);
          litRollover_ = 0;
        }
        break;

      case SW_TOP_LEFT_LANE:
        g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
        g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER);

        if(litRollover_ == 0) {
          g_machineState.increaseScore(100);
          g_machineState.increaseBonus(100);
        }

        newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        break;

      case SW_TOP_MIDDLE_LANE:
        g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
        g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER);

        if(litRollover_ == 1) {
          g_machineState.increaseScore(100);
          g_machineState.increaseBonus(100);
        }

        newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        break;

      case SW_TOP_RIGHT_LANE:
        g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
        g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER);

        if(litRollover_ == 2) {
          g_machineState.increaseScore(100);
          g_machineState.increaseBonus(100);
        }

        newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        break;

      case SW_OUTHOLE:
        break;

      default:
        g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);

        newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
        break;
    }

    switchHit = BSOS_PullFirstFromSwitchStack();
  }

  return newGameMode;
}

void SkillShot::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering SkillShot Mode\n\r");

  g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
  g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER, true);
}
