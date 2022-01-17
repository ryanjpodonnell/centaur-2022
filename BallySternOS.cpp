/**************************************************************************
 *     This file is part of the Bally/Stern OS for Arduino Project.

    I, Dick Hamill, the author of this program disclaim all copyright
    in order to make this program freely available in perpetuity to
    anyone who would like to use it. Dick Hamill, 6/1/2020

    BallySternOS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BallySternOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.
 */

 
#include <Arduino.h>
#include <EEPROM.h>
//#define DEBUG_MESSAGES    1
#define BALLY_STERN_CPP_FILE
#include "BSOS_Config.h"
#include "BallySternOS.h"


#if !defined(BSOS_SWITCH_DELAY_IN_MICROSECONDS) || !defined(BSOS_TIMING_LOOP_PADDING_IN_MICROSECONDS)
#error "Must define BSOS_SWITCH_DELAY_IN_MICROSECONDS and BSOS_TIMING_LOOP_PADDING_IN_MICROSECONDS in BSOS_Config.h"
#endif

// Global variables
volatile byte DisplayDigits[5][BALLY_STERN_OS_NUM_DIGITS];
volatile byte DisplayDigitEnable[5];
volatile boolean DisplayOffCycle = false;
volatile byte CurrentDisplayDigit=0;
volatile byte LampStates[BSOS_NUM_LAMP_BITS], LampDim0[BSOS_NUM_LAMP_BITS], LampDim1[BSOS_NUM_LAMP_BITS];
volatile byte LampFlashPeriod[BSOS_MAX_LAMPS];
byte DimDivisor1 = 2;
byte DimDivisor2 = 3;

volatile byte SwitchesMinus2[5];
volatile byte SwitchesMinus1[5];
volatile byte SwitchesNow[5];

#define SOLENOID_STACK_SIZE 64
#define SOLENOID_STACK_EMPTY 0xFF
volatile byte SolenoidStackFirst;
volatile byte SolenoidStackLast;
volatile byte SolenoidStack[SOLENOID_STACK_SIZE];
boolean SolenoidStackEnabled = true;
volatile byte CurrentSolenoidByte = 0xFF;

#define TIMED_SOLENOID_STACK_SIZE 32
struct TimedSolenoidEntry {
  byte inUse;
  unsigned long pushTime;
  byte solenoidNumber;
  byte numPushes;
  byte disableOverride;
};
TimedSolenoidEntry TimedSolenoidStack[32];

#define SWITCH_STACK_SIZE   64
#define SWITCH_STACK_EMPTY  0xFF
volatile byte SwitchStackFirst;
volatile byte SwitchStackLast;
volatile byte SwitchStack[SWITCH_STACK_SIZE];

#define ADDRESS_U10_A           0x88
#define ADDRESS_U10_A_CONTROL   0x89
#define ADDRESS_U10_B           0x8A
#define ADDRESS_U10_B_CONTROL   0x8B
#define ADDRESS_U11_A           0x90
#define ADDRESS_U11_A_CONTROL   0x91
#define ADDRESS_U11_B           0x92
#define ADDRESS_U11_B_CONTROL   0x93
#define ADDRESS_SB100           0xA0
#define ADDRESS_SB100_CHIMES    0xC0
#define ADDRESS_SB300_SQUARE_WAVES  0xA0
#define ADDRESS_SB300_ANALOG        0xC0


// Rev 3 connections
// Pin D2 = IRQ
// Pin D3 = CLOCK
// Pin D4 = VMA
// Pin D5 = R/W
// Pin D6-12 = D0-D6
// Pin D13 = SWITCH
// Pin D14 = HALT
// Pin D15 = D7
// Pin D16-30 = A0-A14

#if defined(__AVR_ATmega328P__)
#error "BALLY_STERN_OS_HARDWARE_REV 3 requires ATMega2560, check BSOS_Config.h and adjust settings"
#endif

void BSOS_DataWrite(int address, byte data) {
  // Set data pins to output
  DDRH = DDRH | 0x78;
  DDRB = DDRB | 0x70;
  DDRJ = DDRJ | 0x01;

  // Set R/W to LOW
  PORTE = (PORTE & 0xF7);

  // Put data on pins
  // Lower Nibble goes on PortH3 through H6
  PORTH = (PORTH&0x87) | ((data&0x0F)<<3);
  // Bits 4-6 go on PortB4 through B6
  PORTB = (PORTB&0x8F) | ((data&0x70));
  // Bit 7 goes on PortJ0
  PORTJ = (PORTJ&0xFE) | (data>>7);  

  // Set up address lines
  PORTH = (PORTH & 0xFC) | ((address & 0x0001)<<1) | ((address & 0x0002)>>1); // A0-A1
  PORTD = (PORTD & 0xF0) | ((address & 0x0004)<<1) | ((address & 0x0008)>>1) | ((address & 0x0010)>>3) | ((address & 0x0020)>>5); // A2-A5
  PORTA = ((address & 0x3FC0)>>6); // A6-A13
  PORTC = (PORTC & 0x3F) | ((address & 0x4000)>>7) | ((address & 0x8000)>>9); // A14-A15

  // Wait for a falling edge of the clock
  while((PINE & 0x20));

  // Pulse VMA over one clock cycle
  // Set VMA ON
  PORTG = PORTG | 0x20;

  // Wait while clock is low
  while(!(PINE & 0x20));

  // Set VMA OFF
  PORTG = PORTG & 0xDF;

  // Unset address lines
  PORTH = (PORTH & 0xFC);
  PORTD = (PORTD & 0xF0);
  PORTA = 0;
  PORTC = (PORTC & 0x3F);
  
  // Set R/W back to HIGH
  PORTE = (PORTE | 0x08);

  // Set data pins to input
  DDRH = DDRH & 0x87;
  DDRB = DDRB & 0x8F;
  DDRJ = DDRJ & 0xFE;
}


