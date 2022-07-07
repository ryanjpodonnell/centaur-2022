#ifndef ORB_MODE_2_H
#define ORB_MODE_2_H

#define ORB_MODE_2_MAX_SECONDS     60
#define ORB_MODE_2_INITIAL_SECONDS 30

class OrbMode2 {
  private:
    boolean       allowAddTime_;
    int           secondsRemaining_;
    int           totalTime_;
    unsigned long jackpotValue_;
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
    OrbMode2();

    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long jackpotValue();
    void          endModeViaBallEnded();
};

#endif
