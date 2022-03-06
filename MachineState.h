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

#define BALLS_PER_GAME                  3
#define BALL_SAVE_NUMBER_OF_SECONDS     5
#define DEFAULT_HIGH_SCORE              10000
#define MAXIMUM_NUMBER_OF_CREDITS       99
#define MAXIMUM_NUMBER_OF_TILT_WARNINGS 2
#define TILT_WARNING_DEBOUNCE_TIME      1000

class MachineState {
  private:
    PlayerState*  currentPlayer_;
    PlayerState   player1_;
    PlayerState   player2_;
    PlayerState   player3_;
    PlayerState   player4_;
    boolean       ballSaveUsed_;
    boolean       extraBallCollected_;
    boolean       freePlayMode_;
    boolean       machineStateChanged_;
    boolean       samePlayerShootsAgain_;
    byte          ballsPerGame_;
    byte          credits_;
    byte          currentBallInPlay_;
    byte          currentPlayerNumber_;
    byte          numberOfPlayers_;
    byte          numberOfTiltWarnings_;
    byte          scoreMultiplier_;
    int           machineStateId_;
    unsigned long currentBallSwitchHitCounter_;
    unsigned long currentTime_;
    unsigned long highScore_;
    unsigned long lastScoreChangeTime_;
    unsigned long lastTiltWarningTime_;

  public:
    MachineState();
    boolean       allModesQualified();
    boolean       anyModeQualified();
    boolean       ballSaveUsed();
    boolean       currentPlayerTilted();
    boolean       incrementNumberOfPlayers();
    boolean       machineStateChanged();
    boolean       orbsDropTargetsCompleted();
    boolean       resetPlayers();
    boolean       samePlayerShootsAgain();
    byte          bonus(byte player = 0xFF);
    byte          bonusMultiplier();
    byte          credits();
    byte          currentBallInPlay();
    byte          currentPlayerNumber();
    byte          incrementCurrentPlayer();
    byte          numberOfPlayers();
    byte          startQualifiedMode();
    int           initGamePlay();
    int           initNewBall(bool curStateChanged);
    int           machineState();
    unsigned long currentBallSwitchHitCounter();
    unsigned long currentTime();
    unsigned long highScore();
    unsigned long lastScoreChangeTime();
    unsigned long lastTiltWarningTime();
    unsigned long score(byte player = 0xFF);
    void          awardExtraBall();
    void          decreaseBonus(byte amountToSubtract = 1);
    void          hideAllPlayerLamps();
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseCredits(boolean playSound = false, byte numToAdd = 1);
    void          increaseScore(unsigned long amountToAdd);
    void          incrementCurrentBallSwitchHitCounter();
    void          qualifyMode();
    void          readStoredParameters();
    void          registerDropTarget(byte switchHit);
    void          registerGuardianRollover(byte switchHit);
    void          registerTiltWarning();
    void          registerTopRollover(byte switchHit);
    void          resetDropTargets();
    void          rotatePlayerLamps();
    void          rotateQualifiedMode();
    void          setBallSaveUsed(byte value);
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setCredits(byte value);
    void          setCurrentPlayer(byte value);
    void          setCurrentTime(unsigned long value);
    void          setHighScore(unsigned long value);
    void          setMachineState(int id);
    void          setNumberOfPlayers(byte value);
    void          setScore(unsigned long value, byte player = 0xFF);
    void          updateCaptiveOrbsLamps();
    void          updateGuardianRolloverLamps();
    void          updateOrbsDropTargetLamps();
    void          updateSelectedMode();
    void          updateTopRolloverLamps();
    void          writeCoinToAudit(byte switchHit);
};

#endif