byte BSOS_DataRead(int address) {
  
  // Set data pins to input
  DDRH = DDRH & 0x87;
  DDRB = DDRB & 0x8F;
  DDRJ = DDRJ & 0xFE;

  // Set R/W to HIGH
  DDRE = DDRE | 0x08;
  PORTE = (PORTE | 0x08);

  // Set up address lines
  PORTH = (PORTH & 0xFC) | ((address & 0x0001)<<1) | ((address & 0x0002)>>1); // A0-A1
  PORTD = (PORTD & 0xF0) | ((address & 0x0004)<<1) | ((address & 0x0008)>>1) | ((address & 0x0010)>>3) | ((address & 0x0020)>>5); // A2-A5
  PORTA = ((address & 0x3FC0)>>6); // A6-A13
  PORTC = (PORTC & 0x3F) | ((address & 0x4000)>>7) | ((address & 0x8000)>>9); // A14-A15

  // Wait for a falling edge of the clock
  while((PINE & 0x20));

  // Pulse VMA over one clock cycle
  // Set VMA ON
  PORTG = PORTG | 0x20;

  // Wait a full clock cycle to make sure data lines are ready
  // (important for faster clocks)
  // Wait while clock is low
  while(!(PINE & 0x20));

  // Wait for a falling edge of the clock
  while((PINE & 0x20));
  
  // Wait while clock is low
  while(!(PINE & 0x20));

  byte inputData;
  inputData = (PINH & 0x78)>>3;
  inputData |= (PINB & 0x70);
  inputData |= PINJ << 7;

  // Set VMA OFF
  PORTG = PORTG & 0xDF;

  // Set R/W to LOW
  PORTE = (PORTE & 0xF7);

  // Unset address lines
  PORTH = (PORTH & 0xFC);
  PORTD = (PORTD & 0xF0);
  PORTA = 0;
  PORTC = (PORTC & 0x3F);

  return inputData;
}


void WaitClockCycle(int numCycles=1) {
  for (int count=0; count<numCycles; count++) {
    // Wait while clock is low
    while(!(PINE & 0x20));
  
    // Wait for a falling edge of the clock
    while((PINE & 0x20));
  }
}


void TestLightOn() {
  BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL) | 0x08);
}


void TestLightOff() {
  BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL) & 0xF7);
}


void InitializeU10PIA() {
  // CA1 - Self Test Switch
  // CB1 - zero crossing detector
  // CA2 - NOR'd with display latch strobe
  // CB2 - lamp strobe 1
  // PA0-7 - output for switch bank, lamps, and BCD
  // PB0-7 - switch returns
  
  BSOS_DataWrite(ADDRESS_U10_A_CONTROL, 0x38);
  // Set up U10A as output
  BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
  // Set bit 3 to write data
  BSOS_DataWrite(ADDRESS_U10_A_CONTROL, BSOS_DataRead(ADDRESS_U10_A_CONTROL)|0x04);
  // Store F0 in U10A Output
  BSOS_DataWrite(ADDRESS_U10_A, 0xF0);
  
  BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x33);
  // Set up U10B as input
  BSOS_DataWrite(ADDRESS_U10_B, 0x00);
  // Set bit 3 so future reads will read data
  BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL)|0x04);

}


void InitializeU11PIA() {
  // CA1 - Display interrupt generator
  // CB1 - test connector pin 32
  // CA2 - lamp strobe 2
  // CB2 - solenoid bank select
  // PA0-7 - display digit enable
  // PB0-7 - solenoid data

  BSOS_DataWrite(ADDRESS_U11_A_CONTROL, 0x31);
  // Set up U11A as output
  BSOS_DataWrite(ADDRESS_U11_A, 0xFF);
  // Set bit 3 to write data
  BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL)|0x04);
  // Store 00 in U11A Output
  BSOS_DataWrite(ADDRESS_U11_A, 0x00);
  
  BSOS_DataWrite(ADDRESS_U11_B_CONTROL, 0x30);
  // Set up U11B as output
  BSOS_DataWrite(ADDRESS_U11_B, 0xFF);
  // Set bit 3 so future reads will read data
  BSOS_DataWrite(ADDRESS_U11_B_CONTROL, BSOS_DataRead(ADDRESS_U11_B_CONTROL)|0x04);
  // Store 9F in U11B Output
  BSOS_DataWrite(ADDRESS_U11_B, 0x9F);
  CurrentSolenoidByte = 0x9F;
  
}


int SpaceLeftOnSwitchStack() {
  if (SwitchStackFirst>=SWITCH_STACK_SIZE || SwitchStackLast>=SWITCH_STACK_SIZE) return 0;
  if (SwitchStackLast>=SwitchStackFirst) return ((SWITCH_STACK_SIZE-1) - (SwitchStackLast-SwitchStackFirst));
  return (SwitchStackFirst - SwitchStackLast) - 1;
}


void PushToSwitchStack(byte switchNumber) {
  if ((switchNumber>39 && switchNumber!=SW_SELF_TEST_SWITCH)) return;

  // If the switch stack last index is out of range, then it's an error - return
  if (SpaceLeftOnSwitchStack()==0) return;

  // Self test is a special case - there's no good way to debounce it
  // so if it's already first on the stack, ignore it
  if (switchNumber==SW_SELF_TEST_SWITCH) {
    if (SwitchStackLast!=SwitchStackFirst && SwitchStack[SwitchStackFirst]==SW_SELF_TEST_SWITCH) return;
  }

  SwitchStack[SwitchStackLast] = switchNumber;
  
  SwitchStackLast += 1;
  if (SwitchStackLast==SWITCH_STACK_SIZE) {
    // If the end index is off the end, then wrap
    SwitchStackLast = 0;
  }
}


byte BSOS_PullFirstFromSwitchStack() {
  // If first and last are equal, there's nothing on the stack
  if (SwitchStackFirst==SwitchStackLast) return SWITCH_STACK_EMPTY;

  byte retVal = SwitchStack[SwitchStackFirst];

  SwitchStackFirst += 1;
  if (SwitchStackFirst>=SWITCH_STACK_SIZE) SwitchStackFirst = 0;

  return retVal;
}


boolean BSOS_ReadSingleSwitchState(byte switchNum) {
  if (switchNum>39) return false;

  int switchByte = switchNum/8;
  int switchBit = switchNum%8;
  if ( ((SwitchesNow[switchByte])>>switchBit) & 0x01 ) return true;
  else return false;
}


int SpaceLeftOnSolenoidStack() {
  if (SolenoidStackFirst>=SOLENOID_STACK_SIZE || SolenoidStackLast>=SOLENOID_STACK_SIZE) return 0;
  if (SolenoidStackLast>=SolenoidStackFirst) return ((SOLENOID_STACK_SIZE-1) - (SolenoidStackLast-SolenoidStackFirst));
  return (SolenoidStackFirst - SolenoidStackLast) - 1;
}


void BSOS_PushToSolenoidStack(byte solenoidNumber, byte numPushes, boolean disableOverride) {
  if (solenoidNumber>14) return;

  // if the solenoid stack is disabled and this isn't an override push, then return
  if (!disableOverride && !SolenoidStackEnabled) return;

  // If the solenoid stack last index is out of range, then it's an error - return
  if (SpaceLeftOnSolenoidStack()==0) return;

  for (int count=0; count<numPushes; count++) {
    SolenoidStack[SolenoidStackLast] = solenoidNumber;
    
    SolenoidStackLast += 1;
    if (SolenoidStackLast==SOLENOID_STACK_SIZE) {
      // If the end index is off the end, then wrap
      SolenoidStackLast = 0;
    }
    // If the stack is now full, return
    if (SpaceLeftOnSolenoidStack()==0) return;
  }
}


