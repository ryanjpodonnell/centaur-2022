#ifndef SHARED_VARIABLES_H

#define DEBUG_MESSAGES  1

extern boolean ExtraBallCollected;
extern boolean FreePlayMode;
extern boolean SamePlayerShootsAgain;
extern byte BonusX[4];
extern byte Bonus[4];
extern byte Credits;
extern byte CurrentBonus;
extern byte CurrentNumPlayers;
extern byte CurrentPlayer;
extern byte MaxTiltWarnings;
extern byte MaximumCredits;
extern byte NumTiltWarnings;
extern unsigned long CurrentScores[4];
extern unsigned long CurrentTime;
extern unsigned long HighScore;


/*********************************************************************

    Attract State Variables

*********************************************************************/
extern unsigned long LastFlash;


/*********************************************************************

    Attract State Variables

*********************************************************************/
extern unsigned long CountdownStartTime;
extern unsigned long LastCountdownReportTime;
extern unsigned long BonusCountDownEndTime;

#define SHARED_VARIABLES_H
#endif
