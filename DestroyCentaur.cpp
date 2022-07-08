#include "SharedVariables.h"

DestroyCentaur::DestroyCentaur() {}

byte DestroyCentaur::run(boolean gameModeChanged, byte switchHit) {
  if (gameModeChanged) manageNewMode();
  manageModeLamps();

  switch(switchHit) {
    case SW_1ST_INLINE_DROP_TARGET:
    case SW_2ND_INLINE_DROP_TARGET:
    case SW_3RD_INLINE_DROP_TARGET:
    case SW_4TH_INLINE_DROP_TARGET:
    case SW_INLINE_BACK_TARGET:
      g_machineState.registerInlineDropTarget(switchHit);

      g_machineState.increaseScore(jackpotValue_, true);
      g_gameMode.setScoreIncreased(true);
      return endMode();
  }

  return GAME_MODE_DESTROY_CENTAUR;
}

unsigned long DestroyCentaur::jackpotValue() {
  return jackpotValue_;
}

void DestroyCentaur::endModeViaBallEnded() {
  g_machineState.unqualifyAllModes();
  g_lampsHelper.hideAllLamps();
  g_lampsHelper.showLamps(LAMP_COLLECTION_GENERAL_ILLUMINATION);
}

/*********************************************************************
    Private
*********************************************************************/
byte DestroyCentaur::endMode() {
  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);
  g_displayHelper.showPlayerScores(0xFF);

  g_machineState.unqualifyAllModes();
  g_lampsHelper.hideAllLamps();
  g_lampsHelper.showLamps(LAMP_COLLECTION_GENERAL_ILLUMINATION);

  return GAME_MODE_UNSTRUCTURED_PLAY;
}

void DestroyCentaur::manageModeLamps() {
}

void DestroyCentaur::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Destroy Centaur Mode\n\r");

  jackpotValue_         = 1;
  g_machineState.hideAllPlayerLamps();
  g_lampsHelper.hideLamps(LAMP_COLLECTION_GENERAL_ILLUMINATION);

  g_bonusLightShow.start(BONUS_LIGHT_SHOW_SPIN);

  unsigned long activationTime = g_machineState.currentTime() + 500;
  activationTime = g_machineState.resetInlineDropTargets(true, true, activationTime);
  activationTime = g_machineState.resetOrbsDropTargets  (true, true, activationTime);
  activationTime = g_machineState.resetRightDropTargets (true, true, activationTime);
}