void PushToFrontOfSolenoidStack(byte solenoidNumber, byte numPushes) {
  // If the stack is full, return
  if (SpaceLeftOnSolenoidStack()==0  || !SolenoidStackEnabled) return;

  for (int count=0; count<numPushes; count++) {
    if (SolenoidStackFirst==0) SolenoidStackFirst = SOLENOID_STACK_SIZE-1;
    else SolenoidStackFirst -= 1;
    SolenoidStack[SolenoidStackFirst] = solenoidNumber;
    if (SpaceLeftOnSolenoidStack()==0) return;
  }
}


byte PullFirstFromSolenoidStack() {
  // If first and last are equal, there's nothing on the stack
  if (SolenoidStackFirst==SolenoidStackLast) return SOLENOID_STACK_EMPTY;
  
  byte retVal = SolenoidStack[SolenoidStackFirst];

  SolenoidStackFirst += 1;
  if (SolenoidStackFirst>=SOLENOID_STACK_SIZE) SolenoidStackFirst = 0;

  return retVal;
}


boolean BSOS_PushToTimedSolenoidStack(byte solenoidNumber, byte numPushes, unsigned long whenToFire, boolean disableOverride) {
  for (int count=0; count<TIMED_SOLENOID_STACK_SIZE; count++) {
    if (!TimedSolenoidStack[count].inUse) {
      TimedSolenoidStack[count].inUse = true;
      TimedSolenoidStack[count].pushTime = whenToFire;
      TimedSolenoidStack[count].disableOverride = disableOverride;
      TimedSolenoidStack[count].solenoidNumber = solenoidNumber;
      TimedSolenoidStack[count].numPushes = numPushes;
      return true;
    }
  }
  return false;
}


void BSOS_UpdateTimedSolenoidStack(unsigned long curTime) {
  for (int count=0; count<TIMED_SOLENOID_STACK_SIZE; count++) {
    if (TimedSolenoidStack[count].inUse && TimedSolenoidStack[count].pushTime<curTime) {
      BSOS_PushToSolenoidStack(TimedSolenoidStack[count].solenoidNumber, TimedSolenoidStack[count].numPushes, TimedSolenoidStack[count].disableOverride);
      TimedSolenoidStack[count].inUse = false;
    }
  }
}


volatile int numberOfU10Interrupts = 0;
volatile int numberOfU11Interrupts = 0;
volatile byte InsideZeroCrossingInterrupt = 0;


