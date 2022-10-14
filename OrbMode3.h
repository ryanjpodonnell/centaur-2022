#ifndef ORB_MODE_3_H
#define ORB_MODE_3_H

#define ORB_MODE_3_MAX_SECONDS     99
#define ORB_MODE_3_INITIAL_SECONDS 46

class OrbMode3 {
  private:
    int           secondsRemaining_;
    unsigned long jackpotValue_;
    unsigned long lastFlash_;

    byte endMode();
    void manageDropsReset();
    void manageNewMode();
    void manageTimeRemaining();

  public:
    OrbMode3();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
