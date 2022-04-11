#ifndef ATTRACT_STATE_H
#define ATTRACT_STATE_H

class Attract {
  private:
    boolean       updateScores_;
    unsigned long currentFlashCycle_;
    unsigned long lastFlash_;
    unsigned long score1_;
    unsigned long score2_;
    unsigned long score3_;
    unsigned long score4_;
    void          handleLightShow();
    void          handleNewState();

  public:
    Attract();

    int run(int currentState, boolean currentStateChanged);
};

#endif
