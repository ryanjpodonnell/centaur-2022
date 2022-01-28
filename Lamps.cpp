#include "SharedVariables.h"

LampsHelper::LampsHelper() {
}

void LampsHelper::showLamps(byte lampCollection, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  byte* lampsPointer;

  switch(lampCollection) {
    case LAMP_COLLECTION_BONUS_ALL:
      lampsPointer = bonusAllLamps_;
      break;
    case LAMP_COLLECTION_BONUS_MIDDLE_RING:
      lampsPointer = bonusMiddleRingLamps_;
      break;
    case LAMP_COLLECTION_BONUS_OUTER_RING:
      lampsPointer = bonusOuterRingLamps_;
      break;
    case LAMP_COLLECTION_RING_4:
      lampsPointer = ring4Lamps_;
      break;
    case LAMP_COLLECTION_RING_5:
      lampsPointer = ring5Lamps_;
      break;
    case LAMP_COLLECTION_RING_6:
      lampsPointer = ring6Lamps_;
      break;
  }

  for(byte i = 0; lampsPointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(lampsPointer[i], 1);
  }
}

void LampsHelper::showLamp(byte lamp, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  BSOS_SetLampState(lamp, 1);
}

void LampsHelper::showAllLamps() {
  BSOS_TurnOffAllLamps();

  for (byte count = 0; count < BSOS_MAX_LAMPS; count++) {
    BSOS_SetLampState(count, 1);
  }
}