void InterruptService2() {
  byte u10AControl = BSOS_DataRead(ADDRESS_U10_A_CONTROL);
  if (u10AControl & 0x80) {
    // self test switch
    if (BSOS_DataRead(ADDRESS_U10_A_CONTROL) & 0x80) PushToSwitchStack(SW_SELF_TEST_SWITCH);
    BSOS_DataRead(ADDRESS_U10_A);
  }

  // If we get a weird interupt from U11B, clear it
  byte u11BControl = BSOS_DataRead(ADDRESS_U11_B_CONTROL);
  if (u11BControl & 0x80) {
    BSOS_DataRead(ADDRESS_U11_B);    
  }

  byte u11AControl = BSOS_DataRead(ADDRESS_U11_A_CONTROL);
  byte u10BControl = BSOS_DataRead(ADDRESS_U10_B_CONTROL);

  // If the interrupt bit on the display interrupt is on, do the display refresh
  if (u11AControl & 0x80) {
    // Backup U10A
    byte backupU10A = BSOS_DataRead(ADDRESS_U10_A);
    
    // Disable lamp decoders & strobe latch
    BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) | 0x08);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) & 0xF7);

    // Also park the aux lamp board 
    BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL) | 0x08);
    BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL) & 0xF7);    

    // Blank Displays
    BSOS_DataWrite(ADDRESS_U10_A_CONTROL, BSOS_DataRead(ADDRESS_U10_A_CONTROL) & 0xF7);
    BSOS_DataWrite(ADDRESS_U11_A, (BSOS_DataRead(ADDRESS_U11_A) & 0x03) | 0x01);
    BSOS_DataWrite(ADDRESS_U10_A, 0x0F);

    // Write current display digits to 5 displays
    for (int displayCount=0; displayCount<5; displayCount++) {

      if (CurrentDisplayDigit<BALLY_STERN_OS_NUM_DIGITS) {
        // The BCD for this digit is in b4-b7, and the display latch strobes are in b0-b3 (and U11A:b0)
        byte displayDataByte = ((DisplayDigits[displayCount][CurrentDisplayDigit])<<4) | 0x0F;
        byte displayEnable = ((DisplayDigitEnable[displayCount])>>CurrentDisplayDigit)&0x01;
  
        // if this digit shouldn't be displayed, then set data lines to 0xFX so digit will be blank
        if (!displayEnable) displayDataByte = 0xFF;
  
        // Set low the appropriate latch strobe bit
        if (displayCount<4) {
          displayDataByte &= ~(0x01<<displayCount);
        }
        // Write out the digit & strobe (if it's 0-3)
        BSOS_DataWrite(ADDRESS_U10_A, displayDataByte);
        if (displayCount==4) {            
          // Strobe #5 latch on U11A:b0
          BSOS_DataWrite(ADDRESS_U11_A, BSOS_DataRead(ADDRESS_U11_A) & 0xFE);
        }

        // Need to delay a little to make sure the strobe is low for long enough
        WaitClockCycle(4);

        // Put the latch strobe bits back high
        if (displayCount<4) {
          displayDataByte |= 0x0F;
          BSOS_DataWrite(ADDRESS_U10_A, displayDataByte);
        } else {
          BSOS_DataWrite(ADDRESS_U11_A, BSOS_DataRead(ADDRESS_U11_A) | 0x01);
          
          // Set proper display digit enable
          byte displayDigitsMask = (0x02<<CurrentDisplayDigit) | 0x01;
          BSOS_DataWrite(ADDRESS_U11_A, displayDigitsMask);
        }
      }
    }

    // Stop Blanking (current digits are all latched and ready)
    BSOS_DataWrite(ADDRESS_U10_A_CONTROL, BSOS_DataRead(ADDRESS_U10_A_CONTROL) | 0x08);

    // Restore 10A from backup
    BSOS_DataWrite(ADDRESS_U10_A, backupU10A);    

    CurrentDisplayDigit = CurrentDisplayDigit + 1;
    if (CurrentDisplayDigit>=BALLY_STERN_OS_NUM_DIGITS) {
      CurrentDisplayDigit = 0;
      DisplayOffCycle ^= true;
    }
    numberOfU11Interrupts+=1;
  }

  // If the IRQ bit of U10BControl is set, do the Zero-crossing interrupt handler
  if ((u10BControl & 0x80) && (InsideZeroCrossingInterrupt==0)) {
    InsideZeroCrossingInterrupt = InsideZeroCrossingInterrupt + 1;

    byte u10BControlLatest = BSOS_DataRead(ADDRESS_U10_B_CONTROL);

    // Backup contents of U10A
    byte backup10A = BSOS_DataRead(ADDRESS_U10_A);

    // Latch 0xFF separately without interrupt clear
    BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) | 0x08);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) & 0xF7);
    // Read U10B to clear interrupt
    BSOS_DataRead(ADDRESS_U10_B);

    // Turn off U10BControl interrupts
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x30);

    // Copy old switch values
    byte switchCount;
    byte startingClosures;
    byte validClosures;
    for (switchCount=0; switchCount<5; switchCount++) {
      SwitchesMinus2[switchCount] = SwitchesMinus1[switchCount];
      SwitchesMinus1[switchCount] = SwitchesNow[switchCount];

      // Enable playfield strobe
      BSOS_DataWrite(ADDRESS_U10_A, 0x01<<switchCount);
      BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x34);

      // Delay for switch capacitors to charge
      delayMicroseconds(BSOS_SWITCH_DELAY_IN_MICROSECONDS);
      
      // Read the switches
      SwitchesNow[switchCount] = BSOS_DataRead(ADDRESS_U10_B);

      //Unset the strobe
      BSOS_DataWrite(ADDRESS_U10_A, 0x00);

      // Some switches need to trigger immediate closures (bumpers & slings)
      startingClosures = (SwitchesNow[switchCount]) & (~SwitchesMinus1[switchCount]);
      boolean immediateSolenoidFired = false;
      // If one of the switches is starting to close (off, on)
      if (startingClosures) {
        // Loop on bits of switch byte
        for (byte bitCount=0; bitCount<8 && immediateSolenoidFired==false; bitCount++) {
          // If this switch bit is closed
          if (startingClosures&0x01) {
            byte startingSwitchNum = switchCount*8 + bitCount;
            // Loop on immediate switch data
            for (int immediateSwitchCount=0; immediateSwitchCount<NumGamePrioritySwitches && immediateSolenoidFired==false; immediateSwitchCount++) {
              // If this switch requires immediate action
              if (GameSwitches && startingSwitchNum==GameSwitches[immediateSwitchCount].switchNum) {
                // Start firing this solenoid (just one until the closure is validate
                PushToFrontOfSolenoidStack(GameSwitches[immediateSwitchCount].solenoid, 1);
                immediateSolenoidFired = true;
              }
            }
          }
          startingClosures = startingClosures>>1;
        }
      }

      immediateSolenoidFired = false;
      validClosures = (SwitchesNow[switchCount] & SwitchesMinus1[switchCount]) & ~SwitchesMinus2[switchCount];
      // If there is a valid switch closure (off, on, on)
      if (validClosures) {
        // Loop on bits of switch byte
        for (byte bitCount=0; bitCount<8; bitCount++) {
          // If this switch bit is closed
          if (validClosures&0x01) {
            byte validSwitchNum = switchCount*8 + bitCount;
            // Loop through all switches and see what's triggered
            for (int validSwitchCount=0; validSwitchCount<NumGameSwitches; validSwitchCount++) {

              // If we've found a valid closed switch
              if (GameSwitches && GameSwitches[validSwitchCount].switchNum==validSwitchNum) {

                // If we're supposed to trigger a solenoid, then do it
                if (GameSwitches[validSwitchCount].solenoid!=SOL_NONE) {
                  if (validSwitchCount<NumGamePrioritySwitches && immediateSolenoidFired==false) {
                    PushToFrontOfSolenoidStack(GameSwitches[validSwitchCount].solenoid, GameSwitches[validSwitchCount].solenoidHoldTime);
                  } else {
                    BSOS_PushToSolenoidStack(GameSwitches[validSwitchCount].solenoid, GameSwitches[validSwitchCount].solenoidHoldTime);
                  }
                } // End if this is a real solenoid
              } // End if this is a switch in the switch table
            } // End loop on switches in switch table
            // Push this switch to the game rules stack
            PushToSwitchStack(validSwitchNum);
          }
          validClosures = validClosures>>1;
        }        
      }

      // There are no port reads or writes for the rest of the loop, 
      // so we can allow the display interrupt to fire
      interrupts();
      
      // Wait so total delay will allow lamp SCRs to get to the proper voltage
      delayMicroseconds(BSOS_TIMING_LOOP_PADDING_IN_MICROSECONDS);
      
      noInterrupts();
    }
    BSOS_DataWrite(ADDRESS_U10_A, backup10A);

    // If we need to turn off momentary solenoids, do it first
    byte momentarySolenoidAtStart = PullFirstFromSolenoidStack();
    if (momentarySolenoidAtStart!=SOLENOID_STACK_EMPTY) {
      CurrentSolenoidByte = (CurrentSolenoidByte&0xF0) | momentarySolenoidAtStart;
      BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
    } else {
      CurrentSolenoidByte = (CurrentSolenoidByte&0xF0) | SOL_NONE;
      BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
    }

    for (int lampBitCount = 0; lampBitCount<BSOS_NUM_LAMP_BITS; lampBitCount++) {
      byte lampData = 0xF0 + lampBitCount;

      interrupts();
      BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
      noInterrupts();
      
      // Latch address & strobe
      BSOS_DataWrite(ADDRESS_U10_A, lampData);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      

      BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x38);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      

      BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x30);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      

      // Use the inhibit lines to set the actual data to the lamp SCRs 
      // (here, we don't care about the lower nibble because the address was already latched)
      byte lampOutput = LampStates[lampBitCount];
      // Every other time through the cycle, we OR in the dim variable
      // in order to dim those lights
      if (numberOfU10Interrupts%DimDivisor1) lampOutput |= LampDim0[lampBitCount];
      if (numberOfU10Interrupts%DimDivisor2) lampOutput |= LampDim1[lampBitCount];

      BSOS_DataWrite(ADDRESS_U10_A, lampOutput);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      
    }

    // Latch 0xFF separately without interrupt clear
    BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) | 0x08);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) & 0xF7);

    interrupts();
    noInterrupts();

    InsideZeroCrossingInterrupt = 0;
    BSOS_DataWrite(ADDRESS_U10_A, backup10A);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, u10BControlLatest);

    // Read U10B to clear interrupt
    BSOS_DataRead(ADDRESS_U10_B);
    numberOfU10Interrupts+=1;
  }
}


