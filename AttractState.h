#ifndef ATTRACT_STATE_H
#define ATTRACT_STATE_H

class Attract {
  private:
    unsigned long currentFlashCycle_;
    unsigned long lastFlash_;
    unsigned long stateStatedTime_;

    void handleLightShow();
    void handleNewState();

  public:
    Attract();

    int run(int currentState, boolean currentStateChanged);
};

#endif
