#include <Arduino.h>
#include "BallySternOS.h"
#include "Lights.h"
#include "PinballMachineBase.h"

void ShowLamps(int lampCollection) {
  BSOS_TurnOffAllLamps();

  int* lampsPointer;

  switch(lampCollection) {
    case LAMP_COLLECTION_BONUS_ALL:
      lampsPointer = bonusAllLights;
      break;
    case LAMP_COLLECTION_BONUS_CENTER:
      lampsPointer = bonusCenterLights;
      break;
    case LAMP_COLLECTION_BONUS_MIDDLE_RING:
      lampsPointer = bonusMiddleRingLights;
      break;
    case LAMP_COLLECTION_BONUS_OUTER_RING:
      lampsPointer = bonusOuterRingLights;
      break;
  }

  for(int i = 0; lampsPointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(lampsPointer[i], 1);
  }
}
