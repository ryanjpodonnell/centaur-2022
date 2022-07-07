#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF

class DisplayHelper {
  private:
    boolean       updateLastTimeAnimated_;
    byte          lastScrollPhase_;
    unsigned long lastFlashOrDash_;
    unsigned long lastTimeOverrideAnimated_;
    unsigned long overrideAnimationSeed_;

    boolean       scoreOverrideStatus_[4];
    unsigned long scoreOverrideValue_[4];

    byte getDisplayMask(byte numDigits);
    byte magnitudeOfScore(unsigned long score);
    byte numberOfDigits(unsigned long score);
    void dashScore(byte displayNumber, unsigned long score);
    void flashScore(byte displayNumber, unsigned long score);
    void scrollScore(byte displayNumber, unsigned long score);
    void showAnimatedPlayerScore(byte displayNumber, unsigned long score);
    void showPlayerScore(byte displayNumber, byte playerIterator = 0xFF, boolean flashCurrent = false, boolean dashCurrent = false);
    void showScoreOverride(byte displayNumber);

  public:
    DisplayHelper();

    void overrideScoreDisplay(byte displayNumber, unsigned long score);
    void showPlayerScores(byte displayNumber, boolean flashCurrent = false, boolean dashCurrent = false);
};

#endif
