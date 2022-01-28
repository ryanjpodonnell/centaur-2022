#ifndef ATTRACT_STATE_H
#define ATTRACT_STATE_H

class Attract {
  private:
    unsigned long lastFlash_;
    unsigned long stateStatedTime_;

  public:
    Attract();

    byte run(byte currentState, boolean currentStateChanged);
};

#endif
