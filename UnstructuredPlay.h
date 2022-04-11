#ifndef UNSTRUCTURED_PLAY_H
#define UNSTRUCTURED_PLAY_H

#define HURRY_UP_GRACE_PERIOD           2000

class UnstructuredPlay {
  private:
    unsigned long currentFlashCycle_;
    unsigned long hurryUpEndTime_;
    unsigned long hurryUpInitialValue_;
    unsigned long hurryUpStartedTime_;
    unsigned long hurryUpValuePerMillisecond_;
    unsigned long lastFlash_;

    void          endHurryUp();
    void          handleHurryUp();
    void          handleNewMode();
    void          startHurryUp(unsigned long value, int seconds);
    void          updateHurryUpValue();

  public:
    UnstructuredPlay();

    byte run(boolean currentStateChanged, byte switchHit);
};

#endif
