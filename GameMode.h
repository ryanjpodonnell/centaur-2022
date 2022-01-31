#ifndef GAME_MODE_H
#define GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define GAME_MODE_INITIALIZE        255

class GameMode {
  private:
    boolean       gameModeChanged_;
    byte          gameModeId_;
    unsigned long firstSwitchHitTime_;
    unsigned long ballTimeInTrough_;

    boolean ballSaveActive();
    int     manageBallInTrough(int curState);
    int     manageGameBase(int curState);
    int     manageGameMode(int curState);
    int     manageTilt(int curState);

  public:
    GameMode(byte id);

    int  run(int curState, boolean curStateChanged);
    void setGameMode(byte id);
};

#endif
