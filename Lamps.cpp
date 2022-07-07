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

void LampsHelper::showBonusLamps(byte value) {
  for(byte itr = 0; itr < sizeof(descendingBonusValues_); itr++) {
    if (value >= descendingBonusValues_[itr]) {
      showLamp(descendingBonusLamps_[itr]);
      value -= descendingBonusValues_[itr];
    } else {
      hideLamp(descendingBonusLamps_[itr]);
    }
  }
}

void LampsHelper::showBonusMultiplierLamps(byte value) {
  if (value >= 2) showLamp(LAMP_2X_BONUS);
  if (value >= 3) showLamp(LAMP_3X_BONUS);
  if (value >= 4) showLamp(LAMP_4X_BONUS);
  if (value >= 5) showLamp(LAMP_5X_BONUS);
}

void LampsHelper::showLamp(byte lamp, bool flash) {
  if (flash) {
    BSOS_SetLampState(lamp, 1, 0, FLASH_FREQUENCY);
  } else {
    BSOS_SetLampState(lamp, 1);
  }
}

void LampsHelper::showLamps(byte lampCollection, bool flash) {
  byte* pointer = lampsPointer(lampCollection);
  for(byte i = 0; pointer[i] != LAMP_TERMINATOR; i++) {
    showLamp(pointer[i], flash);
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
    case LAMP_COLLECTION_RIGHT_DROP_TARGET_ARROWS:
      pointer = rightDropTargetArrowsLamps_;
      break;
    case LAMP_COLLECTION_ORBS_DROP_TARGET_ARROWS:
      pointer = orbsDropTargetArrowsLamps_;
      break;
    case LAMP_COLLECTION_GENERAL_ILLUMINATION:
      pointer = playfieldGeneralIllumination_;
      break;
    case LAMP_COLLECTION_QUEENS_CHAMBER_HURRY_UP:
      pointer = queensChamberHurryUpLamps_;
      break;
    case LAMP_COLLECTION_ORB_FEATURE:
      pointer = orbFeatureLamps_;
      break;
    case LAMP_COLLECTION_SEQUENCE_FEATURE:
      pointer = sequenceFeatureLamps_;
      break;
    case LAMP_COLLECTION_CHAMBER_FEATURE:
      pointer = chamberFeatureLamps_;
      break;
    case LAMP_COLLECTION_GUARDIAN_FEATURE:
      pointer = guardianFeatureLamps_;
      break;
    case LAMP_COLLECTION_BONUS_FEATURE:
      pointer = bonusFeatureLamps_;
      break;
    case LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_1:
      pointer = bonusCountdownStep1_;
      break;
    case LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_2:
      pointer = bonusCountdownStep2_;
      break;
    case LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_3:
      pointer = bonusCountdownStep3_;
      break;
    case LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_4:
      pointer = bonusCountdownStep4_;
      break;
    case LAMP_COLLECTION_BONUS_COUNTDOWN_STEP_5:
      pointer = bonusCountdownStep5_;
      break;
    case LAMP_COLLECTION_BONUS_RESET_ARROW:
      pointer = bonusResetArrow_;
      break;
    case LAMP_COLLECTION_BONUS_ORBS_DROPS_ARROW:
      pointer = bonusOrbsDropsArrow_;
      break;
    case LAMP_COLLECTION_BONUS_RIGHT_DROPS_ARROW:
      pointer = bonusRightDropsArrow_;
      break;
    case LAMP_COLLECTION_QUEENS_CHAMBER_GI:
      pointer = queensChamberGI_;
      break;
    case LAMP_COLLECTION_GUARDIAN_ROLLOVERS:
      pointer = guardianRolloverLamps_;
      break;
  }

  return pointer;
}
