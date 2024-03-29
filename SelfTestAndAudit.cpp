#include "SharedVariables.h"

SelfTestAndAudit::SelfTestAndAudit() {
  lastResetPress_        = 0;
  lastSelfTestChange_    = 0;
  lastSolTestTime_       = 0;
  nextSpeedyValueChange_ = 0;
  numSpeedyChanges_      = 0;
  resetHold_             = 0;
  savedValue_            = 0;
  curValue_              = 0;
  solenoidCycle_         = true;
}

int SelfTestAndAudit::run(int curState, boolean curStateChanged) {
  if (curStateChanged) {
    if (DEBUG_MESSAGES) Serial.write("Entering Self Test And Audit State\n\r");
    g_soundHelper.stopAudio();
  }

  int returnState = curState;

  if (curState >= MACHINE_STATE_TEST_CHUTE_3_COINS) {
    returnState = runBase(returnState, curStateChanged);
  }

  if (returnState == MACHINE_STATE_ATTRACT) {
    for (int count = 0; count < 4; count++) {
      BSOS_SetDisplay(count, 0);
      BSOS_SetDisplayBlank(count, 0x7F);
    }

    BSOS_SetDisplayCredits(g_machineState.credits());
    BSOS_SetDisplayBallInPlay(0);

    g_machineState.readStoredParameters();
  }

  return returnState;
}

