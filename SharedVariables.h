#include <Arduino.h>
#include "AttractState.h"
#include "BSOS_Config.h"
#include "BallySternOS.h"
#include "Bonus.h"
#include "CountdownBonusState.h"
#include "Display.h"
#include "GameMode.h"
#include "Lamps.h"
#include "MachineState.h"
#include "PinballMachineBase.h"
#include "SelfTestAndAudit.h"
#include "Sound.h"
#include "StoredParameters.h"

#ifndef SHARED_VARIABLES_H

#define DEBUG_MESSAGES  1


/*********************************************************************
    Machine Options Variables
*********************************************************************/
extern boolean FreePlayMode;
extern byte BallSaveNumSeconds;
extern byte BallsPerGame;
extern byte Credits;
extern byte MaximumCredits;
extern unsigned long HighScore;


/*********************************************************************
    Machine State Variables
*********************************************************************/
extern boolean MachineStateChanged;
extern boolean SamePlayerShootsAgain;
extern byte CurrentBallInPlay;
extern byte CurrentNumPlayers;
extern byte CurrentPlayer;
extern char MachineState;
extern unsigned long CurrentScores[4];
extern unsigned long CurrentTime;
extern unsigned long LastTiltWarningTime;
extern unsigned long ScoreMultiplier;


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
extern byte BonusX[4];
extern byte Bonus[4];
extern byte CurrentBonus;
extern unsigned long BonusXAnimationStart;


/*********************************************************************
    Display Variables
*********************************************************************/
extern unsigned long LastFlashOrDash;
extern unsigned long LastRemainingAnimatedScoreShown;
extern unsigned long ScoreAdditionAnimation;
extern unsigned long ScoreAdditionAnimationStartTime;


/*********************************************************************
    Game Mode Variables
*********************************************************************/
extern GameMode GlobalGameMode;

#define SHARED_VARIABLES_H
#endif
