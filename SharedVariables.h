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
#include "StoredParameters.h"

#ifndef SHARED_VARIABLES_H
#define SHARED_VARIABLES_H
#define DEBUG_MESSAGES  1


/*********************************************************************
    Machine Options Variables
*********************************************************************/
extern byte Credits;
extern byte MaximumCredits;
extern unsigned long HighScore;


/*********************************************************************
    Ball State Variables
*********************************************************************/
extern boolean BallSaveUsed;
extern boolean ExtraBallCollected;
extern byte MaxTiltWarnings;
extern byte NumTiltWarnings;


/*********************************************************************
    Attract State Variables
*********************************************************************/
extern unsigned long CountdownStartTime;
extern unsigned long LastCountdownReportTime;
extern unsigned long BonusCountDownEndTime;
extern unsigned long LastFlash;


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

extern DisplayHelper g_displayHelper;
extern GameMode g_gameMode;
extern MachineState g_machineState;

#endif
