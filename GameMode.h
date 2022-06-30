#ifndef GAME_MODE_H
#define GAME_MODE_H

#define GAME_MODE_SKILL_SHOT        0
#define GAME_MODE_UNSTRUCTURED_PLAY 1
#define GAME_MODE_ORBS_1            2
#define GAME_MODE_ORBS_2            3
#define GAME_MODE_ORBS_3            4
#define GAME_MODE_ORBS_4            5
#define GAME_MODE_RESTART_GAME      254
#define GAME_MODE_INITIALIZE        255

#define HURRY_UP_GRACE_PERIOD       2000

class GameMode {
  private:
    boolean       bonusIncreased_;
    boolean       gameModeChanged_;
    boolean       indicatorPlayed_;
    boolean       pushingBallFromOutlane_;
    boolean       scoreIncreased_;
    byte          gameModeId_;
    unsigned long ballTimeInTrough_;
    unsigned long currentFlashCycle_;
    unsigned long hurryUpEndTime_;
    unsigned long hurryUpInitialValue_;
    unsigned long hurryUpStartedTime_;
    unsigned long hurryUpValuePerMillisecond_;
    unsigned long lastFlash_;

    boolean ballSaveActive();
    boolean ballSaveLampActive();
    int     manageBallInTrough();
    int     manageGameRestart();
    int     manageTilt();
    int     runGameLoop();
    int     runGameModes();
    void    manageHurryUp();
    void    manageNewMode();
    void    managePlayerBonusLamps();
    void    managePlayerScore();
    void    manageShootAgainLamp();
    void    manageShotIndicatorShow();
    void    runGameMode(byte switchHit);
    void    updateHurryUpValue();

  public:
    GameMode();

    boolean bonusIncreased();
    boolean scoreIncreased();
    int     run(boolean curStateChanged);
    void    endHurryUp();
    void    resetIndicatorPlayed();
    void    setBonusIncreased(boolean value);
    void    setGameMode(byte id);
    void    setScoreIncreased(boolean value);
    void    startHurryUp(unsigned long value, int seconds);
};

#endif
