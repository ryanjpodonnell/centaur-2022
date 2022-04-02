#ifndef GAME_MODE_H
#define GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define GAME_MODE_ORBS_1            2
#define GAME_MODE_ORBS_2            3
#define GAME_MODE_ORBS_3            4
#define GAME_MODE_ORBS_4            5
#define GAME_MODE_INITIALIZE        255

class GameMode {
  private:
    boolean       gameModeChanged_;
    boolean       pushingBallFromOutlane_;
    boolean       scoreIncreased_;
    byte          gameModeId_;
    unsigned long ballTimeInTrough_;

    boolean ballSaveActive();
    boolean ballSaveLampActive();
    int     manageBallInTrough();
    int     manageGameBase(byte switchHit);
    int     manageTilt();
    int     runGameLoop();
    int     runGameModes();
    void    handleHurryUp();
    void    handleNewMode();
    void    handlePlayerBonusLamps();
    void    handlePlayerScore();
    void    handleShootAgainLamp();
    void    runGameMode(byte switchHit);

  public:
    GameMode();

    boolean       scoreIncreased();
    int           run(boolean curStateChanged);
    void          setGameMode(byte id);
    void          setScoreIncreased(boolean value);
};

#endif
