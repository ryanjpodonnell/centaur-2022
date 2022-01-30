#ifndef GAME_MODE_H
#define GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define GAME_MODE_INITIALIZE        255

class GameMode {
  private:
    byte gameModeId_;
    unsigned long ballFirstSwitchHitTime_;
    unsigned long ballTimeInTrough_;
    unsigned long gameModeEndTime_;
    unsigned long gameModeStartTime_;

  public:
    GameMode(byte id);

    int  manageGameMode();
    int  runGamePlayState(int curState, boolean curStateChanged);
    void setGameMode(byte id);
};

#endif
