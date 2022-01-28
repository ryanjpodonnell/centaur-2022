#include "SharedVariables.h"

DisplayHelper::DisplayHelper() {
  lastScrollPhase_ = 0;
  lastTimeOverrideAnimated_ = 0;
  lastTimeScoreChanged_ = 0;
  unsigned long scoreOverrideValue_[] = { 0, 0, 0, 0 };
  unsigned long scoreOverrideStatus_[] = { false, false, false, false };
  unsigned long scoreOverrideAnimationStatus_[] = { false, false, false, false };
}

byte DisplayHelper::getDisplayMask(byte numDigits) {
  byte displayMask = 0;
  for (byte digitCount = 0; digitCount < numDigits; digitCount++) {
    displayMask |= (0x40 >> digitCount);
  }
  return displayMask;
}

byte DisplayHelper::magnitudeOfScore(unsigned long score) {
  if (score == 0) return 0;

  byte magnitudeOfScore = 0;
  while (score > 0) {
    score = score / 10;
    magnitudeOfScore += 1;
  }

  return magnitudeOfScore;
}

byte DisplayHelper::numberOfDigits(unsigned long score) {
  byte numDigits = magnitudeOfScore(score);
  if (numDigits == 0) return 1;

  return numDigits;
}

void DisplayHelper::overrideScoreDisplay(byte displayNum, unsigned long value, boolean animate) {
  scoreOverrideStatus_[displayNum] = true;
  if (animate) scoreOverrideStatus_[displayNum] = true;

  scoreOverrideValue_[displayNum] = value;
}

void DisplayHelper::startScoreAnimation(unsigned long scoreToAnimate) {
  if (ScoreAdditionAnimation != 0) {
    g_machineState.increaseScore(ScoreAdditionAnimation);
  }
  unsigned long currentTime = g_machineState.currentTime();
  ScoreAdditionAnimation = scoreToAnimate;
  ScoreAdditionAnimationStartTime = currentTime;
  LastRemainingAnimatedScoreShown = 0;
}

void DisplayHelper::showAnimatedPlayerScore(byte displayToUpdate, unsigned long value) {
  if (overrideAnimationSeed_ != lastTimeOverrideAnimated_) {
    byte numDigits = numberOfDigits(value);
    updateLastTimeAnimated_ = true;

    byte shiftDigits = (overrideAnimationSeed_) % (((BALLY_STERN_OS_NUM_DIGITS + 1) - numDigits) + ((BALLY_STERN_OS_NUM_DIGITS - 1) - numDigits));
    if (shiftDigits >= ((BALLY_STERN_OS_NUM_DIGITS + 1) - numDigits)) shiftDigits = (BALLY_STERN_OS_NUM_DIGITS - numDigits) * 2 - shiftDigits;
    byte digitCount;
    byte displayMask = getDisplayMask(numDigits);
    for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
      value *= 10;
      displayMask = displayMask >> 1;
    }
    BSOS_SetDisplayBlank(displayToUpdate, 0x00);
    BSOS_SetDisplay(displayToUpdate, value, false);
    BSOS_SetDisplayBlank(displayToUpdate, displayMask);
  }
}

