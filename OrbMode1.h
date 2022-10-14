#ifndef ORB_MODE_1_H
#define ORB_MODE_1_H

#define ORB_MODE_1_MAX_SECONDS     99
#define ORB_MODE_1_INITIAL_SECONDS 46

class OrbMode1 {
  private:
    byte          currentJackpotTarget_;
    int           secondsRemaining_;
    unsigned long jackpotValue_;
    unsigned long lastFlash_;

    byte endMode();
    void manageModeLamps();
    void manageNewMode();
    void manageTimeRemaining();

  public:
    OrbMode1();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
