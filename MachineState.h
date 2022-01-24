#ifndef MACHINE_STATE_H

#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110

boolean AddPlayer(boolean resetNumPlayers = false);
void AddCoinToAudit(byte switchHit);
void AddCredit(boolean playSound = false, byte numToAdd = 1);
void AwardExtraBall();

#define MACHINE_STATE_H
#endif
