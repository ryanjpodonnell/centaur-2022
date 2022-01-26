#include "SharedVariables.h"

MachineState::MachineState(byte id) {
  currentPlayer = 0;
  machineStateChanged = true;
  machineStateId = id;
}

boolean MachineState::GetMachineStateChanged() {
  return machineStateChanged;
}

boolean MachineState::GetSamePlayerShootsAgain() {
  return samePlayerShootsAgain;
}

boolean MachineState::IncrementNumberOfPlayers() {
  if (Credits < 1 && !FreePlayMode) return false;
  if (currentNumPlayers >= 4) return false;

  currentNumPlayers += 1;

  BSOS_SetDisplay(currentNumPlayers - 1, 0);
  BSOS_SetDisplayBlank(currentNumPlayers - 1, 0x30);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

boolean MachineState::ResetNumberOfPlayers() {
  if (Credits < 1 && !FreePlayMode) return false;

  currentNumPlayers = 1;

  BSOS_SetDisplay(0, 0);
  BSOS_SetDisplayBlank(0, 0x30);

  BSOS_WriteULToEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE, BSOS_ReadULFromEEProm(BSOS_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

byte MachineState::GetBonus() {
  return bonuses[currentPlayer];
}

byte MachineState::GetBonusMultiplier() {
  return bonusMultipliers[currentPlayer];
}

byte MachineState::GetCurrentBallInPlay() {
  return currentBallInPlay;
}

byte MachineState::GetCurrentPlayer() {
  return currentPlayer;
}

byte MachineState::GetMachineState() {
  return machineStateId;
}

byte MachineState::GetNumberOfPlayers() {
  return currentNumPlayers;
}

byte MachineState::IncrementCurrentPlayer() {
  currentPlayer += 1;

  if (currentPlayer >= currentNumPlayers) {
    currentPlayer = 0;
    currentBallInPlay += 1;
  }

  if (currentBallInPlay > BALLS_PER_GAME) {
    for (int count = 0; count < currentNumPlayers; count++) {
      BSOS_SetDisplay(count, scores[count], true, 2);
    }

    return MACHINE_STATE_INIT_GAMEPLAY;
  } else {
    return MACHINE_STATE_INIT_NEW_BALL;
  }
}

byte MachineState::ResetGame() {
  if (FreePlayMode) {
    return MACHINE_STATE_INIT_GAMEPLAY;
  } else if (Credits >= 1 && !FreePlayMode) {
    Credits -= 1;
    BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, Credits);
    BSOS_SetDisplayCredits(Credits);
    return MACHINE_STATE_INIT_GAMEPLAY;
  } else {
    return machineStateId;
  }
}

int MachineState::InitGamePlay() {
  if (DEBUG_MESSAGES) {
    Serial.write("Starting game\n\r");
  }

  BSOS_SetDisableFlippers(false);
  BSOS_EnableSolenoidStack();
  BSOS_TurnOffAllLamps();

  BSOS_PushToTimedSolenoidStack(SOL_ORBS_TARGET_RESET, 10, currentTime + 500);
  BSOS_PushToTimedSolenoidStack(SOL_INLINE_DROP_TARGET_RESET, 10, currentTime + 500);
  BSOS_PushToTimedSolenoidStack(SOL_4_RIGHT_DROP_TARGET_RESET, 10, currentTime + 500);

  for (int count = 0; count < 4; count++) {
    bonusMultipliers[count] = 1;
    bonuses[count] = 0;
  }
  memset(scores, 0, 4 * sizeof(unsigned long));

  samePlayerShootsAgain = false;
  currentBallInPlay = 1;
  currentNumPlayers = 1;
  currentPlayer = 0;
  ShowPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}

int MachineState::InitNewBall(bool curStateChanged, byte playerNum, int ballNum) {
  if (curStateChanged) {
    ShowLamps(LAMP_COLLECTION_BONUS_ALL, true);

    BSOS_SetDisplayCredits(Credits, true);
    samePlayerShootsAgain = false;

    BSOS_SetDisplayBallInPlay(ballNum);
    BSOS_SetLampState(LAMP_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      BSOS_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    GlobalGameMode.SetGameMode(GAME_MODE_SKILL_SHOT);

    ExtraBallCollected = false;

    if (BSOS_ReadSingleSwitchState(SW_OUTHOLE)) {
      BSOS_PushToTimedSolenoidStack(SOL_OUTHOLE_KICKER, 4, currentTime + 600);
    }

    // Reset progress unless holdover awards
    bonuses[currentPlayer] = 0;
    bonusMultipliers[currentPlayer] = 1;

    scoreMultiplier = 1;
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

unsigned long MachineState::GetCurrentTime() {
  return currentTime;
}

unsigned long MachineState::GetLastTiltWarningTime() {
  return lastTiltWarningTime;
}

unsigned long MachineState::GetScore(byte player) {
  if (player == 0xFF) player = currentPlayer;
  return scores[player];
}

void MachineState::AwardExtraBall() {
  if (ExtraBallCollected) return;

  ExtraBallCollected = true;
  samePlayerShootsAgain = true;
  BSOS_SetLampState(LAMP_SHOOT_AGAIN, samePlayerShootsAgain);
}

void MachineState::DecreaseBonus(byte amountToSubtract) {
  bonuses[currentPlayer] -= amountToSubtract;
}

void MachineState::IncreaseBonus(byte amountToAdd) {
  bonuses[currentPlayer] += amountToAdd;
  if (bonuses[currentPlayer] >= MAX_DISPLAY_BONUS) {
    bonuses[currentPlayer] = MAX_DISPLAY_BONUS;
  }
}

void MachineState::IncreaseBonusMultiplier() {
  boolean soundPlayed = false;
  if (bonusMultipliers[currentPlayer] < 5) {
    bonusMultipliers[currentPlayer] += 1;
    BonusXAnimationStart = currentTime;

    if (bonusMultipliers[currentPlayer] == 4) {
      bonusMultipliers[currentPlayer] += 1;
    }
  }
}

void MachineState::IncreaseCredits(boolean playSound, byte numToAdd) {
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

void MachineState::IncreaseScore(unsigned long amountToAdd) {
  scores[currentPlayer] += amountToAdd;
}


void MachineState::SetBonus(byte value) {
  bonuses[currentPlayer] = value;
}

void MachineState::SetCurrentTime(unsigned long value) {
  currentTime = value;
}

void MachineState::SetLastTiltWarningTime(unsigned long value) {
  lastTiltWarningTime = value;
}

void MachineState::SetMachineState(byte id) {
  if (id != machineStateId) {
    machineStateChanged = true;
  } else {
    machineStateChanged = false;
  }

  machineStateId = id;
}

void MachineState::SetNumberOfPlayers(byte value) {
  currentNumPlayers = value;
}

void MachineState::SetScore(unsigned long value, byte player) {
  if (player == 0xFF) player = currentPlayer;
  scores[player] = value;
}

void MachineState::WriteCoinToAudit(byte switchHit) {
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
