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
    boolean       orbsDropTargetsCompletedInOrder_;
    boolean       rightDropTargetsCompletedInOrder_;
    boolean       rightDropTargetsResetQualified_;
    byte          activeInlineDropTarget_;
    byte          activeOrbsDropTarget_;
    byte          activeRightDropTarget_;
    byte          bonusMultiplier_;
    byte          bonus_;
    byte          displayNumber_;
    byte          qualifiedScoreMultiplier_;
    byte          queensChamberBonusValue_;
    byte          selectedMode_;
    unsigned long lastFlash_;
    unsigned long queensChamberScoreValue_;
    unsigned long rightDropTargetsScoreValue_;
    unsigned long score_;
    unsigned long tempScore_;
    boolean       guardianLights_[4];
    boolean       inlineDropTargets_[4];
    boolean       orbsDropTargets_[4];
    boolean       rightDropTargets_[4];
    boolean       topLaneLights_[3];
    byte          modeStatus_[4];

    void          launchLockedBallsIntoPlay();

  public:
    PlayerState();
    PlayerState(byte displayNumber);
    boolean       allModesQualified();
    boolean       allowRightDropTargetProgress();
    boolean       anyModeQualified();
    boolean       guardianRolloversCompleted();
    boolean       orbsDropTargetsAllStanding();
    boolean       orbsDropTargetsCompleted();
    boolean       orbsDropTargetsCompletedInOrder();
    boolean       rightDropTargetsAllStanding();
    boolean       rightDropTargetsCompleted();
    boolean       rightDropTargetsCompletedInOrder();
    boolean       rightDropTargetsResetQualified();
    boolean       scoreIncreasing();
    boolean       topRolloversCompleted();
    byte          activeOrbsDropTarget();
    byte          activeRightDropTarget();
    byte          bonus();
    byte          bonusMultiplier();
    byte          qualifiedScoreMultiplier();
    byte          queensChamberBonusValue();
    byte          startQualifiedMode();
    unsigned long dropRightDropTargets(unsigned long activationTime);
    unsigned long queensChamberHurryUpValue();
    unsigned long queensChamberScoreValue();
    unsigned long rightDropTargetsScoreValue();
    unsigned long score();
    void          completeSelectedMode();
    void          decreaseBonus();
    void          flashOrbsDropTargetsLamps();
    void          flashRightDropTargetsLamps();
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseBonusMultiplier();
    void          increaseQualifiedScoreMultiplier();
    void          increaseScore(unsigned long amountToAdd);
    void          manageInlineTargetScoring(byte switchHit);
    void          manageOrbsDropTargetScoring(byte switchHit);
    void          manageRightDropTargetScoring(byte switchHit);
    void          overridePlayerScore(unsigned long value);
    void          qualifyMode();
    void          qualifyRightDropTargetsReset();
    void          registerGuardianRollover(byte switchHit);
    void          registerInlineDropTarget(byte switchHit);
    void          registerOrbsDropTarget(byte switchHit);
    void          registerRightDropTarget(byte switchHit);
    void          registerTopRollover(byte switchHit);
    void          resetGuardianRollovers();
    void          resetInlineDropTargets();
    void          resetModeStatus();
    void          resetOrbsDropTargets();
    void          resetPlayerState();
    void          resetRightDropTargets();
    void          resetTopRollovers();
    void          rotatePlayerLamps();
    void          rotateQualifiedMode();
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setScore(unsigned long value);
    void          spotRightDropTarget();
    void          unqualifyMode();
    void          unqualifyRightDropTargetsReset();
    void          updateBonusLamps();
    void          updateCaptiveOrbsLamps();
    void          updateGuardianRolloverLamps();
    void          updateOrbsDropTargetsLamps();
    void          updatePlayerScore(boolean flashCurrent = false, boolean dashCurrent = false);
    void          updateQueensChamberLamps();
    void          updateRightDropTargetsLamps();
    void          updateRightDropTargetsResetLamp();
    void          updateRightDropTargetsSpotLamp();
    void          updateRightOrbsReleaseLamp();
    void          updateScoreMultiplierLamps();
    void          updateTopRolloverLamps();
};

#endif
