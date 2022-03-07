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
    void dashScore(byte playerNumber, unsigned long score);
    void flashScore(byte playerNumber, unsigned long score);
    void scrollScore(byte playerNumber, unsigned long score);
    void showAnimatedPlayerScore(byte playerNumber, unsigned long score);
    void showPlayerScore(byte playerNumber, byte playerIterator = 0xFF, boolean flashCurrent = false, boolean dashCurrent = false);
    void showScoreOverride(byte playerNumber);

  public:
    DisplayHelper();

    void overrideCredits(byte value);
    void overrideScoreDisplay(byte playerNumber, unsigned long score);
    void showPlayerScores(byte playerNumber, boolean flashCurrent = false, boolean dashCurrent = false);
};

#endif
