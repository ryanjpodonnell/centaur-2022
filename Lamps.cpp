#include <Arduino.h>
#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "Lamps.h"
#include "PinballMachineBase.h"

void ShowLamps(int lampCollection, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  int* lampsPointer;

  switch(lampCollection) {
    case LAMP_COLLECTION_BONUS_ALL:
      lampsPointer = bonusAllLamps;
      break;
    case LAMP_COLLECTION_BONUS_MIDDLE_RING:
      lampsPointer = bonusMiddleRingLamps;
      break;
    case LAMP_COLLECTION_BONUS_OUTER_RING:
      lampsPointer = bonusOuterRingLamps;
      break;
  }

  for(int i = 0; lampsPointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(lampsPointer[i], 1);
  }
}

void ShowLamp(int lamp, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  BSOS_SetLampState(lamp, 1);
}
