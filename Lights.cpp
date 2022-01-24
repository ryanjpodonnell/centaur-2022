#include <Arduino.h>
#include "BallySternOS.h"
#include "Lights.h"
#include "PinballMachineBase.h"

void ShowLamps(int lampCollection) {
  int* lampsPointer;

  switch(lampCollection) {
    case LAMP_COLLECTION_BONUS_ALL:
      lampsPointer = allBonusLights;
      break;
  }

  for(int i = 0; lampsPointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(lampsPointer[i], 1);
  }
}
