#ifndef ORB_MODE_1_H
#define ORB_MODE_1_H

class OrbMode1 {
  private:
    byte secondsRemaining_;
    byte totalSeconds_;
    unsigned long modeStartedTime_;

    void handleNewMode();

  public:
    OrbMode1();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