#if defined (BALLY_STERN_OS_SOFTWARE_DISPLAY_INTERRUPT)
ISR(TIMER1_COMPA_vect) {    //This is the interrupt request
  // Backup U10A
  byte backupU10A = BSOS_DataRead(ADDRESS_U10_A);
  
  // Disable lamp decoders & strobe latch
  BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
  BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) | 0x08);
  BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) & 0xF7);

  // Also park the aux lamp board 
  BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL) | 0x08);
  BSOS_DataWrite(ADDRESS_U11_A_CONTROL, BSOS_DataRead(ADDRESS_U11_A_CONTROL) & 0xF7);    
#endif

  // Blank Displays
  BSOS_DataWrite(ADDRESS_U10_A_CONTROL, BSOS_DataRead(ADDRESS_U10_A_CONTROL) & 0xF7);
  BSOS_DataWrite(ADDRESS_U11_A, (BSOS_DataRead(ADDRESS_U11_A) & 0x03) | 0x01);
  BSOS_DataWrite(ADDRESS_U10_A, 0x0F);

  // Write current display digits to 5 displays
  for (int displayCount=0; displayCount<5; displayCount++) {

    if (CurrentDisplayDigit<BALLY_STERN_OS_NUM_DIGITS) {
      // The BCD for this digit is in b4-b7, and the display latch strobes are in b0-b3 (and U11A:b0)
      byte displayDataByte = ((DisplayDigits[displayCount][CurrentDisplayDigit])<<4) | 0x0F;
      byte displayEnable = ((DisplayDigitEnable[displayCount])>>CurrentDisplayDigit)&0x01;

      // if this digit shouldn't be displayed, then set data lines to 0xFX so digit will be blank
      if (!displayEnable) displayDataByte = 0xFF;

      // Set low the appropriate latch strobe bit
      if (displayCount<4) {
        displayDataByte &= ~(0x01<<displayCount);
      }
      // Write out the digit & strobe (if it's 0-3)
      BSOS_DataWrite(ADDRESS_U10_A, displayDataByte);
      if (displayCount==4) {            
        // Strobe #5 latch on U11A:b0
        BSOS_DataWrite(ADDRESS_U11_A, BSOS_DataRead(ADDRESS_U11_A) & 0xFE);
      }

      // Need to delay a little to make sure the strobe is low for long enough
      WaitClockCycle(4);

      // Put the latch strobe bits back high
      if (displayCount<4) {
        displayDataByte |= 0x0F;
        BSOS_DataWrite(ADDRESS_U10_A, displayDataByte);
      } else {
        BSOS_DataWrite(ADDRESS_U11_A, BSOS_DataRead(ADDRESS_U11_A) | 0x01);
        
        // Set proper display digit enable
        byte displayDigitsMask = (0x02<<CurrentDisplayDigit) | 0x01;
        BSOS_DataWrite(ADDRESS_U11_A, displayDigitsMask);
      }
    }
  }

  // Stop Blanking (current digits are all latched and ready)
  BSOS_DataWrite(ADDRESS_U10_A_CONTROL, BSOS_DataRead(ADDRESS_U10_A_CONTROL) | 0x08);

  // Restore 10A from backup
  BSOS_DataWrite(ADDRESS_U10_A, backupU10A);    

  CurrentDisplayDigit = CurrentDisplayDigit + 1;
  if (CurrentDisplayDigit>=BALLY_STERN_OS_NUM_DIGITS) {
    CurrentDisplayDigit = 0;
    DisplayOffCycle ^= true;
  }
}


