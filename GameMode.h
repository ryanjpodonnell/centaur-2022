#ifndef GAME_MODE_H
#define GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define GAME_MODE_INITIALIZE        255

class GameMode {
  private:
    boolean       gameModeChanged_;
    boolean       savingBall_;
    boolean       scoreIncreased_;
    byte          gameModeId_;
    unsigned long firstSwitchHitTime_;
    unsigned long ballTimeInTrough_;

    boolean ballSaveActive();
    int     manageBallInTrough();
    int     manageGameBase(byte switchHit);
    int     manageGameModes();
    int     manageTilt();
    void    handleNewMode();
    void    handlePlayerScore();
    void    manageGameMode(byte switchHit);

  public:
    GameMode(byte id);

    boolean       scoreIncreased();
    int           run(boolean curStateChanged);
    unsigned long firstSwitchHitTime();
    void          setFirstSwitchHitTime(unsigned long value);
    void          setGameMode(byte id);
    void          setScoreIncreased(boolean value);
};

#endif
