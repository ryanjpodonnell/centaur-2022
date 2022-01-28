#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF

class DisplayHelper {
  private:
    boolean       scoreOverrideAnimationStatus_[4];
    boolean       scoreOverrideStatus_[4];
    boolean       updateLastTimeAnimated_;
    byte          lastScrollPhase_;
    unsigned long lastTimeOverrideAnimated_;
    unsigned long lastTimeScoreChanged_;
    unsigned long overrideAnimationSeed_;
    unsigned long scoreOverrideValue_[4];

    byte getDisplayMask(byte numDigits);
    byte magnitudeOfScore(unsigned long score);
    byte numberOfDigits(unsigned long score);
    void overrideScoreDisplay(byte displayNum, unsigned long value, boolean animate);
    void showAnimatedPlayerScore(byte displayToUpdate, unsigned long value);

  public:
    DisplayHelper();

    void showPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0);
    void startScoreAnimation(unsigned long scoreToAnimate);
};

#endif
