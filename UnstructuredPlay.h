#ifndef UNSTRUCTURED_PLAY_H
#define UNSTRUCTURED_PLAY_H

class UnstructuredPlay {
  private:
    boolean       rerunOrbsSwitch_;
    boolean       rerunRightSwitch_;
    byte          rerunOrbsSwitchHit_;
    byte          rerunRightSwitchHit_;
    unsigned long rerunOrbsSwitchTime_;
    unsigned long rerunRightSwitchTime_;

    void manageNewMode();
    void manageSwitchReruns();

  public:
    UnstructuredPlay();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
