#ifndef ORB_MODE_3_H
#define ORB_MODE_3_H

#define ORB_MODE_3_MAX_SECONDS     60
#define ORB_MODE_3_INITIAL_SECONDS 30

class OrbMode3 {
  private:
    boolean       allowAddTime_;
    int           secondsRemaining_;
    int           totalTime_;
    unsigned long jackpotValue_;
    unsigned long startedTime_;

    byte endMode();
    void manageDropsReset();
    void manageModeLamps();
    void manageNewMode();
    void manageTimeRemaining();

  public:
    OrbMode3();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
