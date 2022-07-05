#ifndef ORB_MODE_1_H
#define ORB_MODE_1_H

#define ORB_MODE_1_MAX_SECONDS   60
#define ORB_MODE_1_TOTAL_SECONDS 15

class OrbMode1 {
  private:
    boolean       allowAddTime_;
    int           modeTotalTime_;
    int           secondsRemaining_;
    unsigned long modeStartedTime_;

    byte endMode();
    void manageBonusLightShow();
    void manageModeLamps();
    void manageNewMode();
    void manageTimeRemaining();

  public:
    OrbMode1();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