void InterruptService3() {
  byte u10AControl = BSOS_DataRead(ADDRESS_U10_A_CONTROL);
  if (u10AControl & 0x80) {
    // self test switch
    if (BSOS_DataRead(ADDRESS_U10_A_CONTROL) & 0x80) PushToSwitchStack(SW_SELF_TEST_SWITCH);
    BSOS_DataRead(ADDRESS_U10_A);
  }

  // If we get a weird interupt from U11B, clear it
  byte u11BControl = BSOS_DataRead(ADDRESS_U11_B_CONTROL);
  if (u11BControl & 0x80) {
    BSOS_DataRead(ADDRESS_U11_B);    
  }

  byte u11AControl = BSOS_DataRead(ADDRESS_U11_A_CONTROL);
  byte u10BControl = BSOS_DataRead(ADDRESS_U10_B_CONTROL);

  // If the interrupt bit on the display interrupt is on, do the display refresh
  if (u11AControl & 0x80) {
    BSOS_DataRead(ADDRESS_U11_A);
    numberOfU11Interrupts+=1;
  }

  // If the IRQ bit of U10BControl is set, do the Zero-crossing interrupt handler
  if ((u10BControl & 0x80) && (InsideZeroCrossingInterrupt==0)) {
    InsideZeroCrossingInterrupt = InsideZeroCrossingInterrupt + 1;

    byte u10BControlLatest = BSOS_DataRead(ADDRESS_U10_B_CONTROL);

    // Backup contents of U10A
    byte backup10A = BSOS_DataRead(ADDRESS_U10_A);

    // Latch 0xFF separately without interrupt clear
    BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) | 0x08);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) & 0xF7);
    // Read U10B to clear interrupt
    BSOS_DataRead(ADDRESS_U10_B);

    // Turn off U10BControl interrupts
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x30);

    // Copy old switch values
    byte switchCount;
    byte startingClosures;
    byte validClosures;
    for (switchCount=0; switchCount<5; switchCount++) {
      SwitchesMinus2[switchCount] = SwitchesMinus1[switchCount];
      SwitchesMinus1[switchCount] = SwitchesNow[switchCount];

      // Enable playfield strobe
      BSOS_DataWrite(ADDRESS_U10_A, 0x01<<switchCount);
      BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x34);

      // Delay for switch capacitors to charge
      delayMicroseconds(BSOS_SWITCH_DELAY_IN_MICROSECONDS);
      
      // Read the switches
      SwitchesNow[switchCount] = BSOS_DataRead(ADDRESS_U10_B);

      //Unset the strobe
      BSOS_DataWrite(ADDRESS_U10_A, 0x00);

      // Some switches need to trigger immediate closures (bumpers & slings)
      startingClosures = (SwitchesNow[switchCount]) & (~SwitchesMinus1[switchCount]);
      boolean immediateSolenoidFired = false;
      // If one of the switches is starting to close (off, on)
      if (startingClosures) {
        // Loop on bits of switch byte
        for (byte bitCount=0; bitCount<8 && immediateSolenoidFired==false; bitCount++) {
          // If this switch bit is closed
          if (startingClosures&0x01) {
            byte startingSwitchNum = switchCount*8 + bitCount;
            // Loop on immediate switch data
            for (int immediateSwitchCount=0; immediateSwitchCount<NumGamePrioritySwitches && immediateSolenoidFired==false; immediateSwitchCount++) {
              // If this switch requires immediate action
              if (GameSwitches && startingSwitchNum==GameSwitches[immediateSwitchCount].switchNum) {
                // Start firing this solenoid (just one until the closure is validate
                PushToFrontOfSolenoidStack(GameSwitches[immediateSwitchCount].solenoid, 1);
                immediateSolenoidFired = true;
              }
            }
          }
          startingClosures = startingClosures>>1;
        }
      }

      immediateSolenoidFired = false;
      validClosures = (SwitchesNow[switchCount] & SwitchesMinus1[switchCount]) & ~SwitchesMinus2[switchCount];
      // If there is a valid switch closure (off, on, on)
      if (validClosures) {
        // Loop on bits of switch byte
        for (byte bitCount=0; bitCount<8; bitCount++) {
          // If this switch bit is closed
          if (validClosures&0x01) {
            byte validSwitchNum = switchCount*8 + bitCount;
            // Loop through all switches and see what's triggered
            for (int validSwitchCount=0; validSwitchCount<NumGameSwitches; validSwitchCount++) {

              // If we've found a valid closed switch
              if (GameSwitches && GameSwitches[validSwitchCount].switchNum==validSwitchNum) {

                // If we're supposed to trigger a solenoid, then do it
                if (GameSwitches[validSwitchCount].solenoid!=SOL_NONE) {
                  if (validSwitchCount<NumGamePrioritySwitches && immediateSolenoidFired==false) {
                    PushToFrontOfSolenoidStack(GameSwitches[validSwitchCount].solenoid, GameSwitches[validSwitchCount].solenoidHoldTime);
                  } else {
                    BSOS_PushToSolenoidStack(GameSwitches[validSwitchCount].solenoid, GameSwitches[validSwitchCount].solenoidHoldTime);
                  }
                } // End if this is a real solenoid
              } // End if this is a switch in the switch table
            } // End loop on switches in switch table
            // Push this switch to the game rules stack
            PushToSwitchStack(validSwitchNum);
          }
          validClosures = validClosures>>1;
        }        
      }

      // There are no port reads or writes for the rest of the loop, 
      // so we can allow the display interrupt to fire
      interrupts();
      
      // Wait so total delay will allow lamp SCRs to get to the proper voltage
      delayMicroseconds(BSOS_TIMING_LOOP_PADDING_IN_MICROSECONDS);
      
      noInterrupts();
    }
    BSOS_DataWrite(ADDRESS_U10_A, backup10A);

    // If we need to turn off momentary solenoids, do it first
    byte momentarySolenoidAtStart = PullFirstFromSolenoidStack();
    if (momentarySolenoidAtStart!=SOLENOID_STACK_EMPTY) {
      CurrentSolenoidByte = (CurrentSolenoidByte&0xF0) | momentarySolenoidAtStart;
      BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
    } else {
      CurrentSolenoidByte = (CurrentSolenoidByte&0xF0) | SOL_NONE;
      BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
    }

    for (int lampBitCount = 0; lampBitCount<BSOS_NUM_LAMP_BITS; lampBitCount++) {
      byte lampData = 0xF0 + lampBitCount;

      interrupts();
      BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
      noInterrupts();
      
      // Latch address & strobe
      BSOS_DataWrite(ADDRESS_U10_A, lampData);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      

      BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x38);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      

      BSOS_DataWrite(ADDRESS_U10_B_CONTROL, 0x30);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      

      // Use the inhibit lines to set the actual data to the lamp SCRs 
      // (here, we don't care about the lower nibble because the address was already latched)
      byte lampOutput = LampStates[lampBitCount];
      // Every other time through the cycle, we OR in the dim variable
      // in order to dim those lights
      if (numberOfU10Interrupts%DimDivisor1) lampOutput |= LampDim0[lampBitCount];
      if (numberOfU10Interrupts%DimDivisor2) lampOutput |= LampDim1[lampBitCount];

      BSOS_DataWrite(ADDRESS_U10_A, lampOutput);
#ifdef BSOS_SLOW_DOWN_LAMP_STROBE      
      WaitClockCycle();
#endif      
    }

    // Latch 0xFF separately without interrupt clear
    BSOS_DataWrite(ADDRESS_U10_A, 0xFF);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) | 0x08);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, BSOS_DataRead(ADDRESS_U10_B_CONTROL) & 0xF7);


    interrupts();
    noInterrupts();

    InsideZeroCrossingInterrupt = 0;
    BSOS_DataWrite(ADDRESS_U10_A, backup10A);
    BSOS_DataWrite(ADDRESS_U10_B_CONTROL, u10BControlLatest);

    // Read U10B to clear interrupt
    BSOS_DataRead(ADDRESS_U10_B);
    numberOfU10Interrupts+=1;
  }
}


byte BSOS_SetDisplay(int displayNumber, unsigned long value, boolean blankByMagnitude, byte minDigits) {
  if (displayNumber<0 || displayNumber>4) return 0;

  byte blank = 0x00;

  for (int count=0; count<BALLY_STERN_OS_NUM_DIGITS; count++) {
    blank = blank * 2;
    if (value!=0 || count<minDigits) blank |= 1;
    DisplayDigits[displayNumber][(BALLY_STERN_OS_NUM_DIGITS-1)-count] = value%10;
    value /= 10;    
  }

  if (blankByMagnitude) DisplayDigitEnable[displayNumber] = blank;

  return blank;
}


void BSOS_SetDisplayBlank(int displayNumber, byte bitMask) {
  if (displayNumber<0 || displayNumber>4) return;
  
  DisplayDigitEnable[displayNumber] = bitMask;
}


byte BSOS_GetDisplayBlank(int displayNumber) {
  if (displayNumber<0 || displayNumber>4) return 0;
  return DisplayDigitEnable[displayNumber];
}


void BSOS_SetDisplayFlash(int displayNumber, unsigned long value, unsigned long curTime, int period, byte minDigits) {
  // A period of zero toggles display every other time
  if (period) {
    if ((curTime/period)%2) {
      BSOS_SetDisplay(displayNumber, value, true, minDigits);
    } else {
      BSOS_SetDisplayBlank(displayNumber, 0);
    }
  }
  
}


void BSOS_SetDisplayFlashCredits(unsigned long curTime, int period) {
  if (period) {
    if ((curTime/period)%2) {
      DisplayDigitEnable[4] |= 0x06;
    } else {
      DisplayDigitEnable[4] &= 0x39;
    }
  }
}


void BSOS_SetDisplayCredits(int value, boolean displayOn, boolean showBothDigits) {
  DisplayDigits[4][2] = (value%100) / 10;
  DisplayDigits[4][3] = (value%10);

  byte enableMask = DisplayDigitEnable[4] & BALLY_STERN_OS_MASK_SHIFT_1;

  if (displayOn) {
    if (value>9 || showBothDigits) enableMask |= BALLY_STERN_OS_MASK_SHIFT_2; 
    else enableMask |= 0x04;
  }

  DisplayDigitEnable[4] = enableMask;
}

