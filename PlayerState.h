#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#define MAX_DISPLAY_BONUS 175

class PlayerState {
  private:
    boolean       guardianLights_[4];
    byte          bonusMultiplier_;
    byte          bonus_;
    unsigned long score_;

  public:
    PlayerState();
    byte          bonus();
    byte          bonusMultiplier();
    byte          playerNumber();
    unsigned long score();
    void          decreaseBonus(byte amountToSubtract = 1);
    void          increaseBonus(byte amountToAdd = 1);
    void          increaseScore(unsigned long amountToAdd);
    void          registerGuardianRollover(byte switchHit);
    void          resetPlayerState();
    void          rotatePlayerLamps();
    void          setBonus(byte value);
    void          setBonusMultiplier(byte value);
    void          setScore(unsigned long value);
    void          showPlayerLamps();
};

#endif
