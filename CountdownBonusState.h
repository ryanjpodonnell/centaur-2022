#ifndef COUNTDOWN_BONUS_STATE_H
#define COUNTDOWN_BONUS_STATE_H

class CountdownBonus {
  private:
    unsigned long countdownEndTime_;
    unsigned long stateStartedTime_;

    void addBonusToScore();
    void countdownBonusStep();
    void handleNewState();

  public:
    CountdownBonus();

    int run(boolean currentStateChanged);
};

#endif
