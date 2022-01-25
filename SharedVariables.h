#ifndef SHARED_VARIABLES_H

#define DEBUG_MESSAGES  1

extern boolean SamePlayerShootsAgain;
extern byte CurrentNumPlayers;
extern byte CurrentPlayer;
extern byte MaxTiltWarnings;
extern byte NumTiltWarnings;
extern unsigned long CurrentScores[4];
extern unsigned long HighScore;


/*********************************************************************
    Machine State Variables
*********************************************************************/
extern unsigned long CurrentTime;


/*********************************************************************
    Machine Options Variables
*********************************************************************/
extern boolean FreePlayMode;
extern byte Credits;
extern byte MaximumCredits;


/*********************************************************************
    Ball State Variables
*********************************************************************/
extern boolean ExtraBallCollected;


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

#define SHARED_VARIABLES_H
#endif
