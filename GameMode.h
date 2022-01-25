#ifndef GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define TILT_WARNING_DEBOUNCE_TIME 1000

int InitGamePlay();
int InitNewBall(bool curStateChanged, byte playerNum, int ballNum);
int ManageGameMode();
int RunGamePlayMode(int curState, boolean curStateChanged);
void SetGameMode(byte newGameMode);

#define GAME_MODE_H
#endif
