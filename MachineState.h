#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110

#define MAX_DISPLAY_BONUS 175
#define BALLS_PER_GAME 3

class MachineState {
  private:
    boolean       machineStateChanged;
    boolean       samePlayerShootsAgain;
    byte          ballsPerGame;
    byte          bonusMultipliers[4];
    byte          bonuses[4];
    byte          currentBallInPlay;
    byte          currentNumPlayers;
    byte          currentPlayer;
    byte          machineStateId;
    byte          scoreMultiplier;
    unsigned long currentTime;
    unsigned long lastTiltWarningTime;
    unsigned long scores[4];

  public:
    MachineState(byte id);
    boolean       GetMachineStateChanged();
    boolean       GetSamePlayerShootsAgain();
    boolean       IncrementNumberOfPlayers();
    boolean       ResetNumberOfPlayers();
    byte          GetBonus();
    byte          GetBonusMultiplier();
    byte          GetCurrentBallInPlay();
    byte          GetCurrentPlayer();
    byte          GetMachineState();
    byte          GetNumberOfPlayers();
    byte          IncrementCurrentPlayer();
    byte          ResetGame();
    int           InitGamePlay();
    int           InitNewBall(bool curStateChanged, byte playerNum, int ballNum);
    unsigned long GetCurrentTime();
    unsigned long GetLastTiltWarningTime();
    unsigned long GetScore(byte player = 0xFF);
    void          AwardExtraBall();
    void          DecreaseBonus(byte amountToSubtract = 1);
    void          IncreaseBonus(byte amountToAdd = 1);
    void          IncreaseBonusMultiplier();
    void          IncreaseCredits(boolean playSound = false, byte numToAdd = 1);
    void          IncreaseScore(unsigned long amountToAdd);
    void          SetBonus(byte value);
    void          SetCurrentTime(unsigned long value);
    void          SetLastTiltWarningTime(unsigned long value);
    void          SetMachineState(byte id);
    void          SetNumberOfPlayers(byte value);
    void          SetScore(unsigned long value, byte player = 0xFF);
    void          WriteCoinToAudit(byte switchHit);
};

#endif
