#ifndef UNSTRUCTURED_PLAY_H
#define UNSTRUCTURED_PLAY_H

#define HURRY_UP_LENGTH       10000
#define HURRY_UP_GRACE_PERIOD 2000

class UnstructuredPlay {
  private:
    boolean       hurryUpActivated_;
    boolean       indicatorPlayed_;
    boolean       rerunOrbsSwitch_;
    boolean       rerunRightSwitch_;
    boolean       waitingToRerunOrbsSwitch_;
    boolean       waitingToRerunRightSwitch_;
    byte          rerunOrbsSwitchHit_;
    byte          rerunRightSwitchHit_;
    unsigned long hurryUpDisplayEndTime_;
    unsigned long hurryUpEndTime_;
    unsigned long hurryUpInitialValue_;
    unsigned long hurryUpMinimumValue_;
    unsigned long hurryUpStartedTime_;
    unsigned long hurryUpValuePerMillisecond_;
    unsigned long hurryUpValue_;
    unsigned long lastFlash_;
    unsigned long rerunOrbsSwitchTime_;
    unsigned long rerunRightSwitchTime_;

    void collectHurryUp();
    void endHurryUp();
    void manageHurryUp();
    void manageHurryUpCollect();
    void manageNewMode();
    void managePlayerBonusLamps();
    void manageShotIndicatorShow();
    void manageSwitchReruns();
    void startHurryUp(unsigned long value);
    void updateHurryUpLamps();
    void updateHurryUpValue();

  public:
    UnstructuredPlay();

    boolean       hurryUpActivated();
    byte          run(boolean currentStateChanged, byte switchHit);
    unsigned long hurryUpValue();
    void          endModeViaBallEnded();
};

#endif
