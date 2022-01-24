#include <Arduino.h>
#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "SharedVariables.h"
#include "StoredParameters.h"

void ReadStoredParameters() {
  HighScore = BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, 10000);
  Credits = BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE);
  if (Credits > MaximumCredits) Credits = MaximumCredits;
}

