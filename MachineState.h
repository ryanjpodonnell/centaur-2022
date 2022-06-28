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
#define MAXIMUM_NUMBER_OF_BALLS_IN_PLAY 5
#define MAXIMUM_NUMBER_OF_CREDITS       99
#define MAXIMUM_NUMBER_OF_TILT_WARNINGS 2
#define TILT_WARNING_DEBOUNCE_TIME      1000

#define FREE_PLAY                       1

class MachineState {
  private:
    PlayerState*  currentPlayer_;
    PlayerState   player1_;
    PlayerState   player2_;
    PlayerState   player3_;
    PlayerState   player4_;
    boolean       ballSaveActivated_;
    boolean       ballSaveUsed_;
    boolean       extraBallCollected_;
    boolean       hurryUpActivated_;
    boolean       machineStateChanged_;
    boolean       playfieldValidated_;
    boolean       samePlayerShootsAgain_;
    boolean       troughSwitchActivated_;
    byte          credits_;
    byte          currentBallInPlay_;
    byte          currentPlayerNumber_;
    byte          numberOfBallsInPlay_;
    byte          numberOfPlayers_;
    byte          numberOfTiltWarnings_;
    int           machineStateId_;
    unsigned long ballEnteredTroughTime_;
    unsigned long currentBallFirstSwitchHitTime_;
    unsigned long currentTime_;
    unsigned long highScore_;
    unsigned long hurryUpValue_;
    unsigned long lastTiltWarningTime_;
    unsigned long mostRecentSwitchHitTime_;

    void          resetMachineState();

  public:
    MachineState();
    boolean       allModesQualified();
    boolean       allowRightDropTargetProgress();
    boolean       anyModeQualified();
    boolean       ballSaveActivated();
    boolean       ballSaveUsed();
    boolean       currentPlayerTilted();
    boolean       firstBallActive();
    boolean       guardianRolloversCompleted();
    boolean       hurryUpActivated();
    boolean       increaseMultiplierQualified();
    boolean       increaseNumberOfPlayers();
    boolean       machineStateChanged();
    boolean       orbsDropTargetsAllStanding();
    boolean       orbsDropTargetsCompleted();
    boolean       orbsDropTargetsCompletedInOrder();
    boolean       playfieldValidated();
    boolean       resetPlayers();
    boolean       rightDropTargetsAllStanding();
    boolean       rightDropTargetsCompleted();
    boolean       rightDropTargetsCompletedInOrder();
    boolean       samePlayerShootsAgain();
    boolean       scoreIncreasing();
    boolean       topRolloversCompleted();
    boolean       troughSwitchActivated();
    byte          bonus(byte player = 0xFF);
    byte          bonusMultiplier();
    byte          credits();
    byte          currentBallInPlay();
    byte          currentPlayerNumber();
    byte          increaseCurrentPlayer();
    byte          numberOfBallsInPlay();
    byte          numberOfPlayers();
    byte          qualifiedScoreMultiplier();
    byte          queensChamberBonusValue();
    byte          scoreMultiplier();
    byte          startQualifiedMode();
    int           initGamePlay();
    int           initNewBall(bool curStateChanged);
    int           machineState();
    unsigned long ballEnteredTroughTime();
    unsigned long currentBallFirstSwitchHitTime();
    unsigned long currentTime();
    unsigned long highScore();
    unsigned long hurryUpValue();
    unsigned long lastTiltWarningTime();
    unsigned long mostRecentSwitchHitTime();
    unsigned long queensChamberScoreValue();
    unsigned long rightDropTargetsFinishedTime();
    unsigned long rightDropTargetsScoreValue();
    unsigned long score(byte player = 0xFF);
    void          awardExtraBall();
    void          completeSelectedMode();
    void          decreaseBonus();
    void          decreaseNumberOfBallsInPlay();
    void          decreaseScoreMultiplier();
    void          dropRightDropTargets();
    void          flashOrbsDropTargetLamps();
    void          flashRightDropTargetLamps();
    void          hideAllPlayerLamps();
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseBonusMultiplier();
    void          increaseCredits(boolean playSound = false, byte numToAdd = 1);
    void          increaseNumberOfBallsInPlay();
    void          increaseQualifiedScoreMultiplier();
    void          increaseQueensChamberBonusValue();
    void          increaseQueensChamberScoreValue();
    void          increaseScore(unsigned long amountToAdd);
    void          increaseScoreMultiplier();
    void          launchBallIntoPlay(int lag = 0);
    void          overridePlayerScore(unsigned long value);
    void          qualifyIncreaseMultiplier();
    void          qualifyMode();
    void          readStoredParameters();
    void          registerGuardianRollover(byte switchHit);
    void          registerOrbsDropTarget(byte switchHit);
    void          registerRightDropTarget(byte switchHit);
    void          registerTiltWarning();
    void          registerTopRollover(byte switchHit);
    void          resetGuardianRollovers();
    void          resetOrbsDropTargets(boolean activateSolenoid = false);
    void          resetQueensChamberBonusValue();
    void          resetQueensChamberScoreValue();
    void          resetRightDropTargets(boolean activateSolenoid = false);
    void          resetTiltWarnings();
    void          resetTopRollovers();
    void          rotatePlayerLamps();
    void          rotateQualifiedMode();
    void          setBallEnteredTroughTime();
    void          setBallSaveActivated();
    void          setBallSaveUsed(byte value);
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setCredits(byte value);
    void          setCurrentBallFirstSwitchHitTime();
    void          setCurrentPlayer(byte value);
    void          setCurrentTime(unsigned long value);
    void          setHighScore(unsigned long value);
    void          setHurryUpActivated(boolean value);
    void          setHurryUpValue(unsigned long value);
    void          setMachineState(int id);
    void          setMostRecentSwitchHitTime();
    void          setNumberOfPlayers(byte value);
    void          setPlayfieldValidated();
    void          setRightDropTargetsFinishedTime();
    void          setScore(unsigned long value, byte player = 0xFF);
    void          setTroughSwitchActivated(boolean value);
    void          showAllPlayerLamps();
    void          unqualifyIncreaseMultiplier();
    void          unqualifyMode();
    void          updateBonusLamps();
    void          updateCaptiveOrbsLamps();
    void          updateGuardianRolloverLamps();
    void          updateOrbsDropTargetLamps();
    void          updatePlayerScore(boolean flashCurrent = false, boolean dashCurrent = false);
    void          updateQueensChamberLamps();
    void          updateRightDropTargetLamps();
    void          updateRightDropTargetResetLamp();
    void          updateRightDropTargetSpotLamp();
    void          updateRightOrbsReleaseLamp();
    void          updateScoreMultiplierLamps();
    void          updateSelectedMode();
    void          updateTopRolloverLamps();
    void          writeCoinToAudit(byte switchHit);
};

#endif
