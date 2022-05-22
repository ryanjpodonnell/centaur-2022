#include "SharedVariables.h"

SkillShot::SkillShot() {}

byte SkillShot::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) handleNewMode();
  byte newGameMode = GAME_MODE_SKILL_SHOT;

  switch(switchHit) {
    case SW_RIGHT_FLIPPER_BUTTON:
      bonusValue_ = 1;
      scoreValue_ = 100;

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
      newGameMode = checkRollover(SW_TOP_LEFT_LANE);
      break;

    case SW_TOP_MIDDLE_LANE:
      newGameMode = checkRollover(SW_TOP_MIDDLE_LANE);
      break;

    case SW_TOP_RIGHT_LANE:
      newGameMode = checkRollover(SW_TOP_RIGHT_LANE);
      break;

    case SW_TILT:
    case SW_SELF_TEST_SWITCH:
    case SW_COIN_1:
    case SW_COIN_2:
    case SW_COIN_3:
    case SW_CREDIT_BUTTON:
    case SW_OUTHOLE:
    case 0xFF:
      break;

    default:
      g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);
      newGameMode = GAME_MODE_UNSTRUCTURED_PLAY;
      break;
  }


  return newGameMode;
}

/*********************************************************************
    Private
*********************************************************************/
byte SkillShot::checkRollover(byte switchHit) {
  g_lampsHelper.hideLamps(LAMP_COLLECTION_TOP_ROLLOVERS);

  if(activeRollover_ == switchHit) {
    g_machineState.increaseScore(scoreValue_);
    g_machineState.increaseBonus(bonusValue_);
    g_gameMode.setScoreIncreased(true);
  }

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void SkillShot::handleNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Skill Shot Mode\n\r");
  g_machineState.hideAllPlayerLamps();

  bonusValue_ = 10;
  scoreValue_ = 1000;

  long randomNumber = random(3);
  if (randomNumber == 0) {
    activeRollover_ = SW_TOP_LEFT_LANE;
    g_lampsHelper.showLamp(LAMP_TOP_LEFT_ROLLOVER, true);
  } else if (randomNumber == 1) {
    activeRollover_ = SW_TOP_MIDDLE_LANE;
    g_lampsHelper.showLamp(LAMP_TOP_MIDDLE_ROLLOVER, true);
  } else {
    activeRollover_ = SW_TOP_RIGHT_LANE;
    g_lampsHelper.showLamp(LAMP_TOP_RIGHT_ROLLOVER, true);
  }
}
