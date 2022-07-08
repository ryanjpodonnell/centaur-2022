#ifndef GAME_MODE_H
#define GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define GAME_MODE_ORBS_1            2
#define GAME_MODE_ORBS_2            3
#define GAME_MODE_ORBS_3            4
#define GAME_MODE_ORBS_4            5
#define GAME_MODE_DESTROY_CENTAUR   6
#define GAME_MODE_INITIALIZE        255

class GameMode {
  private:
    boolean       bonusIncreased_;
    boolean       gameModeChanged_;
    boolean       pushingBallFromOutlane_;
    boolean       scoreIncreased_;
    byte          gameModeId_;
    unsigned long ballSaveEndTime_;
    unsigned long ballTimeInTrough_;
    unsigned long currentFlashCycle_;

    boolean ballSaveActive();
    boolean ballSaveLampActive();
    int     manageBallInTrough();
    int     manageTilt();
    int     runGameLoop();
    int     runGameModes();
    void    manageBallSave();
    void    manageNewMode();
    void    managePlayerScore();
    void    runGameMode(byte switchHit);

  public:
    GameMode();

    boolean bonusIncreased();
    boolean scoreIncreased();
    byte    gameMode();
    int     run(boolean curStateChanged);
    void    setBallSaveEndTime(unsigned long value);
    void    setBonusIncreased(boolean value);
    void    setGameMode(byte id);
    void    setScoreIncreased(boolean value);
};

#endif