void DisplayHelper::showPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue) {
  if (displayToUpdate == 0xFF) unsigned long scoreOverrideStatus_[] = { false, false, false, false };
  byte          displayMask = 0x7F;
  byte          scrollPhaseChanged = false;
  unsigned long displayScore = 0;
  unsigned long currentTime = g_machineState.currentTime();
  unsigned long timeSinceLastScoreChange = currentTime - lastTimeScoreChanged_;

  overrideAnimationSeed_  = currentTime / 250;
  updateLastTimeAnimated_ = false;

  byte scrollPhase = (timeSinceLastScoreChange / 250) % 16;
  if (scrollPhase != lastScrollPhase_) {
    lastScrollPhase_ = scrollPhase;
    scrollPhaseChanged = true;
  }

  for (byte scoreCount = 0; scoreCount < 4; scoreCount++) {
    if (allScoresShowValue == 0 && scoreOverrideStatus_[scoreCount]) {
      displayScore = scoreOverrideValue_[scoreCount];
      if (displayScore == DISPLAY_OVERRIDE_BLANK_SCORE) {
        BSOS_SetDisplayBlank(scoreCount, 0);
      } else {
        byte numDigits = numberOfDigits(displayScore);
        if (numDigits < (BALLY_STERN_OS_NUM_DIGITS - 1) && scoreOverrideAnimationStatus_[scoreCount]) {
          showAnimatedPlayerScore(scoreCount, displayScore);
        } else {
          BSOS_SetDisplay(scoreCount, displayScore, true, 1);
        }
      }
    } else {
      // No override, update scores designated by displayToUpdate
      //CurrentScores[CurrentPlayer] = CurrentScoreOfCurrentPlayer;
      if (allScoresShowValue == 0) displayScore = g_machineState.score(scoreCount);
      else displayScore = allScoresShowValue;

      // If we're updating all displays, or the one currently matching the loop, or if we have to scroll
      if (displayToUpdate == 0xFF || displayToUpdate == scoreCount || displayScore > BALLY_STERN_OS_MAX_DISPLAY_SCORE) {

        // Don't show this score if it's not a current player score (even if it's scrollable)
        if (displayToUpdate == 0xFF && (scoreCount >= g_machineState.numberOfPlayers() && g_machineState.numberOfPlayers() != 0) && allScoresShowValue == 0) {
          BSOS_SetDisplayBlank(scoreCount, 0x00);
          continue;
        }

        if (displayScore > BALLY_STERN_OS_MAX_DISPLAY_SCORE) {
          // Score needs to be scrolled
          if ((currentTime - lastTimeScoreChanged_) < 4000) {
            BSOS_SetDisplay(scoreCount, displayScore % (BALLY_STERN_OS_MAX_DISPLAY_SCORE + 1), false);
            BSOS_SetDisplayBlank(scoreCount, BALLY_STERN_OS_ALL_DIGITS_MASK);
          } else {
            // Scores are scrolled 10 digits and then we wait for 6
            if (scrollPhase < 11 && scrollPhaseChanged) {
              byte numDigits = magnitudeOfScore(displayScore);

              // Figure out top part of score
              unsigned long tempScore = displayScore;
              if (scrollPhase < BALLY_STERN_OS_NUM_DIGITS) {
                displayMask = BALLY_STERN_OS_ALL_DIGITS_MASK;
                for (byte scrollCount = 0; scrollCount < scrollPhase; scrollCount++) {
                  displayScore = (displayScore % (BALLY_STERN_OS_MAX_DISPLAY_SCORE + 1)) * 10;
                  displayMask = displayMask >> 1;
                }
              } else {
                displayScore = 0;
                displayMask = 0x00;
              }

              // Add in lower part of score
              if ((numDigits + scrollPhase) > 10) {
                byte numDigitsNeeded = (numDigits + scrollPhase) - 10;
                for (byte scrollCount = 0; scrollCount < (numDigits - numDigitsNeeded); scrollCount++) {
                  tempScore /= 10;
                }
                displayMask |= getDisplayMask(magnitudeOfScore(tempScore));
                displayScore += tempScore;
              }
              BSOS_SetDisplayBlank(scoreCount, displayMask);
              BSOS_SetDisplay(scoreCount, displayScore);
            }
          }
        } else {
          if (flashCurrent) {
            unsigned long flashSeed = currentTime / 250;
            if (flashSeed != LastFlashOrDash) {
              LastFlashOrDash = flashSeed;
              if (((currentTime / 250) % 2) == 0) BSOS_SetDisplayBlank(scoreCount, 0x00);
              else BSOS_SetDisplay(scoreCount, displayScore, true, 2);
            }
          } else if (dashCurrent) {
            unsigned long dashSeed = currentTime / 50;
            if (dashSeed != LastFlashOrDash) {
              LastFlashOrDash = dashSeed;
              byte dashPhase = (currentTime / 60) % 36;
              byte numDigits = magnitudeOfScore(displayScore);
              if (dashPhase < 12) {
                displayMask = getDisplayMask((numDigits == 0) ? 2 : numDigits);
                if (dashPhase < 7) {
                  for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
                    displayMask &= ~(0x01 << maskCount);
                  }
                } else {
                  for (byte maskCount = 12; maskCount > dashPhase; maskCount--) {
                    displayMask &= ~(0x20 >> (maskCount - dashPhase - 1));
                  }
                }
                BSOS_SetDisplay(scoreCount, displayScore);
                BSOS_SetDisplayBlank(scoreCount, displayMask);
              } else {
                BSOS_SetDisplay(scoreCount, displayScore, true, 2);
              }
            }
          } else {
            BSOS_SetDisplay(scoreCount, displayScore, true, 2);
          }
        }
      } // End if this display should be updated
    } // End on non-overridden
  } // End loop on scores

  if (updateLastTimeAnimated_) {
    lastTimeOverrideAnimated_ = overrideAnimationSeed_;
  }
}
