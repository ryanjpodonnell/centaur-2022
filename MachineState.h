#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#define MACHINE_STATE_DEBUG           0
#define MACHINE_STATE_ATTRACT         1
#define MACHINE_STATE_INIT_GAMEPLAY   2
#define MACHINE_STATE_INIT_NEW_BALL   3
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110

#define MAX_DISPLAY_BONUS 175
#define BALLS_PER_GAME 3

class MachineState {
  private:
    boolean       freePlayMode_;
    boolean       machineStateChanged_;
    boolean       samePlayerShootsAgain_;
    byte          ballSaveNumSeconds_;
    byte          ballsPerGame_;
    byte          bonusMultipliers_[4];
    byte          bonuses_[4];
    byte          currentBallInPlay_;
    byte          numberOfPlayers_;
    byte          currentPlayer_;
    byte          scoreMultiplier_;
    int           machineStateId_;
    unsigned long currentTime_;
    unsigned long lastTiltWarningTime_;
    unsigned long scores_[4];

  public:
    MachineState(byte id);
    boolean       incrementNumberOfPlayers();
    boolean       machineStateChanged();
    boolean       resetPlayers();
    boolean       samePlayerShootsAgain();
    byte          ballSaveNumSeconds();
    byte          bonus();
    byte          bonusMultiplier();
    byte          currentBallInPlay();
    byte          currentPlayer();
    byte          incrementCurrentPlayer();
    byte          numberOfPlayers();
    byte          resetGame();
    int           initGamePlay();
    int           initNewBall(bool curStateChanged, byte playerNum, int ballNum);
    int           machineState();
    unsigned long currentTime();
    unsigned long lastTiltWarningTime();
    unsigned long score(byte player = 0xFF);
    void          setMachineState(int id);
    void          awardExtraBall();
    void          decreaseBonus(byte amountToSubtract = 1);
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseBonusMultiplier();
    void          increaseCredits(boolean playSound = false, byte numToAdd = 1);
    void          increaseScore(unsigned long amountToAdd);
    void          setBonus(byte value);
    void          setCurrentTime(unsigned long value);
    void          setLastTiltWarningTime(unsigned long value);
    void          setNumberOfPlayers(byte value);
    void          setScore(unsigned long value, byte player = 0xFF);
    void          writeCoinToAudit(byte switchHit);
};

#endif
