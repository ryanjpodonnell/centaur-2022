#ifndef GAME_MODE_H

#define GAME_MODE_INITIALIZE       -1
#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1

#define TILT_WARNING_DEBOUNCE_TIME  1000

class GameMode {
  private:
    byte gameModeId;
    unsigned long ballFirstSwitchHitTime;
    unsigned long ballTimeInTrough;
    unsigned long gameModeEndTime;
    unsigned long gameModeStartTime;

  public:
    GameMode(byte id);

    void SetGameMode(byte id);

    int ManageGameMode();
    int RunGamePlayMode(int curState, boolean curStateChanged);
};

#define GAME_MODE_H
#endif