/*********************************************************************
    Private
*********************************************************************/
int SelfTestAndAudit::runBase(int curState, boolean curStateChanged) {
  boolean        resetBeingHeld      = false;
  boolean        resetDoubleClick    = false;
  byte           curSwitch           = BSOS_PullFirstFromSwitchStack();
  byte           resetSwitch         = SW_CREDIT_BUTTON;
  byte           slamSwitch          = SW_SLAM;
  int            returnState         = curState;
  unsigned long  currentTime         = g_machineState.currentTime();
  unsigned short auditNumStartByte   = 0;

  if (curSwitch == resetSwitch) {
    resetHold_ = currentTime;
    if ((currentTime - lastResetPress_) < 400) {
      curSwitch        = SWITCH_STACK_EMPTY;
      resetDoubleClick = true;
    }
    lastResetPress_ = currentTime;
  }

  if (resetHold_ != 0 && !BSOS_ReadSingleSwitchState(resetSwitch)) {
    nextSpeedyValueChange_ = 0;
    resetHold_             = 0;
  }

  if (resetHold_ != 0 && (currentTime - resetHold_) > 1300) {
    resetBeingHeld = true;
    if (nextSpeedyValueChange_ == 0) {
      nextSpeedyValueChange_ = currentTime;
      numSpeedyChanges_      = 0;
    }
  }

  if (slamSwitch != 0xFF && curSwitch == slamSwitch) {
    returnState = MACHINE_STATE_ATTRACT;
  }
  
  if (curSwitch == SW_SELF_TEST_SWITCH && (currentTime - lastSelfTestChange_) > 250) {
    returnState -= 1;
    if (returnState == MACHINE_STATE_TEST_DONE) returnState = MACHINE_STATE_ATTRACT;
    lastSelfTestChange_ = currentTime;
  }

  if (curStateChanged) {
    BSOS_SetCoinLockout(false);
    
    for (int count = 0; count < 4; count++) {
      BSOS_SetDisplay(count, 0);
      BSOS_SetDisplayBlank(count, 0x00);
    }

    if (curState <= MACHINE_STATE_TEST_FREE_PLAY) {
      BSOS_SetDisplayCredits(MACHINE_STATE_TEST_SWITCHES - curState);
      BSOS_SetDisplayBallInPlay(0, false);
    }
  }

  if (curState==MACHINE_STATE_TEST_LIGHTS) {
    if (curStateChanged) {
      BSOS_DisableSolenoidStack();
      BSOS_SetDisableFlippers(true);
      BSOS_SetDisplayCredits(0);
      BSOS_SetDisplayBallInPlay(1);
      BSOS_TurnOffAllLamps();
      for (int count = 0; count < BSOS_MAX_LAMPS; count++) {
        BSOS_SetLampState(count, 1, 0, 500);
      }
      curValue_ = 99;
      BSOS_SetDisplay(0, curValue_, true);
    }
    if (curSwitch == resetSwitch || resetDoubleClick) {
      curValue_ += 1;
      if (curValue_ > 99) curValue_ = 0;
      if (curValue_ == BSOS_MAX_LAMPS) {
        curValue_ = 99;
        for (int count = 0; count < BSOS_MAX_LAMPS; count++) {
          BSOS_SetLampState(count, 1, 0, 500);
        }
      } else {
        BSOS_TurnOffAllLamps();
        BSOS_SetLampState(curValue_, 1, 0, 500);
      }      
      BSOS_SetDisplay(0, curValue_, true);
    }    

  } else if (curState == MACHINE_STATE_TEST_DISPLAYS) {
    if (curStateChanged) {
      BSOS_TurnOffAllLamps();
      BSOS_SetDisplayCredits(0);
      BSOS_SetDisplayBallInPlay(2);
      for (int count = 0; count < 4; count++) {
        BSOS_SetDisplayBlank(count, 0x3F);
      }
      curValue_ = 0;
    }
    if (curSwitch == resetSwitch || resetDoubleClick) {
      curValue_ += 1;
      if (curValue_ > 30) curValue_ = 0;
    }    
    BSOS_CycleAllDisplays(currentTime, curValue_);

  } else if (curState == MACHINE_STATE_TEST_SOLENOIDS) {
    if (curStateChanged) {
      BSOS_TurnOffAllLamps();
      lastSolTestTime_ = currentTime;
      BSOS_EnableSolenoidStack(); 
      BSOS_SetDisableFlippers(false);
      BSOS_SetDisplayCredits(0);
      BSOS_SetDisplayBallInPlay(3);
      solenoidCycle_ = true;
      savedValue_ = 0;
      BSOS_PushToSolenoidStack(savedValue_, 5);
    } 
    if (curSwitch == resetSwitch || resetDoubleClick) {
      solenoidCycle_ = (solenoidCycle_) ? false : true;
    }

    if ((currentTime-lastSolTestTime_) > 1000) {
      if (solenoidCycle_) {
        savedValue_ += 1;
        if (savedValue_ > 14) savedValue_ = 0;
      }
      BSOS_PushToSolenoidStack(savedValue_, 3);
      BSOS_SetDisplay(0, savedValue_, true);
      lastSolTestTime_ = currentTime;
    }
    
  } else if (curState == MACHINE_STATE_TEST_SWITCHES) {
    if (curStateChanged) {
      BSOS_TurnOffAllLamps();
      BSOS_DisableSolenoidStack(); 
      BSOS_SetDisableFlippers(true);
      BSOS_SetDisplayCredits(0);
      BSOS_SetDisplayBallInPlay(4);
    }

    byte displayOutput = 0;
    for (byte switchCount = 0; switchCount < 40 && displayOutput < 4; switchCount++) {
      if (BSOS_ReadSingleSwitchState(switchCount)) {
        BSOS_SetDisplay(displayOutput, switchCount, true);
        displayOutput += 1;
      }
    }

    if (displayOutput < 4) {
      for (int count = displayOutput; count < 4; count++) {
        BSOS_SetDisplayBlank(count, 0x00);
      }
    }

  } else if (curState == MACHINE_STATE_TEST_FREE_PLAY) {
    BSOS_SetDisplayBallInPlay(5);
    if (curStateChanged) {
      savedValue_ = BSOS_ReadByteFromEEProm(BSOS_FREE_PLAY_EEPROM_START_BYTE);
      BSOS_SetDisplay(0, savedValue_, true);
    }

    if (curSwitch == resetSwitch || resetDoubleClick) {
      savedValue_ = (savedValue_ + 1) % (MAXIMUM_NUMBER_OF_COINS_PER_CREDIT + 1);
      BSOS_SetDisplay(0, savedValue_, true);
      BSOS_WriteByteToEEProm(BSOS_FREE_PLAY_EEPROM_START_BYTE, savedValue_& 0x000000FF);
    }

  } else if (curState == MACHINE_STATE_TEST_HISCORE) {
    BSOS_SetDisplayBallInPlay(6);
    if (curStateChanged) {
      savedValue_ = BSOS_ReadULFromEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE);
      BSOS_SetDisplay(0, savedValue_, true);
    }

    if (curSwitch == resetSwitch) {
      savedValue_ += 1000;
      BSOS_SetDisplay(0, savedValue_, true);
      BSOS_WriteULToEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, savedValue_);
    }

    if (resetBeingHeld && (currentTime >= nextSpeedyValueChange_)) {
      savedValue_ += 1000;
      BSOS_SetDisplay(0, savedValue_, true);
      if (numSpeedyChanges_ < 6) nextSpeedyValueChange_ = currentTime + 400;
      else if (numSpeedyChanges_ < 50) nextSpeedyValueChange_ = currentTime + 50;
      else nextSpeedyValueChange_ = currentTime + 10;
      numSpeedyChanges_ += 1;
    }

    if (!resetBeingHeld && numSpeedyChanges_ > 0) {
      BSOS_WriteULToEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, savedValue_);
      numSpeedyChanges_ = 0;
    }
    
    if (resetDoubleClick) {
      savedValue_ = 0;
      BSOS_SetDisplay(0, savedValue_, true);
      BSOS_WriteULToEEProm(BSOS_HIGHSCORE_EEPROM_START_BYTE, savedValue_);
    }

  } else if (curState == MACHINE_STATE_TEST_CREDITS) {
    BSOS_SetDisplayBallInPlay(7);
    if (curStateChanged) {
      savedValue_ = BSOS_ReadByteFromEEProm(BSOS_CREDITS_EEPROM_BYTE);
      BSOS_SetDisplay(0, savedValue_, true);
    }

    if (curSwitch == resetSwitch || resetDoubleClick) {
      savedValue_ = (savedValue_ + 1) % MAXIMUM_NUMBER_OF_CREDITS;
      BSOS_SetDisplay(0, savedValue_, true);
      BSOS_WriteByteToEEProm(BSOS_CREDITS_EEPROM_BYTE, savedValue_ & 0x000000FF);
    }

  } else if (curState == MACHINE_STATE_TEST_TOTAL_PLAYS) {
    BSOS_SetDisplayBallInPlay(8);
    auditNumStartByte = BSOS_TOTAL_PLAYS_EEPROM_START_BYTE;

  } else if (curState == MACHINE_STATE_TEST_TOTAL_REPLAYS) {
    BSOS_SetDisplayBallInPlay(9);
    auditNumStartByte = BSOS_TOTAL_REPLAYS_EEPROM_START_BYTE;

  } else if (curState == MACHINE_STATE_TEST_HISCORE_BEAT) {
    BSOS_SetDisplayBallInPlay(10);
    auditNumStartByte = BSOS_TOTAL_HISCORE_BEATEN_START_BYTE;

  } else if (curState == MACHINE_STATE_TEST_CHUTE_2_COINS) {
    BSOS_SetDisplayBallInPlay(11);
    auditNumStartByte = BSOS_CHUTE_2_COINS_START_BYTE;

  } else if (curState == MACHINE_STATE_TEST_CHUTE_1_COINS) {
    BSOS_SetDisplayBallInPlay(12);
    auditNumStartByte = BSOS_CHUTE_1_COINS_START_BYTE;

  } else if (curState == MACHINE_STATE_TEST_CHUTE_3_COINS) {
    BSOS_SetDisplayBallInPlay(13);
    auditNumStartByte = BSOS_CHUTE_3_COINS_START_BYTE;
  }

  if (auditNumStartByte) {
    if (curStateChanged) {
      savedValue_ = BSOS_ReadULFromEEProm(auditNumStartByte);
      BSOS_SetDisplay(0, savedValue_, true);
    }

    if (resetDoubleClick) {
      savedValue_ = 0;
      BSOS_SetDisplay(0, savedValue_, true);
      BSOS_WriteULToEEProm(auditNumStartByte, savedValue_);
    }
    
  }
  
  return returnState;
}
