#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#define MAX_BONUS_MULTIPLIER           5
#define MAX_DISPLAY_BONUS              175
#define MAX_MODE_MULTIPLIER            5
#define MAX_QUEENS_CHAMBER_BONUS_VALUE 5
#define MAX_QUEENS_CHAMBER_SCORE_VALUE 50000

#define MODE_STATUS_NOT_QUALIFIED 0
#define MODE_STATUS_QUALIFIED     1
#define MODE_STATUS_STARTED       2
#define MODE_STATUS_COMPLETED     3

class PlayerState {
  private:
    boolean       modeMultiplierQualified_;
    boolean       orbsDropTargetsCompletedInOrder_;
    boolean       rightDropTargetsCompletedInOrder_;
    byte          activeOrbsDropTarget_;
    byte          activeRightDropTarget_;
    byte          bonusMultiplier_;
    byte          bonus_;
    byte          displayNumber_;
    byte          modeMultiplier_;
    byte          queensChamberBonusValue_;
    byte          selectedMode_;
    unsigned long lastFlash_;
    unsigned long queensChamberScoreValue_;
    unsigned long rightDropTargetsFinishedTime_;
    unsigned long rightDropTargetsScoreValue_;
    unsigned long score_;
    unsigned long tempScore_;
    boolean       guardianLights_[4];
    boolean       orbsDropTargets_[4];
    boolean       rightDropTargets_[4];
    boolean       topLaneLights_[3];
    byte          modeStatus_[4];

    boolean       anyModeStarted();
    void          launchLockedBallsIntoPlay();
    void          manageOrbsDropTargetScoring(byte switchHit);
    void          manageRightDropTargetScoring(byte switchHit);
    void          manageSpotRightDropTarget();

  public:
    PlayerState();
    PlayerState(byte displayNumber);
    boolean       allModesQualified();
    boolean       allowRightDropTargetProgress();
    boolean       anyModeQualified();
    boolean       guardianRolloversCompleted();
    boolean       modeMultiplierQualified();
    boolean       orbsDropTargetsAllStanding();
    boolean       orbsDropTargetsCompleted();
    boolean       orbsDropTargetsCompletedInOrder();
    boolean       rightDropTargetsAllStanding();
    boolean       rightDropTargetsCompleted();
    boolean       rightDropTargetsCompletedInOrder();
    boolean       scoreIncreasing();
    boolean       topRolloversCompleted();
    byte          bonus();
    byte          bonusMultiplier();
    byte          queensChamberBonusValue();
    byte          startQualifiedMode();
    unsigned long queensChamberScoreValue();
    unsigned long rightDropTargetsFinishedTime();
    unsigned long rightDropTargetsScoreValue();
    unsigned long score();
    void          decreaseBonus(byte amountToSubtract = 1);
    void          decreaseModeMultiplier();
    void          dropRightDropTargets();
    void          flashOrbsDropTargetLamps();
    void          flashRightDropTargetLamps();
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseBonusMultiplier();
    void          increaseModeMultiplier();
    void          increaseQueensChamberBonusValue();
    void          increaseQueensChamberScoreValue();
    void          increaseScore(unsigned long amountToAdd);
    void          overridePlayerScore(unsigned long value);
    void          qualifyMode();
    void          qualifyModeMultiplier();
    void          registerGuardianRollover(byte switchHit);
    void          registerOrbsDropTarget(byte switchHit);
    void          registerRightDropTarget(byte switchHit);
    void          registerTopRollover(byte switchHit);
    void          resetGuardianRollovers();
    void          resetModeStatus();
    void          resetOrbsDropTargets(boolean activateSolenoid = false);
    void          resetPlayerState();
    void          resetQueensChamberBonusValue();
    void          resetQueensChamberScoreValue();
    void          resetRightDropTargets(boolean activateSolenoid = false);
    void          resetTopRollovers();
    void          rotatePlayerLamps();
    void          rotateQualifiedMode();
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setRightDropTargetsFinishedTime();
    void          setScore(unsigned long value);
    void          unqualifyMode();
    void          updateBonusLamps();
    void          updateCaptiveOrbsLamps();
    void          updateGuardianRolloverLamps();
    void          updateModeMultiplierLamps();
    void          updateOrbsDropTargetLamps();
    void          updatePlayerScore(boolean flashCurrent = false, boolean dashCurrent = false);
    void          updateQueensChamberLamps();
    void          updateRightDropTargetLamps();
    void          updateRightDropTargetResetLamp();
    void          updateRightDropTargetSpotLamp();
    void          updateRightOrbsReleaseLamp();
    void          updateSelectedMode();
    void          updateTopRolloverLamps();
};

#endif
