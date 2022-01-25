#ifndef SHARED_VARIABLES_H

#define DEBUG_MESSAGES  1


/*********************************************************************
    Game State Variables
*********************************************************************/
extern boolean SamePlayerShootsAgain;
extern byte CurrentNumPlayers;
extern byte CurrentPlayer;
extern byte MaxTiltWarnings;
extern byte NumTiltWarnings;
extern unsigned long CurrentScores[4];
extern unsigned long CurrentTime;
extern unsigned long HighScore;


/*********************************************************************
    Machine Options Variables
*********************************************************************/
extern boolean FreePlayMode;
extern byte BallSaveNumSeconds;
extern byte Credits;
extern byte MaximumCredits;


/*********************************************************************
    Ball State Variables
*********************************************************************/
extern boolean BallSaveUsed;
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
extern byte GameMode;
extern unsigned long BallFirstSwitchHitTime;
extern unsigned long BallTimeInTrough;
extern unsigned long GameModeStartTime;
extern unsigned long GameModeEndTime;

#define SHARED_VARIABLES_H
#endif
