#include "SharedVariables.h"

LampsHelper::LampsHelper() {
}

void LampsHelper::hideLamp(byte lamp) {
  BSOS_SetLampState(lamp, 0);
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
    case LAMP_COLLECTION_RING_7:
      lampsPointer = ring7Lamps_;
      break;
    case LAMP_COLLECTION_RING_8:
      lampsPointer = ring8Lamps_;
      break;
    case LAMP_COLLECTION_RING_9:
      lampsPointer = ring9Lamps_;
      break;
    case LAMP_COLLECTION_RING_10:
      lampsPointer = ring10Lamps_;
      break;
    case LAMP_COLLECTION_RING_11:
      lampsPointer = ring11Lamps_;
      break;
    case LAMP_COLLECTION_RING_12:
      lampsPointer = ring12Lamps_;
      break;
    case LAMP_COLLECTION_RING_13:
      lampsPointer = ring13Lamps_;
      break;
    case LAMP_COLLECTION_RING_14:
      lampsPointer = ring14Lamps_;
      break;
  }

  for(byte i = 0; lampsPointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(lampsPointer[i], 1);
  }
}

void LampsHelper::showLamp(byte lamp, bool flash, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  if (flash) {
    BSOS_SetLampState(lamp, 1, 0, 500);
  } else {
    BSOS_SetLampState(lamp, 1);
  }
}

void LampsHelper::showAllLamps() {
  BSOS_TurnOffAllLamps();

  for (byte count = 0; count < BSOS_MAX_LAMPS; count++) {
    BSOS_SetLampState(count, 1);
  }
}
