#ifndef ORB_MODE_4_H
#define ORB_MODE_4_H

#define ORB_MODE_4_MAX_SECONDS     99
#define ORB_MODE_4_INITIAL_SECONDS 46

class OrbMode4 {
  private:
    byte          currentJackpotTarget_;
    int           secondsRemaining_;
    unsigned long jackpotValue_;
    unsigned long lastFlashRovingTarget_;
    unsigned long lastFlashSecondsRemaining_;
    unsigned long resetInlineDropsTime_;
    unsigned long resetOrbsDropsTime_;
    unsigned long resetRightDropsTime_;

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
