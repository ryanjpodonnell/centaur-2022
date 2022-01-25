#include "SharedVariables.h"

boolean AddPlayer(boolean resetNumPlayers) {
  if (Credits < 1 && !FreePlayMode) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers >= 4) return false;

  CurrentNumPlayers += 1;
  BSOS_SetDisplay(CurrentNumPlayers - 1, 0);
  BSOS_SetDisplayBlank(CurrentNumPlayers - 1, 0x30);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

int InitGamePlay() {
  if (DEBUG_MESSAGES) {
    Serial.write("Starting game\n\r");
  }

  BSOS_SetDisableFlippers(false);
  BSOS_EnableSolenoidStack();
  BSOS_TurnOffAllLamps();

  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, CurrentTime + 500);
  BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 10, CurrentTime + 500);
  BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 10, CurrentTime + 500);

  for (int count = 0; count < 4; count++) {
    BonusX[count] = 1;
    Bonus[count] = 0;
  }
  memset(CurrentScores, 0, 4 * sizeof(unsigned long));

  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
  CurrentPlayer = 0;
  ShowPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}

int InitNewBall(bool curStateChanged, byte playerNum, int ballNum) {
  if (curStateChanged) {
    ShowLamps(LAMP_COLLECTION_BONUS_ALL, true);

    BSOS_SetDisplayCredits(Credits, true);
    SamePlayerShootsAgain = false;

    BSOS_SetDisplayBallInPlay(ballNum);
    BSOS_SetLampState(LAMP_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      BSOS_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    GlobalGameMode.SetGameMode(GAME_MODE_SKILL_SHOT);

    ExtraBallCollected = false;

    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
      BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, CurrentTime + 600);
    }

    // Reset progress unless holdover awards
    Bonus[CurrentPlayer] = 0;
    BonusX[CurrentPlayer] = 1;

    ScoreMultiplier = 1;
    CurrentBonus = Bonus[CurrentPlayer];
    ScoreAdditionAnimation = 0;
    ScoreAdditionAnimationStartTime = 0;
    BonusXAnimationStart = 0;
  }

  // We should only consider the ball initialized when
  // the ball is no longer triggering the SW_OUTHOLE
  if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }
}

void AddCoinToAudit(byte switchHit) {
  unsigned short coinAuditStartByte = 0;

  switch (switchHit) {
    case SW_COIN_3: coinAuditStartByte = BSOS_CHUTE_3_COINS_START_BYTE; break;
    case SW_COIN_2: coinAuditStartByte = BSOS_CHUTE_2_COINS_START_BYTE; break;
    case SW_COIN_1: coinAuditStartByte = BSOS_CHUTE_1_COINS_START_BYTE; break;
  }

  if (coinAuditStartByte) {
    BSOS_WriteULToEEProm(coinAuditStartByte, BSOS_ReadULFromEEProm(coinAuditStartByte) + 1);
  }

}

void AddCredit(boolean playSound, byte numToAdd) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
    BSOS_SetDisplayCredits(Credits);
    BSOS_SetCoinLockout(false);
  } else {
    BSOS_SetDisplayCredits(Credits);
    BSOS_SetCoinLockout(true);
  }

}

void AwardExtraBall() {
  if (ExtraBallCollected) return;

  ExtraBallCollected = true;
  SamePlayerShootsAgain = true;
  BSOS_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
}
