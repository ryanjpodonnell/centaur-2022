#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#define MAX_BONUS_MULTIPLIER      5
#define MAX_DISPLAY_BONUS         175
#define MODE_STATUS_NOT_QUALIFIED 0
#define MODE_STATUS_QUALIFIED     1
#define MODE_STATUS_STARTED       2
#define MODE_STATUS_COMPLETED     3

class PlayerState {
  private:
    byte          bonusMultiplier_;
    byte          bonus_;
    byte          displayNumber_;
    byte          selectedMode_;

    unsigned long score_;

    boolean       guardianLights_[4];
    boolean       orbsDropTargets_[4];
    boolean       topLaneLights_[3];
    byte          modeStatus_[4];

    boolean       guardianRolloversCompleted();
    boolean       topRolloversCompleted();

  public:
    PlayerState();
    PlayerState(byte displayNumber);

    boolean       allModesQualified();
    boolean       anyModeQualified();
    boolean       orbsDropTargetsCompleted();

    byte          bonus();
    byte          bonusMultiplier();
    byte          startQualifiedMode();
    void          increaseBonusMultiplier();

    unsigned long score();

    void          decreaseBonus(byte amountToSubtract = 1);
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseScore(unsigned long amountToAdd);
    void          qualifyMode();
    void          registerDropTarget(byte switchHit);
    void          registerGuardianRollover(byte switchHit);
    void          registerTopRollover(byte switchHit);
    void          resetDropTargets();
    void          resetPlayerState();
    void          rotatePlayerLamps();
    void          rotateQualifiedMode();
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setScore(unsigned long value);
    void          updateBonusLamps();
    void          updateCaptiveOrbsLamps();
    void          updateGuardianRolloverLamps();
    void          updateOrbsDropTargetLamps();
    void          updatePlayerScore(boolean flashCurrent = false, boolean dashCurrent = false);
    void          updateSelectedMode();
    void          updateTopRolloverLamps();
};

#endif
