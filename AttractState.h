#ifndef ATTRACT_STATE_H
#define ATTRACT_STATE_H

#define FEATURE_SHOW_WINDOW 60000
#define TAUNT_WINDOW        15000

class Attract {
  private:
    boolean       featureShowRunning_;
    boolean       updateScores_;
    byte          currentFeatureCycle_;
    unsigned long currentFlashCycle_;
    unsigned long featureShowStartedTime_;
    unsigned long lastFlash_;
    unsigned long lastTaunt_;
    unsigned long score1_;
    unsigned long score2_;
    unsigned long score3_;
    unsigned long score4_;

    boolean       featureShowEligible();
    boolean       tauntEligible();
    void          handleFeatureShow();
    void          handleLightShow();
    void          handleNewState();
    void          startFeatureShow();

  public:
    Attract();

    int run(int currentState, boolean currentStateChanged);
};

#endif
