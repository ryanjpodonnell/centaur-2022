#ifndef ATTRACT_STATE_H
#define ATTRACT_STATE_H

#define FEATURE_SHOW_TIMEOUT 60000
#define TAUNT_TIMEOUT        15000

class Attract {
  private:
    boolean       bonusShowRunning_;
    boolean       featureShowRunning_;
    boolean       updateScores_;
    byte          currentFeatureCycle_;
    unsigned long bonusLightShowEndTime_;
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
    void          handleBonusLightShow();
    void          handleFeatureShow();
    void          handleLightShow();
    void          handleNewState();
    void          startBonusLightsShow();
    void          startFeatureShow();

  public:
    Attract();

    int run(boolean currentStateChanged);
};

#endif
