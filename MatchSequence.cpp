#include "SharedVariables.h"

MatchSequence::MatchSequence() {
  matchDelay_             = 150;
  matchDigit_             = 0;
  matchSequenceStartTime_ = 0;
  numMatchSpins_          = 0;
  scoreMatches_           = 0;
}

byte MatchSequence::run(boolean gameModeChanged) {
  if (gameModeChanged) manageNewMode();

  byte switchHit;
  while ((switchHit = BSOS_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY) {
    switch(switchHit) {
      case(SW_CREDIT_BUTTON):
        return g_machineState.manageCreditButton(MACHINE_STATE_MATCH_MODE);
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

  if (numMatchSpins_ < 40) {
    if (g_machineState.currentTime() > (matchSequenceStartTime_ + matchDelay_)) {
      matchDigit_ += 1;
      if (matchDigit_ > 9) matchDigit_ = 0;

      g_soundHelper.playSound(SOUND_LASERS);
      BSOS_SetDisplayBallInPlay((int)matchDigit_ * 10);

      matchDelay_    += 50 + 4 * numMatchSpins_;
      numMatchSpins_ += 1;

      if (numMatchSpins_ % 2 == 0) g_lampsHelper.hideLamp(LAMP_MATCH);
      if (numMatchSpins_ % 2 == 1) g_lampsHelper.showLamp(LAMP_MATCH);

      if (numMatchSpins_ == 40) {
        g_lampsHelper.hideLamp(LAMP_MATCH);
        matchDelay_ = g_machineState.currentTime() - matchSequenceStartTime_;
      }
    }
  }

  if (numMatchSpins_ >= 40 && numMatchSpins_ <= 43) {
    if (g_machineState.currentTime() > (matchSequenceStartTime_ + matchDelay_)) {
      byte playerNumber = numMatchSpins_ - 40;

      if ((g_machineState.numberOfPlayers() > (playerNumber)) && ((g_machineState.score(playerNumber) / 10) % 10) == matchDigit_) {
        scoreMatches_ |= (1 << (playerNumber));

        g_machineState.increaseCredits();
        BSOS_PushToTimedSolenoidStack(SOL_KNOCKER, SOL_KNOCKER_STRENGTH, g_machineState.currentTime() + (300 * playerNumber));

        matchDelay_ += 1000;
        numMatchSpins_ += 1;

        g_lampsHelper.showLamp(LAMP_MATCH);
      } else {
        numMatchSpins_ += 1;
      }
      if (numMatchSpins_ == 44) {
        matchDelay_ += 5000;
      }
    }
  }

  if (numMatchSpins_ > 43) {
    if (g_machineState.currentTime() > (matchSequenceStartTime_ + matchDelay_)) {
      g_machineState.setNumberOfPlayers(0);
      return MACHINE_STATE_ATTRACT;
    }
  }

  for (int count = 0; count < 4; count++) {
    if ((scoreMatches_ >> count) & 0x01) {
      // If this score matches, we're going to flash the last two digits
      if ( (g_machineState.currentTime() / 200) % 2 ) {
        BSOS_SetDisplayBlank(count, BSOS_GetDisplayBlank(count) & 0x1F);
      } else {
        BSOS_SetDisplayBlank(count, BSOS_GetDisplayBlank(count) | 0x60);
      }
    }
  }

  return MACHINE_STATE_MATCH_MODE;
}

/*********************************************************************
    Private
*********************************************************************/
void MatchSequence::manageNewMode() {
  if (DEBUG_MESSAGES) Serial.write("Entering Match Sequence Mode\n\r");

  matchSequenceStartTime_ = g_machineState.currentTime();
  matchDelay_             = 1500;
  matchDigit_             = g_machineState.currentTime() % 10;
  numMatchSpins_          = 0;
  scoreMatches_           = 0;

  g_lampsHelper.hideAllLamps();
  g_lampsHelper.showLamp(LAMP_MATCH, true);
  g_lampsHelper.showLamp(LAMP_GAME_OVER);

  BSOS_DisableSolenoidStack();
  BSOS_SetDisableFlippers(true);
}