void BSOS_SetDisplayMatch(int value, boolean displayOn, boolean showBothDigits) {
  BSOS_SetDisplayBallInPlay(value, displayOn, showBothDigits);
}

void BSOS_SetDisplayBallInPlay(int value, boolean displayOn, boolean showBothDigits) {
  DisplayDigits[4][5] = (value%100) / 10;
  DisplayDigits[4][6] = (value%10); 

  byte enableMask = DisplayDigitEnable[4] & BALLY_STERN_OS_MASK_SHIFT_2;

  if (displayOn) {
    if (value>9 || showBothDigits) enableMask |= BALLY_STERN_OS_MASK_SHIFT_1;
    else enableMask |= 0x20;
  }

  DisplayDigitEnable[4] = enableMask;
}


void BSOS_SetDimDivisor(byte level, byte divisor) {
  if (level==1) DimDivisor1 = divisor;
  if (level==2) DimDivisor2 = divisor;
}


void BSOS_SetLampState(int lampNum, byte s_lampState, byte s_lampDim, int s_lampFlashPeriod) {
  if (lampNum>=BSOS_MAX_LAMPS || lampNum<0) return;
  
  if (s_lampState) {
    int adjustedLampFlash = s_lampFlashPeriod/50;
    
    if (s_lampFlashPeriod!=0 && adjustedLampFlash==0) adjustedLampFlash = 1;
    if (adjustedLampFlash>250) adjustedLampFlash = 250;
    
    // Only turn on the lamp if there's no flash, because if there's a flash
    // then the lamp will be turned on by the ApplyFlashToLamps function
    if (s_lampFlashPeriod==0) LampStates[lampNum/4] &= ~(0x10<<(lampNum%4));
    LampFlashPeriod[lampNum] = adjustedLampFlash;
  } else {
    LampStates[lampNum/4] |= (0x10<<(lampNum%4));
    LampFlashPeriod[lampNum] = 0;
  }

  if (s_lampDim & 0x01) {    
    LampDim0[lampNum/4] |= (0x10<<(lampNum%4));
  } else {
    LampDim0[lampNum/4] &= ~(0x10<<(lampNum%4));
  }

  if (s_lampDim & 0x02) {    
    LampDim1[lampNum/4] |= (0x10<<(lampNum%4));
  } else {
    LampDim1[lampNum/4] &= ~(0x10<<(lampNum%4));
  }

}


void BSOS_ApplyFlashToLamps(unsigned long curTime) {
  for (int count=0; count<BSOS_MAX_LAMPS; count++) {
    if ( LampFlashPeriod[count]!=0 ) {
      unsigned long adjustedLampFlash = (unsigned long)LampFlashPeriod[count] * (unsigned long)50;
      if ((curTime/adjustedLampFlash)%2) {
        LampStates[count/4] &= ~(0x10<<(count%4));
      } else {
        LampStates[count/4] |= (0x10<<(count%4));
      }
    } // end if this light should flash
  } // end loop on lights
}


void BSOS_FlashAllLamps(unsigned long curTime) {
  for (int count=0; count<BSOS_MAX_LAMPS; count++) {
    BSOS_SetLampState(count, 1, 0, 500);  
  }

  BSOS_ApplyFlashToLamps(curTime);
}


void BSOS_TurnOffAllLamps() {
  for (int count=0; count<BSOS_MAX_LAMPS; count++) {
    BSOS_SetLampState(count, 0, 0, 0);  
  }
}


void BSOS_InitializeMPU() {
  // Wait for board to boot
  delayMicroseconds(50000);
  delayMicroseconds(50000);

  for (byte count=2; count<32; count++) pinMode(count, INPUT);

  // Decide if halt should be raised (based on switch) 
  pinMode(13, INPUT);
  if (digitalRead(13)==0) {
    // Switch indicates the Arduino should run, so HALT the 6800
    pinMode(14, OUTPUT); // Halt
    digitalWrite(14, LOW);
  } else {
    // Let the 6800 run 
    pinMode(14, OUTPUT); // Halt
    digitalWrite(14, HIGH);
    while(1);
  }  
  
  pinMode(3, INPUT); // CLK
  pinMode(4, OUTPUT); // VMA
  pinMode(5, OUTPUT); // R/W
  for (byte count=6; count<13; count++) pinMode(count, INPUT); // D0-D6
  pinMode(13, INPUT); // Switch
  pinMode(14, OUTPUT); // Halt
  pinMode(15, INPUT); // D7
  for (byte count=16; count<32; count++) pinMode(count, OUTPUT); // Address lines are output
  digitalWrite(5, HIGH);  // Set R/W line high (Read)
  digitalWrite(4, LOW);  // Set VMA line LOW

  // Interrupt line (IRQ)
  pinMode(2, INPUT);

  // Prep the address bus (all lines zero)
  BSOS_DataRead(0);
  // Set up the PIAs
  InitializeU10PIA();
  InitializeU11PIA();

  // Reset address bus
  BSOS_DataRead(0);

  // Reset solenoid stack
  SolenoidStackFirst = 0;
  SolenoidStackLast = 0;

  // Reset switch stack
  SwitchStackFirst = 0;
  SwitchStackLast = 0;

  CurrentDisplayDigit = 0; 

  // Set default values for the displays
  for (int displayCount=0; displayCount<5; displayCount++) {
    for (int digitCount=0; digitCount<6; digitCount++) {
      DisplayDigits[displayCount][digitCount] = 0;
    }
    DisplayDigitEnable[displayCount] = 0x03;
  }

  // Turn off all lamp states
  for (int lampNibbleCounter=0; lampNibbleCounter<BSOS_NUM_LAMP_BITS; lampNibbleCounter++) {
    LampStates[lampNibbleCounter] = 0xFF;
    LampDim0[lampNibbleCounter] = 0x00;
    LampDim1[lampNibbleCounter] = 0x00;
  }

  for (int lampFlashCount=0; lampFlashCount<BSOS_MAX_LAMPS; lampFlashCount++) {
    LampFlashPeriod[lampFlashCount] = 0;
  }

  // Reset all the switch values 
  // (set them as closed so that if they're stuck they don't register as new events)
  byte switchCount;
  for (switchCount=0; switchCount<5; switchCount++) {
    SwitchesMinus2[switchCount] = 0xFF;
    SwitchesMinus1[switchCount] = 0xFF;
    SwitchesNow[switchCount] = 0xFF;
  }
  
  // Hook up the interrupt
#if not defined (BALLY_STERN_OS_SOFTWARE_DISPLAY_INTERRUPT)
  attachInterrupt(digitalPinToInterrupt(2), InterruptService2, LOW);
#else
/*
  cli();
  TCCR2A|=(1<<WGM21);     //Set the CTC mode
  OCR2A=0xBA;            //Set the value for 3ms
  TIMSK2|=(1<<OCIE2A);   //Set the interrupt request
  TCCR2B|=(1<<CS22);     //Set the prescale 1/64 clock
  sei();                 //Enable interrupt
*/  

  cli();
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for selected increment
  OCR1A = BALLY_STERN_OS_SOFTWARE_DISPLAY_INTERRUPT_INTERVAL;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
  
  attachInterrupt(digitalPinToInterrupt(2), InterruptService3, LOW);
#endif  
  BSOS_DataRead(0);  // Reset address bus

  // Cleary all possible interrupts by reading the registers
  BSOS_DataRead(ADDRESS_U11_A);
  BSOS_DataRead(ADDRESS_U11_B);
  BSOS_DataRead(ADDRESS_U10_A);
  BSOS_DataRead(ADDRESS_U10_B);
  BSOS_DataRead(0);  // Reset address bus

}


