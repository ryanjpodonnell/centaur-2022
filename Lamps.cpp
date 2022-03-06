#include "SharedVariables.h"

LampsHelper::LampsHelper() {
}

void LampsHelper::hideAllLamps() {
  BSOS_TurnOffAllLamps();
}

void LampsHelper::hideLamp(byte lamp) {
  BSOS_SetLampState(lamp, 0);
}

void LampsHelper::hideLamps(byte lampCollection) {
  byte* pointer = lampsPointer(lampCollection);
  for(byte i = 0; pointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(pointer[i], 0);
  }
}

void LampsHelper::showAllLamps() {
  for (byte count = 0; count < BSOS_MAX_LAMPS; count++) {
    BSOS_SetLampState(count, 1);
  }
}

void LampsHelper::showBonusLamps(byte playerNumber) {
  byte bonus = g_machineState.bonus(playerNumber);

  for(byte itr = 0; itr < sizeof(descendingBonusValues_); itr++) {
    if (bonus >= descendingBonusValues_[itr]) {
      showLamp(descendingBonusLamps_[itr]);
      bonus -= descendingBonusValues_[itr];
    } else {
      hideLamp(descendingBonusLamps_[itr]);
    }
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

void LampsHelper::showLamps(byte lampCollection, bool clearAllLamps) {
  if (clearAllLamps) BSOS_TurnOffAllLamps();

  byte* pointer = lampsPointer(lampCollection);
  for(byte i = 0; pointer[i] != LAMP_TERMINATOR; i++) {
    BSOS_SetLampState(pointer[i], 1);
  }
}


/*********************************************************************
    Private
*********************************************************************/
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
      pointer = topRolloverLamps_;
      break;
    case LAMP_COLLECTION_ALL_ROLLOVERS:
      pointer = allRolloverLamps_;
      break;
    case LAMP_COLLECTION_CAPTIVE_ORBS:
      pointer = captiveOrbsLamps_;
      break;
  }

  return pointer;
}
