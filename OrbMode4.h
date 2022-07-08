#ifndef ORB_MODE_4_H
#define ORB_MODE_4_H

#define ORB_MODE_4_MAX_SECONDS     60
#define ORB_MODE_4_INITIAL_SECONDS 30

class OrbMode4 {
  private:
    boolean       allowAddTime_;
    byte          currentJackpotTarget_;
    int           secondsRemaining_;
    int           totalTime_;
    unsigned long jackpotValue_;
    unsigned long lastFlash_;
    unsigned long resetInlineDropsTime_;
    unsigned long resetOrbsDropsTime_;
    unsigned long resetRightDropsTime_;
    unsigned long startedTime_;

    byte endMode();
    void manageDropsReset();
    void manageModeLamps();
    void manageNewMode();
    void manageTimeRemaining();

  public:
    OrbMode4();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