byte BSOS_GetDipSwitches(byte index) {
  return 0x00 & index;
}


void BSOS_SetupGameSwitches(int s_numSwitches, int s_numPrioritySwitches, PlayfieldAndCabinetSwitch *s_gameSwitchArray) {
  NumGameSwitches = s_numSwitches;
  NumGamePrioritySwitches = s_numPrioritySwitches;
  GameSwitches = s_gameSwitchArray;
}


void BSOS_SetCoinLockout(boolean lockoutOn, byte solbit) {
  if (lockoutOn) {
    CurrentSolenoidByte = CurrentSolenoidByte & ~solbit;
  } else {
    CurrentSolenoidByte = CurrentSolenoidByte | solbit;
  }
  BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
}


void BSOS_SetDisableFlippers(boolean disableFlippers, byte solbit) {
  if (disableFlippers) {
    CurrentSolenoidByte = CurrentSolenoidByte | solbit;
  } else {
    CurrentSolenoidByte = CurrentSolenoidByte & ~solbit;
  }
  
  BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
}

void BSOS_SetContinuousSolenoidBit(boolean bitOn, byte solbit) {
  if (bitOn) {
    CurrentSolenoidByte = CurrentSolenoidByte | solbit;
  } else {
    CurrentSolenoidByte = CurrentSolenoidByte & ~solbit;
  }
  BSOS_DataWrite(ADDRESS_U11_B, CurrentSolenoidByte);
}


byte BSOS_ReadContinuousSolenoids() {
  return BSOS_DataRead(ADDRESS_U11_B);
}


void BSOS_DisableSolenoidStack() {
  SolenoidStackEnabled = false;
}


void BSOS_EnableSolenoidStack() {
  SolenoidStackEnabled = true;
}


void BSOS_CycleAllDisplays(unsigned long curTime, byte digitNum) {
  int displayDigit = (curTime/250)%10;
  unsigned long value;
  value = displayDigit*1111111;

  byte displayNumToShow = 0;
  byte displayBlank = BALLY_STERN_OS_ALL_DIGITS_MASK;

  if (digitNum!=0) {
    displayNumToShow = (digitNum-1)/6;
    displayBlank = (0x40)>>((digitNum-1)%7);
  }

  for (int count=0; count<5; count++) {
    if (digitNum) {
      BSOS_SetDisplay(count, value);
      if (count==displayNumToShow) BSOS_SetDisplayBlank(count, displayBlank);
      else BSOS_SetDisplayBlank(count, 0);
    } else {
      BSOS_SetDisplay(count, value, true);
    }
  }
}


void BSOS_PlaySoundSquawkAndTalk(byte soundByte) {

  byte oldSolenoidControlByte, soundLowerNibble, soundUpperNibble;

  // mask further zero-crossing interrupts during this 
  noInterrupts();

  // Get the current value of U11:PortB - current solenoids
  oldSolenoidControlByte = BSOS_DataRead(ADDRESS_U11_B);
  soundLowerNibble = (oldSolenoidControlByte&0xF0) | (soundByte&0x0F); 
  soundUpperNibble = (oldSolenoidControlByte&0xF0) | (soundByte/16); 
    
  // Put 1s on momentary solenoid lines
  BSOS_DataWrite(ADDRESS_U11_B, oldSolenoidControlByte | 0x0F);

  // Put sound latch low
  BSOS_DataWrite(ADDRESS_U11_B_CONTROL, 0x34);

  // Let the strobe stay low for a moment
  delayMicroseconds(32);

  // Put sound latch high
  BSOS_DataWrite(ADDRESS_U11_B_CONTROL, 0x3C);
  
  // put the new byte on U11:PortB (the lower nibble is currently loaded)
  BSOS_DataWrite(ADDRESS_U11_B, soundLowerNibble);
        
  // wait 138 microseconds
  delayMicroseconds(138);

  // put the new byte on U11:PortB (the uppper nibble is currently loaded)
  BSOS_DataWrite(ADDRESS_U11_B, soundUpperNibble);

  // wait 76 microseconds
  delayMicroseconds(145);

  // Restore the original solenoid byte
  BSOS_DataWrite(ADDRESS_U11_B, oldSolenoidControlByte);

  // Put sound latch low
  BSOS_DataWrite(ADDRESS_U11_B_CONTROL, 0x34);

  interrupts();
}


// EEProm Helper functions
void BSOS_WriteByteToEEProm(unsigned short startByte, byte value) {
  EEPROM.write(startByte, value);
}


byte BSOS_ReadByteFromEEProm(unsigned short startByte) {
  byte value = EEPROM.read(startByte);

  // If this value is unset, set it
  if (value==0xFF) {
    value = 0;
    BSOS_WriteByteToEEProm(startByte, value);
  }
  return value;
}


unsigned long BSOS_ReadULFromEEProm(unsigned short startByte, unsigned long defaultValue) {
  unsigned long value;

  value = (((unsigned long)EEPROM.read(startByte+3))<<24) | 
          ((unsigned long)(EEPROM.read(startByte+2))<<16) | 
          ((unsigned long)(EEPROM.read(startByte+1))<<8) | 
          ((unsigned long)(EEPROM.read(startByte)));

  if (value==0xFFFFFFFF) {
    value = defaultValue; 
    BSOS_WriteULToEEProm(startByte, value);
  }
  return value;
}


void BSOS_WriteULToEEProm(unsigned short startByte, unsigned long value) {
  EEPROM.write(startByte+3, (byte)(value>>24));
  EEPROM.write(startByte+2, (byte)((value>>16) & 0x000000FF));
  EEPROM.write(startByte+1, (byte)((value>>8) & 0x000000FF));
  EEPROM.write(startByte, (byte)(value & 0x000000FF));
}
