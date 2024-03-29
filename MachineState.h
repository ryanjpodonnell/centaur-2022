#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#define MACHINE_STATE_DEBUG           0
#define MACHINE_STATE_ATTRACT         1
#define MACHINE_STATE_INIT_GAMEPLAY   2
#define MACHINE_STATE_INIT_NEW_BALL   3
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_RESTART_GAME    101
#define MACHINE_STATE_MATCH_MODE      110

#define BALLS_PER_GAME                     3
#define BALL_SAVE_NUMBER_OF_SECONDS        5
#define COINS_PER_CREDIT                   2
#define DEFAULT_HIGH_SCORE                 10000
#define MAXIMUM_NUMBER_OF_BALLS_IN_PLAY    5
#define MAXIMUM_NUMBER_OF_COINS_PER_CREDIT 4
#define MAXIMUM_NUMBER_OF_CREDITS          99
#define MAXIMUM_NUMBER_OF_TILT_WARNINGS    2
#define MULTIBALL_SAVE_NUMBER_OF_SECONDS   15
#define SOLENOID_BUFFER_TIME               250
#define TILT_WARNING_DEBOUNCE_TIME         1000

class MachineState {
  private:
    PlayerState*  currentPlayer_;
    PlayerState   player1_;
    PlayerState   player2_;
    PlayerState   player3_;
    PlayerState   player4_;
    boolean       extraBallCollected_;
    boolean       freePlay_;
    boolean       machineStateChanged_;
    boolean       playfieldValidated_;
    boolean       resettingPlayers_;
    boolean       samePlayerShootsAgain_;
    byte          coins_;
    byte          credits_;
    byte          currentBallInPlay_;
    byte          currentPlayerNumber_;
    byte          mostRecentSwitchHit_;
    byte          numberOfBallsInPlay_;
    byte          numberOfCoinsPerCredit_;
    byte          numberOfPlayers_;
    byte          numberOfTiltWarnings_;
    int           machineStateId_;
    unsigned long activationTime_;
    unsigned long currentTime_;
    unsigned long highScore_;
    unsigned long lastTiltWarningTime_;
    unsigned long mostRecentRolloverTime_;
    unsigned long mostRecentSwitchHitTime_;

    void          resetMachineState();

  public:
    MachineState();
    boolean       allowRightDropTargetProgress();
    boolean       anyModeQualified();
    boolean       centaurDestroyed();
    boolean       currentPlayerTilted();
    boolean       destroyCentaurQualified();
    boolean       firstBallActive();
    boolean       guardianRolloversCompleted();
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
    boolean       rightDropTargetsResetQualified();
    boolean       samePlayerShootsAgain();
    boolean       scoreIncreasing();
    boolean       topRolloversCompleted();
    byte          activeOrbsDropTarget();
    byte          activeRightDropTarget();
    byte          bonus(byte player = 0xFF);
    byte          bonusMultiplier();
    byte          credits();
    byte          currentBallInPlay();
    byte          currentPlayerNumber();
    byte          increaseCurrentPlayer();
    byte          manageCreditButton(byte state);
    byte          mostRecentSwitchHit();
    byte          numberOfBallsInPlay();
    byte          numberOfBallsInTrough();
    byte          numberOfPlayers();
    byte          qualifiedScoreMultiplier();
    byte          queensChamberBonusValue();
    byte          startQualifiedMode();
    int           initGamePlay();
    int           initNewBall(bool curStateChanged);
    int           machineState();
    unsigned long currentTime();
    unsigned long dropRightDropTargets(unsigned long activationTime);
    unsigned long highScore();
    unsigned long lastTiltWarningTime();
    unsigned long mostRecentRolloverTime();
    unsigned long mostRecentSwitchHitTime();
    unsigned long queensChamberHurryUpValue();
    unsigned long queensChamberScoreValue();
    unsigned long resetInlineDropTargets(boolean activateSolenoid = false, boolean resetProgress = false, unsigned long activationTime = 0);
    unsigned long resetOrbsDropTargets  (boolean activateSolenoid = false, boolean resetProgress = false, unsigned long activationTime = 0);
    unsigned long resetRightDropTargets (boolean activateSolenoid = false, boolean resetProgress = false, unsigned long activationTime = 0);
    unsigned long rightDropTargetsScoreValue();
    unsigned long score(byte player = 0xFF);
    void          awardExtraBall();
    void          completeSelectedMode();
    void          decreaseBonus();
    void          decreaseNumberOfBallsInPlay();
    void          flashOrbsDropTargetsLamps();
    void          flashRightDropTargetsLamps();
    void          hideAllPlayerLamps();
    void          increaseBonus(byte amountToAdd = 1, boolean checkQueensChamberMultiplier = false);
    void          increaseBonusMultiplier();
    void          increaseCredits();
    void          increaseNumberOfBallsInPlay();
    void          increaseQualifiedScoreMultiplier();
    void          increaseScore(unsigned long amountToAdd, boolean checkQueensChamberMultiplier = false);
    void          launchBallIntoPlay(int lag = 0);
    void          manageCoinDrop(byte switchHit);
    void          manageInlineTargetScoring(byte switchHit);
    void          manageOrbsDropTargetScoring(byte switchHit);
    void          manageRightDropTargetScoring(byte switchHit);
    void          overridePlayerScore(unsigned long value);
    void          qualifyMode();
    void          qualifyRightDropTargetsReset();
    void          readStoredParameters();
    void          registerGuardianRollover(byte switchHit);
    void          registerInlineDropTarget(byte switchHit);
    void          registerOrbsDropTarget(byte switchHit);
    void          registerRightDropTarget(byte switchHit);
    void          registerTiltWarning();
    void          registerTopRollover(byte switchHit);
    void          resetCurrentBallInPlay();
    void          resetGuardianRollovers();
    void          resetTiltWarnings();
    void          resetTopRollovers();
    void          rotatePlayerLamps();
    void          rotateQualifiedMode();
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setCentaurDestoyed(boolean value);
    void          setCredits(byte value);
    void          setCurrentPlayer(byte value);
    void          setCurrentTime(unsigned long value);
    void          setHighScore(unsigned long value);
    void          setMachineState(int id);
    void          setMostRecentRolloverTime();
    void          setMostRecentSwitchHit(byte value);
    void          setMostRecentSwitchHitTime();
    void          setNumberOfPlayers(byte value);
    void          setPlayfieldValidated();
    void          setScore(unsigned long value, byte player = 0xFF);
    void          showAllPlayerLamps();
    void          spotRightDropTarget();
    void          unqualifyAllModes();
    void          unqualifyMode();
    void          unqualifyRightDropTargetsReset();
    void          updateBonusLamps();
    void          updateCaptiveOrbsLamps();
    void          updateGuardianRolloverLamps();
    void          updateOrbs50KLamp();
    void          updateOrbsDropTargetsLamps();
    void          updatePlayerScore(boolean flashCurrent = false, boolean dashCurrent = false);
    void          updateQueensChamberLamps();
    void          updateRightDropTargetsLamps();
    void          updateRightDropTargetsResetLamp();
    void          updateRightDropTargetsSpotLamp();
    void          updateRightOrbsReleaseLamp();
    void          updateScoreMultiplierLamps();
    void          updateTopRolloverLamps();
    void          writeCoinToAudit(byte switchHit);
};

#endif
