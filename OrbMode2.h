#ifndef ORB_MODE_2_H
#define ORB_MODE_2_H

#define ORB_MODE_2_MAX_SECONDS     99
#define ORB_MODE_2_INITIAL_SECONDS 46

class OrbMode2 {
  private:
    int           secondsRemaining_;
    unsigned long jackpotValue_;
    unsigned long lastFlash_;
    unsigned long resetInlineDropsTime_;
    unsigned long resetOrbsDropsTime_;
    unsigned long resetRightDropsTime_;

    byte endMode();
    void manageDropsReset();
    void manageNewMode();
    void manageTimeRemaining();

  public:
    OrbMode2();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
