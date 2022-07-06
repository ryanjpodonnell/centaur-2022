#ifndef ORB_MODE_1_H
#define ORB_MODE_1_H

#define ORB_MODE_1_MAX_SECONDS     60
#define ORB_MODE_1_INITIAL_SECONDS 30

class OrbMode1 {
  private:
    boolean       allowAddTime_;
    byte          currentJackpotTarget_;
    int           secondsRemaining_;
    int           totalTime_;
    unsigned long jackpotValue_;
    unsigned long startedTime_;

    byte endMode();
    void manageBonusLightShow();
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
