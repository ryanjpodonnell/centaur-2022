#include "SharedVariables.h"

LampsHelper::LampsHelper() {
}

void LampsHelper::hideLamps(byte lampCollection) {
  byte* pointer = lampsPointer(lampCollection);
  for(byte i = 0; pointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(pointer[i], 0);
  }
}

void LampsHelper::hideLamp(byte lamp) {
  BSOS_SetLampState(lamp, 0);
}

void LampsHelper::showLamps(byte lampCollection, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  byte* pointer = lampsPointer(lampCollection);
  for(byte i = 0; pointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(pointer[i], 1);
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

byte* LampsHelper::lampsPointer(byte lampCollection) {
  byte* pointer;
  switch(lampCollection) {
    case LAMP_COLLECTION_BONUS_ALL:
      pointer = bonusAllLamps_;
      break;
    case LAMP_COLLECTION_BONUS_MIDDLE_RING:
      pointer = bonusMiddleRingLamps_;
      break;
    case LAMP_COLLECTION_BONUS_OUTER_RING:
      pointer = bonusOuterRingLamps_;
      break;
    case LAMP_COLLECTION_RING_4:
      pointer = ring4Lamps_;
      break;
    case LAMP_COLLECTION_RING_5:
      pointer = ring5Lamps_;
      break;
    case LAMP_COLLECTION_RING_6:
      pointer = ring6Lamps_;
      break;
    case LAMP_COLLECTION_RING_7:
      pointer = ring7Lamps_;
      break;
    case LAMP_COLLECTION_RING_8:
      pointer = ring8Lamps_;
      break;
    case LAMP_COLLECTION_RING_9:
      pointer = ring9Lamps_;
      break;
    case LAMP_COLLECTION_RING_10:
      pointer = ring10Lamps_;
      break;
    case LAMP_COLLECTION_RING_11:
      pointer = ring11Lamps_;
      break;
    case LAMP_COLLECTION_RING_12:
      pointer = ring12Lamps_;
      break;
    case LAMP_COLLECTION_RING_13:
      pointer = ring13Lamps_;
      break;
    case LAMP_COLLECTION_RING_14:
      pointer = ring14Lamps_;
      break;
    case LAMP_COLLECTION_TOP_ROLLOVERS:
      pointer = rolloverLamps;
      break;
  }

  return pointer;
}
