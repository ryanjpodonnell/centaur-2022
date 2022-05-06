#ifndef COUNTDOWN_BONUS_STATE_H
#define COUNTDOWN_BONUS_STATE_H

class CountdownBonus {
  private:
    int stepDuration_;

    unsigned long countdownEndTime_;
    unsigned long lastDecrease_;
    unsigned long lastFlash_;
    unsigned long stateStartedTime_;
    unsigned long stepDurationChangedTime_;

    void addBonusToScore();
    void countdownBonusStep();
    void handleBonusLightShow();
    void handleNewState();

  public:
    CountdownBonus();

    int run(boolean currentStateChanged);
};

#endif
