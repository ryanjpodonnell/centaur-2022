#include <Arduino.h>
#include "AttractState.h"
#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "CountdownBonusState.h"
#include "Debug.h"
#include "Display.h"
#include "GameMode.h"
#include "Lamps.h"
#include "machineState.h"
#include "PinballMachineBase.h"
#include "SelfTestAndAudit.h"
#include "Sound.h"

#ifndef SHARED_VARIABLES_H
#define SHARED_VARIABLES_H
#define DEBUG_MESSAGES  1


/*********************************************************************
    Bonus Variables
*********************************************************************/
extern unsigned long BonusXAnimationStart;


/*********************************************************************
    Display Variables
*********************************************************************/
extern unsigned long LastRemainingAnimatedScoreShown;
extern unsigned long ScoreAdditionAnimation;
extern unsigned long ScoreAdditionAnimationStartTime;


extern Attract          g_attract;
extern CountdownBonus   g_countdownBonus;
extern Debug            g_debug;
extern DisplayHelper    g_displayHelper;
extern GameMode         g_gameMode;
extern LampsHelper      g_lampsHelper;
extern MachineState     g_machineState;
extern SelfTestAndAudit g_selfTestAndAudit;

#endif
