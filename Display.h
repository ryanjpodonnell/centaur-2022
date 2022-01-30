#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF

class DisplayHelper {
  private:
    boolean       scoreOverrideAnimationStatus_[4];
    boolean       scoreOverrideStatus_[4];
    boolean       updateLastTimeAnimated_;
    byte          lastScrollPhase_;
    unsigned long lastFlashOrDash_;
    unsigned long lastTimeOverrideAnimated_;
    unsigned long lastTimeScoreChanged_;
    unsigned long overrideAnimationSeed_;
    unsigned long scoreOverrideValue_[4];

    byte getDisplayMask(byte numDigits);
    byte magnitudeOfScore(unsigned long score);
    byte numberOfDigits(unsigned long score);
    void dashScore(byte playerNumber, unsigned long score);
    void flashScore(byte playerNumber, unsigned long score);
    void scrollScore(byte playerNumber, unsigned long score);
    void showAnimatedPlayerScore(byte playerNumber, unsigned long score);
    void showScoreOverride(byte playerNumber);

  public:
    DisplayHelper();

    void overrideScoreDisplay(byte playerNumber, unsigned long score, boolean animate);
    void showPlayerScore(byte playerNumber, byte playerIterator = 0xFF, boolean flashCurrent = false, boolean dashCurrent = false, unsigned long allScoresShowValue = 0);
    void showPlayerScores(byte playerNumber, boolean flashCurrent = false, boolean dashCurrent = false, unsigned long allScoresShowValue = 0);
    void startScoreAnimation(unsigned long scoreToAnimate);
};

#endif
