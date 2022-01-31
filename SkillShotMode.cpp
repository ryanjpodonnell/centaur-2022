#include "SharedVariables.h"

SkillShot::SkillShot() {
  activeRollover_ = SW_TOP_LEFT_LANE;
}

byte SkillShot::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) handleNewMode();
  byte newGameMode = GAME_MODE_SKILL_SHOT;

  switch(switchHit) {
    case SW_RIGHT_FLIPPER_BUTTON:
      g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);

      if(activeRollover_ == SW_TOP_LEFT_LANE) {
        activeRollover_ = SW_TOP_MIDDLE_LANE;
        g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER, true);
      } else if(activeRollover_ == SW_TOP_MIDDLE_LANE) {
        activeRollover_ = SW_TOP_RIGHT_LANE;
        g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER, true);
      } else {
        activeRollover_ = SW_TOP_LEFT_LANE;
        g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER, true);
      }
      break;

    case SW_TOP_LEFT_LANE:
      g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
      g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER);

      if(activeRollover_ == SW_TOP_LEFT_LANE) {
        g_machineState.increaseScore(100);
        g_machineState.increaseBonus(100);
        g_gameMode.setScoreIncreased(true);
      }
      newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
      break;

    case SW_TOP_MIDDLE_LANE:
      g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
      g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER);

      if(activeRollover_ == SW_TOP_MIDDLE_LANE) {
        g_machineState.increaseScore(100);
        g_machineState.increaseBonus(100);
        g_gameMode.setScoreIncreased(true);
      }
      newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
      break;

    case SW_TOP_RIGHT_LANE:
      g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
      g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER);

      if(activeRollover_ == SW_TOP_RIGHT_LANE) {
        g_machineState.increaseScore(100);
        g_machineState.increaseBonus(100);
        g_gameMode.setScoreIncreased(true);
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


  return newGameMode;
}

void SkillShot::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering SkillShot Mode\n\r");

  activeRollover_ = SW_TOP_LEFT_LANE;
  g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
  g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER, true);
}
